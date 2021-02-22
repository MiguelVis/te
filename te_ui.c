/*	te_ui.c

	Text editor.

	User interface.

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
	22 Feb 2018 : Ask for confirmation only if changes were not saved. INTRO equals Y on confirmation.
	16 Jan 2019 : Modified Refresh() to show block selection. Added RefreshBlock().
	19 Jan 2019 : Added ShowFilename().
	23 Jan 2019 : Refactorized MenuHelp().
	30 Jan 2019 : Added putchrx().
	24 Dec 2019 : Modified some text messages. SysLineKey() is now SysLineCont(). Added support for numbered lines.
	26 Dec 2019 : Now K_INTRO is K_CR. Add SysLineWait(), SysLineBack().
	28 Feb 2020 : Minor changes.
	08 Mar 2020 : Support for CRT_LONG in menu, about and (ejem) help options.
	31 Dec 2020 : Use NUM_SEP instead of space to separate line numbers from text.
	04 Jan 2021 : Use configuration variables.
	22 Feb 2021 : Removed CRT_ROWS, CRT_COLS.
*/

/* Read character from keyboard
   ----------------------------
*/
/* **************************** SEE #define
getchr()
{
	return GetKey();
}
******************************* */

/* Print character on screen
   -------------------------
*/
/* **************************** SEE #define
putchr(ch)
int ch;
{
	CrtOut(ch);
}
******************************* */

/* Print character on screen X times
   ---------------------------------
*/
putchrx(ch, n)
int ch, n;
{
	while(n--) {
		putchr(ch);
	}
}

/* Print string on screen
   ----------------------
*/
putstr(s)
char *s;
{
	while(*s)
		putchr(*s++);
}

/* Print string + '\n' on screen
   -----------------------------
*/
putln(s)
char *s;
{
	putstr(s); putchr('\n');
}

/* Print number on screen
   ----------------------
*/
putint(format, value)
char *format; int value;
{
	char r[7]; /* -12345 + ZERO */

	sprintf(r, format, value);

	putstr(r);
}

/* Print program layout
   --------------------
*/
Layout()
{
	int i, k, w;

	/* Clear screen */
	CrtClear();

	/* Header */
	putstr("te:");

	/* Information layout */
	CrtLocate(PS_ROW, PS_INF); putstr(PS_TXT);

	/* Max. # of lines */
	CrtLocate(PS_ROW, PS_LIN_MAX); putint("%04d", cf_mx_lines);

	/* # of columns */
	CrtLocate(PS_ROW, PS_COL_MAX); putint("%02d", 1 + ln_max);

	/* Ruler */
#if CRT_LONG
	CrtLocate(BOX_ROW - 1, cf_num);

	w = cf_cols - cf_num;

	for(i = k = 0; i < w; ++i)
	{
		if(k++)
		{
			putchr(RULER_CHR);

			if(k == cf_tab_cols)
				k = 0;
		}
		else
			putchr(RULER_TAB);
	}

	/* System line separator */
	CrtLocate(cf_rows - 2, 0);

	putchrx(SYS_LINE_SEP, cf_cols);
#endif
}

/* Print filename
   --------------
*/
ShowFilename()
{
	char *s;

	CrtLocate(PS_ROW, PS_FNAME);

	putstr((s = CurrentFile()));

	putchrx(' ', FILENAME_MAX - strlen(s) - 1);
}

/* Print message on system line
   ----------------------------
   Message can be NULL == blank line / clear system line.
*/
SysLine(s)
char *s;
{
	CrtClearLine(cf_rows - 1);

	if(s)
		putstr(s);

	/* Set flag for Loop() */
	sysln = 1;
}

/* Print message on system line and wait
   for CR and / or ESC key press
   -------------------------------------
   Message can be NULL. Returns NZ if CR, else Z.
*/
SysLineWait(s, cr, esc)
char *s, *cr, *esc;
{
	int ch;

	SysLine(s);

	if(s)
		putstr(" (");

	if(cr)
	{
		putstr(GetKeyName(K_CR)); putstr(" = "); putstr(cr); putstr(", ");
	}

	if(esc)
	{
		putstr(GetKeyName(K_ESC)); putstr(" = "); putstr(esc);
	}

	if(s)
		putchr(')');

	putstr(": ");

	for(;;)
	{
		ch = getchr();

		if(cr && ch == K_CR)
			break;

		if(esc && ch == K_ESC)
			break;
	}

	SysLine(NULL);

	return (ch == K_CR);
}


/* Print message on system line and wait
   for ESC key press to CONTINUE
   -------------------------------------
   Message can be NULL.
*/
SysLineCont(s)
char *s;
{
	SysLineWait(s, NULL, "continue");
}

/* Print message on system line and wait
   for ESC key press to COMEBACK
   -------------------------------------
   Message can be NULL.
*/
SysLineBack(s)
char *s;
{
	SysLineWait(s, NULL, "back");
}

