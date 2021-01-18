/*	te_ws100.c

	Text editor -- version for VT100 & WordStar keys, under CP/M.

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

	Usage:

	te_ws100 [filename]

	Compilation:

	cc te_ws100
	ccopt te_ws100
	zsm te_ws100
	hextocom te_ws100

	Changes:

	12 May 2015 : 1st version.
	14 May 2015 : Completed adaptation for WS keys.
	02 Jun 2016 : Minor changes.
	11 Jun 2016 : Minor changes in help text.
	24 Jan 2018 : Find & find next keys.
	26 Jan 2018 : Key to execute macro from file.
	04 Feb 2018 : Key to go to line #.
	30 Dec 2018 : Refactorized i/o functions.
	15 Jan 2019 : Added CrtReverse().
	18 Jan 2019 : Added K_DELETE.
	23 Jan 2019 : Modified a lot for key bindings support.
	29 Jan 2019 : Added K_CLRCLP.
	24 Dec 2019 : Added OPT_NUM.
	26 Dec 2019 : Now K_INTRO is K_CR. Remove CRT_ESC_KEY.
	14 Jan 2021 : Remove OPT_NUM.

	Notes:

	-
*/

/* Options
   -------
   Set to 1 to add the following functionalities, else 0.
*/
#define OPT_LWORD 1  /* Go to word on the left */
#define OPT_RWORD 1  /* Go to word on the right */
#define OPT_FIND  1  /* Find string */
#define OPT_GOTO  1  /* Go to line # */
#define OPT_BLOCK 1  /* Block selection */
#define OPT_MACRO 1  /* Enable macros */

/* Definitions
   -----------
*/
#define CRT_NAME "VT100 & WordStar keys"

#define CRT_ROWS 25       /* CRT rows */
#define CRT_COLS 80       /* CRT columns */

#define RULER_TAB    '!'  /* Ruler: Tab stop character - ie: ! */
#define RULER_CHR    '.'  /* Ruler: Character - ie: . */
#define SYS_LINE_SEP '-'  /* System line separator character - ie: - */

/* Include main code
   -----------------
*/
#include "te.c"

/* Setup CRT: Used when the editor starts
   --------------------------------------
   void CrtSetup(void)
*/
CrtSetup()
{
	CrtSetupEx();

	SetKey(K_UP,        CTL_E, '\0', NULL);
	SetKey(K_DOWN,      CTL_X, '\0', NULL);
	SetKey(K_LEFT,      CTL_S, '\0', NULL);
	SetKey(K_RIGHT,     CTL_D, '\0', NULL);
	SetKey(K_BEGIN,     CTL_Q, 'S',  NULL);
	SetKey(K_END,       CTL_Q, 'D',  NULL);
	SetKey(K_TOP,       CTL_Q, 'R',  NULL);
	SetKey(K_BOTTOM,    CTL_Q, 'C',  NULL);
	SetKey(K_PGUP,      CTL_C, '\0', NULL);
	SetKey(K_PGDOWN,    CTL_R, '\0', NULL);	
	SetKey(K_TAB,       CTL_I, '\0', "TAB");
	SetKey(K_CR,        CTL_M, '\0', "or ^N");
	SetKey(K_ESC,       ESC,   '\0', "ESC");
	SetKey(K_RDEL,      CTL_G, '\0', NULL);
	SetKey(K_LDEL,      CTL_H, '\0', "or DEL");
	SetKey(K_CUT,       CTL_Y, '\0', NULL);
	SetKey(K_COPY,      CTL_O, '\0', NULL);
	SetKey(K_PASTE,     CTL_W, '\0', NULL);
	SetKey(K_DELETE,    CTL_K, 'Y',  NULL);
	SetKey(K_CLRCLP,    CTL_T, '\0', NULL);
#if OPT_FIND
	SetKey(K_FIND,      CTL_Q, 'F',  NULL);
	SetKey(K_NEXT,      CTL_L, '\0', NULL);
#endif
#if OPT_GOTO
	SetKey(K_GOTO,      CTL_J, '\0', NULL);
#endif
#if OPT_LWORD	
	SetKey(K_LWORD,     CTL_A, '\0', NULL);
#endif
#if OPT_RWORD
	SetKey(K_RWORD,     CTL_F, '\0', NULL);
#endif
#if OPT_BLOCK
	SetKey(K_BLK_START, CTL_K, 'B', NULL);
	SetKey(K_BLK_END,   CTL_K, 'K', NULL);
	SetKey(K_BLK_UNSET, CTL_K, 'U', NULL);
#endif
#if OPT_MACRO
	SetKey(K_MACRO,     CTL_Q, 'M', NULL);
#endif
}

#asm
CrtSetupEx:
	ld  hl,(1)
	inc hl
	inc hl
	inc hl
	ld  de,BiosConst
	ld  bc,9
	ldir
	ret

BiosConst:  jp 0
BiosConin:  jp 0
BiosConout: jp 0
#endasm

/* Reset CRT: Used when the editor exits
   -------------------------------------
   void CrtReset(void)
*/
CrtReset()
{
}

/* Output character to the CRT
   ---------------------------
   All program output is done with this function.
   
   On '\n' outputs '\n' + '\r'.

   void CrtOut(int ch)
*/
#asm
CrtOut:
	ld   a,l
	cp   10
	jr   nz,CrtOutRaw
	ld   c,13
	call BiosConout
	ld   l,10
CrtOutRaw:
	ld   c,l
	jp   BiosConout
#endasm

/* Input character from the keyboard
   ---------------------------------
   All program input is done with this function.

   Translate WordStar key sequences to TE key codes.

   int CrtIn(void)
*/
CrtIn()
{
	int ch;

	switch(ch = CrtInEx()) {
		case DEL :   /* DEL == CTL_H */
			return CTL_H;
		case CTL_N : /* ^N == ^M */
			return CTL_M;
	}
	
	return ch;
}

#asm
CrtInEx:
	call BiosConin
	ld h,0
	ld l,a
	ret
#endasm

/* Clear screen and send cursor to 0,0
   -----------------------------------
   void CrtClear(void)
*/
CrtClear()
{
	CrtOut(27); putstr("[1;1H"); /* Cursor to 0,0 */
	CrtOut(27); putstr("[2J");   /* Clear CRT */
}

/* Locate the cursor (HOME is 0,0)
   -------------------------------
   void CrtLocate(int row, int col)
*/
CrtLocate(row, col)
int row, col;
{
	CrtOut(27); CrtOut('[');
	putint("%d", row + 1); CrtOut(';');
	putint("%d", col + 1); CrtOut('H');
}

/* Erase line and cursor to row,0
   ------------------------------
   void CrtClearLine(int row)
*/
CrtClearLine(row)
int row;
{
	CrtLocate(row, 0); CrtClearEol();
}

/* Erase from the cursor to the end of the line
   --------------------------------------------
*/
CrtClearEol()
{
	CrtOut(27); putstr("[K");
}

/* Turn on / off reverse video
   ---------------------------
*/
CrtReverse(on)
int on;
{
	CrtOut(27); CrtOut('['); CrtOut(on ? '7' : '0'); CrtOut('m');
}

