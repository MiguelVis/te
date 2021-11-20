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
	18 Nov 2021 : Added {AutoIndent}, {AutoList}.
	20 Nov 2021 : Added MatchSym().
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
	
	/* Reset auto-indentation and auto-list */
	mac_indent = cf_indent;
	mac_list = cf_list;
	
	cf_indent = cf_list = 0;
	
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
	
	/* Restore auto-indentation and auto-list */
	cf_indent = mac_indent;
	cf_list = mac_list;

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
	return isalpha(ch) || ch == '#' || ch == '+' || ch == '-';
}

/* Check if a string is a symbol
   -----------------------------
*/
MatchSym(s)
char *s;
{
	return MatchStr(mac_sym, s);
}

/* Process a macro input unit
   --------------------------
*/
MacroGet()
{
	int i, n, ch;

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
			mac_sym[i] = tolower(ch);
		}

		if(i)
		{
			/* End of symbol name */
			mac_sym[i] = '\0';

			/* Get # of repeats if any - ie: {up:12} --> 12 */
			if(ch == MAC_SEP)
			{
				n = 0;

				while(isdigit(ch = MacroGetRaw()))
					n = n * 10 + ch - '0';

				if(n < 0 || n > FORCED_MAX)
				{
					n = -1;
				}
			}
			else
			{
				n = 1;
			}

			if(n >= 0)
			{
				/* Check for comments */
				if(ch == ' ')
				{
					if((MatchSym("#")))
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

					if     (MatchSym("up"))         ch = K_UP;
					else if(MatchSym("down"))       ch = K_DOWN;
					else if(MatchSym("left"))       ch = K_LEFT;
					else if(MatchSym("right"))      ch = K_RIGHT;
					else if(MatchSym("begin"))      ch = K_BEGIN;
					else if(MatchSym("end"))        ch = K_END;
					else if(MatchSym("top"))        ch = K_TOP;
					else if(MatchSym("bottom"))     ch = K_BOTTOM;
					else if(MatchSym("newline"))    ch = K_CR;
					else if(MatchSym("indent"))     ch = K_TAB;
					else if(MatchSym("delright"))   ch = K_RDEL;
					else if(MatchSym("delleft"))    ch = K_LDEL;
					else if(MatchSym("cut"))        ch = K_CUT;
					else if(MatchSym("copy"))       ch = K_COPY;
					else if(MatchSym("paste"))      ch = K_PASTE;
					else if(MatchSym("delete"))     ch = K_DELETE;
					else if(MatchSym("clearclip"))  ch = K_CLRCLP;

#if OPT_BLOCK
					else if(MatchSym("blockstart")) ch = K_BLK_START;
					else if(MatchSym("blockend"))   ch = K_BLK_END;
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
					if(MatchSym("filename"))
					{
						while(n--)
							ForceStr(CurrentFile());

						return;
					}
					else if(MatchSym("autoindent")) {
						cf_indent = (n ? 1 : 0);
						
						ForceCh('\0');
						
						return;
					}
					else if(MatchSym("autolist")) {
						cf_list = (n ? 1 : 0);
						
						ForceCh('\0');
						
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