/* Print message on system line and wait
   for CONFIRMATION
   -------------------------------------
   Message can be NULL. Returns NZ if YES, else Z.
*/
SysLineConf(s)
char *s;
{
	return SysLineWait(s, "continue", "cancel");
}

/* Ask for a string
   ----------------
   Return NZ if entered, else Z.
*/
SysLineStr(what, buf, maxlen)
char *what, *buf; int maxlen;
{
	int ch;

	SysLine(what);
	putstr(" (");
	putstr(GetKeyName(K_ESC));
	putstr(" = cancel): ");

	ch = ReadLine(buf, maxlen);

	SysLine(NULL);

	if(ch == K_CR && *buf)
			return 1;

	return 0;
}

/* Ask for a filename
   ------------------
   Return NZ if entered, else Z.
*/
SysLineFile(fn)
char *fn;
{
	return SysLineStr("Filename", fn, FILENAME_MAX - 1);
}

/* Ask for confirmation on changes not saved
   -----------------------------------------
   Returns NZ if YES, else Z.
*/
SysLineChanges()
{
	return SysLineConf("Changes will be lost!");
}

/* Read simple line
   ----------------
   Returns last character entered: INTRO or ESC.
*/
ReadLine(buf, width)
char *buf;
int width;
{
	int len;
	int ch;

	putstr(buf); len=strlen(buf);

	while(1)
	{
		switch((ch = getchr()))
		{
			case K_LDEL :
				if(len)
				{
					putchr('\b'); putchr(' '); putchr('\b');

					--len;
				}
				break;
			case K_CR :
			case K_ESC :
				buf[len] = 0;
				return ch;
			default :
				if(len < width && ch >= ' ')
					putchr(buf[len++] = ch);
				break;
		}
	}
}

/* Return name of current file
   ---------------------------
*/
CurrentFile()
{
	return (file_name[0] ? file_name : "-");
}

/* Clear the editor box
   --------------------
*/
ClearBox()
{
	int i;

	for(i = 0; i < box_rows; ++i)
		CrtClearLine(BOX_ROW + i);
}

/* Print centered text on the screen
   ---------------------------------
*/
CenterText(row, txt)
int row; char *txt;
{
	CrtLocate(row, (cf_cols - strlen(txt)) / 2);

	putstr(txt);
}

#if OPT_BLOCK

/* Refresh block selection in editor box
   -------------------------------------
   Set 'sel' to NZ for reverse print, else Z for normal print.
*/
RefreshBlock(row, sel)
int row, sel;
{
	int i, line;

	line = GetFirstLine() + row;

	for(i = row; i < box_rows; ++i) {
		if(line >= blk_start) {
			if(line <= blk_end) {
#if CRT_CAN_REV
				CrtLocate(BOX_ROW + i, cf_num);
				CrtClearEol();

				if(sel) {
					CrtReverse(1);
				}

				putstr(lp_arr[line]);
				putchr(' ');

				if(sel) {

					CrtReverse(0);
				}
#else
				CrtLocate(BOX_ROW + i, cf_cols - 1); putchr(sel ? BLOCK_CHR : ' ');
#endif
			}
			else {
				break;
			}
		}

		++line;
	}
}

#endif

/* Refresh editor box
   ------------------
   Starting from box row 'row', line 'line'.
*/
Refresh(row, line)
int row, line;
{
	int i;
	char *format;

#if OPT_BLOCK

	int blk, sel;

	blk = (blk_count && blk_start <= GetLastLine() && blk_end >= GetFirstLine());
	sel = 0;

#endif

	if(cf_num) {
		format = "%?d";
		format[1] = '0' + cf_num - 1;
	}

	for(i = row; i < box_rows; ++i)
	{
		CrtClearLine(BOX_ROW + i);

		if(line < lp_now) {

			if(cf_num) {
						putint(format, line + 1);
						putchr(NUM_SEP);
			}

#if OPT_BLOCK

			if(blk) {
				if(line >= blk_start) {
					if(line <= blk_end) {
#if CRT_CAN_REV
						CrtReverse((sel = 1));
#else
						sel = 1;
#endif
					}
				}
			}

#endif

			putstr(lp_arr[line++]);

#if OPT_BLOCK

			if(sel) {
#if CRT_CAN_REV
				putchr(' ');

				CrtReverse((sel = 0));
#else
				sel = 0;

				CrtLocate(BOX_ROW + i, cf_cols - 1); putchr(BLOCK_CHR);
#endif
			}

#endif

		}
	}
}

/* Refresh editor box
   ------------------
*/
RefreshAll()
{
	Refresh(0, lp_cur - box_shr);
}

