/*	te_macro.c

	Text editor.

	Macros.

	Copyright (c) 2015-2021 Miguel Garcia / FloppySoftware

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Changes:

	30 Jan 2018 : Extracted from te.c.
	20 Feb 2018 : Disable code for macros from strings, for now.
	26 Dec 2018 : Allow # of repeats in macros - ie {up:12}. Rename MacroGetCh() to MacroGet().
	29 Dec 2018 : Added MacroRunning().
	26 Dec 2019 : Now K_INTRO is K_CR.
	01 Jul 2021 : Change macro symbol names to match key bindings names.
	06 Jul 2021 : Optimize MacroGet() a bit.
	25 Sep 2021 : Added MacroIsCmdChar(). Allow comments as {# comment}. Send '\0' from MacroStop().
	01 Nov 2021 : Modified MacroRunFile() to set raw mode and return success / error flag.
	              Modified MacroGetRaw() and MacroGet() to support raw mode.
				  Check in MacroGetRaw() for illegal characters.
*/

/* Run a macro from file
   ---------------------
   Returns NZ on error.
*/
MacroRunFile(fname, raw)
char *fname;
int raw;
{
	if(!(mac_fp = fopen(fname, "r")))
	{
		ErrLineOpen();
		
		return -1;
	}
	
	mac_raw = raw;
	
	return 0;
}

/* Run a macro from string
   -----------------------
*/
/*
MacroRunStr(s)
char *s;
{
	mac_str = s;
}
*/

/* Tell if a macro is running
   --------------------------
*/
MacroRunning()
{
	return mac_fp != NULL /* || mac_str != NULL */;
}

/* Stop a macro
   ------------
*/
MacroStop()
{
	if(mac_fp)
	{
		fclose(mac_fp);
	}

	mac_fp = /*mac_str =*/ NULL;

	/* Flag end of input */
	ForceCh('\0');
}

/* Read raw character from macro input
   -----------------------------------
*/
MacroGetRaw()
{
	int ch;

	if(mac_fp)
	{
		if(mac_raw)
		{
			/* Raw mode - translate some control chars. */
			switch(ch = fgetc(mac_fp))
			{
				case '\n' : ch = K_CR; break;
				case '\t' : ch = ' '; break;
			}
		}
		else
		{
			/* Normal mode: ignore new-lines */
			while((ch = fgetc(mac_fp)) == '\n')
				;
		}

		if(ch != EOF)
		{
			/* Translate control chars. */
			if(ch < 32 || ch == 127)
			{
				ch = '?';
			}
			
			return ch;
		}

		MacroStop();
	}
	/*
	else if(mac_str)
	{
		if(*mac_str)
		{
			return *mac_str++;
		}

		MacroStop();
	}
	*/

	/* No character available */
	return '\0';
}

/* Check if a character is legal for symbol name
   ---------------------------------------------
*/
MacroIsCmdChar(ch)
char ch;
{
	return isalpha(ch) || ch == '#';
}

/* Process a macro input unit
   --------------------------
*/
MacroGet()
{
	int i, n, ch;
	char sym[MAC_SYM_SIZ];

	/* Continue if there is a character available */
	if((ch = MacroGetRaw()))
	{
		/* Return character if raw mode */
		if(mac_raw)
		{
			ForceCh(ch);
			
			return;
		}
		
		/* Return character if it's not the start of a symbol */
		if(ch != MAC_START)
		{
			/* Check for escaped characters */
			if(ch != MAC_ESCAPE)
			{
				ForceCh(ch);
			}
			else
			{
				if((ch = MacroGetRaw()))
				{
					ForceCh(ch);
				}
				else
				{
					/* Error: missing escaped character */
					ErrLine("Bad escape sequence");

					MacroStop();
				}
			}

			return;
		}

		/* Get symbol name like {up} or {up:12} --> "up" */
		for(i = 0; MacroIsCmdChar(ch = MacroGetRaw()) && i < MAC_SYM_MAX; ++i)
		{
			sym[i] = tolower(ch);
		}

		if(i)
		{
			/* End of symbol name */
			sym[i] = '\0';

			/* Get # of repeats if any - ie: {up:12} --> 12 */
			if(ch == MAC_SEP)
			{
				n = 0;

				while(isdigit(ch = MacroGetRaw()))
					n = n * 10 + ch - '0';

				if(n < 0 || n > FORCED_MAX)
				{
					n = 0;
				}
			}
			else
			{
				n = 1;
			}

			if(n)
			{
				/* Check for comments */
				if(ch == ' ')
				{
					if((MatchStr(sym, "#")))
					{
						while((ch = MacroGetRaw()))
						{
							if(ch == MAC_END)
							{
								ForceCh('\0');

								return;
							}
						}
					}
				}

				/* Check for commands */
				if(ch == MAC_END)
				{
					/* Do command action */
					ch = 0;

					if     (MatchStr(sym, "up"))         ch = K_UP;
					else if(MatchStr(sym, "down"))       ch = K_DOWN;
					else if(MatchStr(sym, "left"))       ch = K_LEFT;
					else if(MatchStr(sym, "right"))      ch = K_RIGHT;
					else if(MatchStr(sym, "begin"))      ch = K_BEGIN;
					else if(MatchStr(sym, "end"))        ch = K_END;
					else if(MatchStr(sym, "top"))        ch = K_TOP;
					else if(MatchStr(sym, "bottom"))     ch = K_BOTTOM;
					else if(MatchStr(sym, "newline"))    ch = K_CR;
					else if(MatchStr(sym, "indent"))     ch = K_TAB;
					else if(MatchStr(sym, "delright"))   ch = K_RDEL;
					else if(MatchStr(sym, "delleft"))    ch = K_LDEL;
					else if(MatchStr(sym, "cut"))        ch = K_CUT;
					else if(MatchStr(sym, "copy"))       ch = K_COPY;
					else if(MatchStr(sym, "paste"))      ch = K_PASTE;
					else if(MatchStr(sym, "delete"))     ch = K_DELETE;
					else if(MatchStr(sym, "clearclip"))  ch = K_CLRCLP;

#if OPT_BLOCK
					else if(MatchStr(sym, "blockstart")) ch = K_BLK_START;
					else if(MatchStr(sym, "blockend"))   ch = K_BLK_END;
#endif

					if(ch)
					{
						while(n--)
						{
							if(ForceCh(ch))
								break;
						}

						return;
					}

					/* Special commands */
					if(MatchStr(sym, "filename"))
					{
						while(n--)
							ForceStr(CurrentFile());

						return;
					}
				}
			}
		}

		/* Error: symbol name not found, bad formed, too large, bad # of repeats */
		ErrLine("Bad symbol");

		MacroStop();
	}
}

