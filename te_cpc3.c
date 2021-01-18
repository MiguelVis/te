/*	te_cpc3.c

	Text editor -- version for the Amstrad CPC with CP/M 3.

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

	te_cpc3 [filename]

	Compilation:

	cc te_cpc3
	ccopt te_cpc3
	zsm te_cpc3
	hextocom te_cpc3

	Changes:

	05 May 2015 : 1st version.
	20 Jun 2015 : Modified ESC key name.
	02 Jun 2016 : Minor changes.
	25 Jan 2018 : Find & find next keys.
	20 Feb 2018 : Macro & go to line # keys.
	30 Dec 2018 : Refactorized i/o functions.
	17 Jan 2019 : Added CrtReverse().
	18 Jan 2019 : Added K_DELETE.
	23 Jan 2019 : Modified a lot for key bindings support.
	29 Jan 2019 : Added K_CLRCLP.
	24 Dec 2019 : Added OPT_NUM.
	26 Dec 2019 : Now K_INTRO is K_CR. Remove CRT_ESC_KEY.
	14 Jan 2021 : Remove OPT_NUM.

	Notes:

	The Amstrad CPC with CP/M Plus (v3.1) has a VT52-like emulation,
	and a 25x80 CRT minus 1 line for system messages (it can be enabled
	or disabled).
	
	In order to configure the keyboard for CP/M Plus, the following
	command must be executed before TE:
	
	SETKEYS KEYS.CCP
*/

/* Options
   -------
   Set to 1 to add the following functionalities, else 0.
*/
#define OPT_LWORD 0  /* Go to word on the left */
#define OPT_RWORD 0  /* Go to word on the right */
#define OPT_FIND  1  /* Find string */
#define OPT_GOTO  1  /* Go to line # */
#define OPT_BLOCK 1  /* Block selection */
#define OPT_MACRO 1  /* Enable macros */

/* Definitions
   -----------
*/
#define CRT_NAME "Amstrad CPC & CP/M 3"

#define CRT_ROWS 24        /* CRT rows: 25 - 1 (system line) */
#define CRT_COLS 80        /* CRT columns */

#define RULER_TAB     0x91 /* Ruler: Tab stop character - ie: ! */
#define RULER_CHR     0x90 /* Ruler: Character - ie: . */
#define SYS_LINE_SEP  0x9A /* System line separator character - ie: - */

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
	
	CrtOut(27); CrtOut('x'); /* 24x80 mode on */

	SetKey(K_UP,        31,    '\0', NULL);
	SetKey(K_DOWN,      30,    '\0', NULL);
	SetKey(K_LEFT,      CTL_A, '\0', NULL);
	SetKey(K_RIGHT,     CTL_F, '\0', NULL);
	SetKey(K_BEGIN,     CTL_V, '\0', NULL);
	SetKey(K_END,       CTL_E, '\0', NULL);
	SetKey(K_TOP,       CTL_P, '\0', NULL);
	SetKey(K_BOTTOM,    CTL_S, '\0', NULL);
	SetKey(K_PGUP,      CTL_Q, '\0', NULL);
	SetKey(K_PGDOWN,    CTL_Z, '\0', NULL);
	SetKey(K_TAB,       CTL_I, '\0', "TAB");
	SetKey(K_CR,        CTL_M, '\0', "RETURN");
	SetKey(K_ESC,       ESC,   '\0', "ESC");
	SetKey(K_RDEL,      CTL_G, '\0', "CLR");
	SetKey(K_LDEL,      DEL,   '\0', "DEL");
	SetKey(K_CUT,       CTL_U, '\0', NULL);
	SetKey(K_COPY,      CTL_R, '\0', NULL);
	SetKey(K_PASTE,     CTL_W, '\0', "COPY");
	SetKey(K_DELETE,    CTL_D, '\0', NULL);
	SetKey(K_CLRCLP,    CTL_T, '\0', NULL);
#if OPT_FIND	
	SetKey(K_FIND,      CTL_K, '\0', NULL);
	SetKey(K_NEXT,      CTL_L, '\0', NULL);
#endif
#if OPT_GOTO
	SetKey(K_GOTO,      CTL_J, '\0', NULL);
#endif
#if OPT_LWORD		
	/*SetKey(K_LWORD,     '\0', '\0', NULL);*/
#endif
#if OPT_RWORD	
	/*SetKey(K_RWORD,     '\0', '\0', NULL);*/
#endif
#if OPT_BLOCK	
	SetKey(K_BLK_START, CTL_B, 'S', NULL);
	SetKey(K_BLK_END,   CTL_B, 'E', NULL);
	SetKey(K_BLK_UNSET, CTL_B, 'U', NULL);
#endif
#if OPT_MACRO	
	SetKey(K_MACRO,     CTL_Y, '\0', NULL);
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
	CrtOut(27); CrtOut('y'); /* 24x80 mode off */
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

   int CrtIn(void)
*/
#asm
CrtIn:
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
	CrtOut(27); CrtOut('H'); /* Cursor to 0,0 */
	CrtOut(27); CrtOut('E'); /* Clear CRT */
}

/* Locate the cursor (HOME is 0,0)
   -------------------------------
   void CrtLocate(int row, int col)
*/
CrtLocate(row, col)
int row, col;
{
	CrtOut(27); CrtOut('Y'); CrtOut(row + 32); CrtOut(col + 32);
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
	CrtOut(27); CrtOut('K');
}

/* Turn on / off reverse video
   ---------------------------
*/
CrtReverse(on)
int on;
{
	CrtOut(27); CrtOut(on ? 'p' : 'q');
}