/* Show the menu
   -------------
   Return NZ to quit program.
*/
Menu()
{
	int run, row, stay, menu, ask;

	/* Setup some things */
	run = stay = menu = ask = 1;

	/* Loop */
	while(run)
	{
		/* Show the menu */
		if(menu)
		{
			row = BOX_ROW + 1;

			ClearBox();

			CenterText(row++, "OPTIONS");
			row++;
#if CRT_LONG
			CenterText(row++, "New");
			CenterText(row++, "Open");
			CenterText(row++, "Save");
			CenterText(row++, "save As");
			CenterText(row++, "Help");
			CenterText(row++, "aBout te");
			CenterText(row  , "eXit te");
#else
			CenterText(row++, "New   Open      Save     Save As");
			CenterText(row++, "Help  aBout te  eXit te         ");
#endif
			menu = 0;
		}

		/* Ask for option */
		if(ask)
		{
			SysLine("Option (");
			putstr(GetKeyName(K_ESC));
			putstr(" = back): ");
		}
		else
		{
			ask = 1;
		}

		/* Do it */
		switch(toupper(getchr()))
		{
			case 'N'   : run = MenuNew(); break;
			case 'O'   : run = MenuOpen(); break;
			case 'S'   : run = MenuSave(); break;
			case 'A'   : run = MenuSaveAs(); break;
			case 'B'   : MenuAbout(); menu = 1; break;
			case 'H'   : MenuHelp(); menu = 1; break;
			case 'X'   : run = stay = MenuExit(); break;
			case K_ESC : run = 0; break;
			default    : ask = 0; break;
		}
	}

	/* Clear editor box */
	ClearBox();

	SysLine(NULL);

	/* Return NZ to quit the program */
	return !stay;
}

/* Menu option: New
   ----------------
   Return Z to quit the menu.
*/
MenuNew()
{
	if(lp_chg)
	{
		if(!SysLineChanges())
			return 1;
	}

	NewFile();

	return 0;
}

/* Menu option: Open
   -----------------
   Return Z to quit the menu.
*/
MenuOpen()
{
	char fn[FILENAME_MAX];

	if(lp_chg)
	{
		if(!SysLineChanges())
			return 1;
	}

	fn[0] = 0;

	if(SysLineFile(fn))
	{
		if(ReadFile(fn))
			NewFile();
		else
			strcpy(file_name, fn);

		return 0;
	}

	return 1;
}

/* Menu option: Save
   -----------------
   Return Z to quit the menu.
*/
MenuSave()
{
	if(!file_name[0])
		return MenuSaveAs();

	WriteFile(file_name);

	return 1;
}

/* Menu option: Save as
   --------------------
   Return Z to quit the menu.
*/
MenuSaveAs()
{
	char fn[FILENAME_MAX];

	strcpy(fn, file_name);

	if(SysLineFile(fn))
	{
		if(!WriteFile(fn))
			strcpy(file_name, fn);

		return 0;
	}

	return 1;
}

/* Menu option: Help
   -----------------
*/
MenuHelp()
{
	int i, k;
	char *s;

	ClearBox();

	CrtLocate(BOX_ROW + 1, 0);

	putstr("HELP for te & "); putstr(CRT_NAME); putln(":\n");

#if CRT_LONG

	for(i = 0; help_items[i] != -1; ++i) {

		// 12345678 123 12345678 (21 characters)
		// BlkEnd   ^BE RETURN

		if((k = help_items[i])) {
			if(*(s = GetKeyWhat(k)) == '?') {
				k = 0;
			}
		}

		if(k) {
			putstr(s); putchrx(' ', 9 - strlen(s));

			k -= 1000;

			if(keys[k] < 32) {
				putchr('^'); putchr('@' + keys[k]);
			}
			else {
				putint("%02x", keys[k]);
			}

			putchr(keys_ex[k] ? keys_ex[k] : ' ');

			putchr(' ');

			putstr((s = GetKeyName(k + 1000))); putchrx(' ', 8 - strlen(s));
		}
		else {
			putchrx(' ', 21);
		}

		if((i + 1) % 3) {
			putstr(" | ");
		}
		else {
			putchr('\n');
		}
	}

#else

	putstr("Sorry, no help is available in short format yet.");

#endif

	SysLineBack(NULL);
}

/* Menu option: About
   ------------------
*/
MenuAbout()
{
	int row;

#if CRT_LONG
	row = BOX_ROW + 1;

	ClearBox();

	CenterText(row++, "te - Text Editor");
	row++;
	CenterText(row++, VERSION);
	row++;
	CenterText(row++, "Configured for");
	CenterText(row++, CRT_NAME);
	row++;
	CenterText(row++, COPYRIGHT);
	row++;
	CenterText(row++, "www.floppysoftware.es");
	CenterText(row++, "cpm-connections.blogspot.com");
	CenterText(row  , "floppysoftware@gmail.com");
#else
	row = BOX_ROW;

	ClearBox();

	CenterText(row++, "te - Text Editor");
	CenterText(row++, VERSION);
	CenterText(row++, "Configured for");
	CenterText(row++, CRT_NAME);
	CenterText(row++, COPYRIGHT);
	CenterText(row++, "www.floppysoftware.es");
#endif

	SysLineBack(NULL);
}

/* Menu option: Quit program
   -------------------------
*/
MenuExit()
{
	if(lp_chg)
	{
		return !SysLineChanges();
	}

	/* Quit program */
	return 0;
}

