/*	te_tak.c

	Text editor -- version for the Takeda Toshiya's CP/M emulator.

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

	te_tak [filename]

	Compilation:

	cc te_tak
	ccopt te_tak
	zsm te_tak
	hextocom te_tak

	Changes:

	06 May 2015 : 1st version.
	02 Jun 2016 : Minor changes.
	25 Jan 2018 : Find & find next keys.
	26 Jan 2018 : Key to execute macro from file.
	04 Feb 2018 : Key to go to line #.
	30 Dec 2018 : Refactorized i/o functions.
	15 Jan 2019 : Added CrtReverse().
	18 Jan 2019 : Added K_DELETE.
	23 Jan 2019 : Modified a lot for key bindings support.
	29 Jan 2019 : Added K_CLRCLP.
	22 Dec 2019 : Modified K_BEGIN to CTL_V.
	24 Dec 2019 : Added OPT_NUM.
	26 Dec 2019 : Now K_INTRO is K_CR. Remove CRT_ESC_KEY.
	14 Jan 2021 : Remove OPT_NUM.
	22 Feb 2021 : Move CRT_ROWS, CRT_COLS to assembler.
	04 Apr 2021 : Remove key bindings.
	11 May 2021 : Remove CRT configuration values.
	30 Jun 2021 : Added CRT_DEF_ROWS, CRT_DEF_COLS.

	Notes:

	For CPM.EXE / CP/M Player for Win32 console from Takeda Toshiya.

	It emulates a 25x80 VT-100.

	It needs to translate the IBM PC keyboard codes.
*/

/* Default configuration values
   ----------------------------
*/
#define CRT_DEF_ROWS 25
#define CRT_DEF_COLS 80

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

   Translates the IBM PC key codes into single characters.

   int CrtIn(void)
*/
CrtIn()
{
	int ch;

	ch = CrtInEx();

	/* Translate key codes begining with 0 or 224 */

	if(!ch || ch == 224)
	{
		ch = CrtInEx();

		switch(ch)
		{
			case 72  : /* UP */
				return 5;
			case 80  : /* DOWN */
				return 24;
			case 75  : /* LEFT */
				return 19;
			case 77  : /* RIGHT */
				return 4;
			case 71  : /* HOME - INICIO */
				return 22;
			case 79  : /* END */
				return 1;
			case 73  : /* PGUP */
				return 18;
			case 81  : /* PGDN */
				return 3;
			case 83  : /* DELETE - SUPR */
				return 127;
			case 119 : /* CTL HOME */
				return 16;
			case 117 : /* CTL END */
				return 6;
			case 59  : /* F1 */
				return 21;
			case 60  : /* F2 */
				return 15;
			case 61  : /* F3 */
				return 23;
			case 62  : /* F4 */
				return 7;
			default  :
				return 0;
		}
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