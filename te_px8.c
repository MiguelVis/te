/*	te_px8.c

	Text editor -- version for the Epson PX-8 "Geneva".

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

	te_px8 [filename]

	Compilation:

	cc te_px8
	ccopt te_px8
	zsm te_px8
	hextocom te_px8

	Changes:

	16 Feb 2020 : 1st version.
	08 Mar 2020 : Alternative to reverse video. First public release.
	14 Jan 2021 : Remove OPT_NUM.
	22 Feb 2021 : Move CRT_ROWS, CRT_COLS to assembler.
	04 Apr 2021 : Remove key bindings.
	11 May 2021 : Remove CRT configuration values.
	30 Jun 2021 : Added CRT_DEF_ROWS, CRT_DEF_COLS.

	Notes:

	The Epson PX-8 "Geneva" runs CP/M 2.2 with a physical LCD screen of 8x80.

	References:

	https://fjkraan.home.xs4all.nl/comp/px8
*/

/* Default configuration values
   ----------------------------
*/
#define CRT_DEF_ROWS 8
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

/* Definitions
   -----------
*/
#define CRT_CAN_REV 0    /* Reverse video is not available */
#define CRT_LONG    0    /* CRT has few lines */

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
	// Display mode 0: 40 lines screen 0, 8 lines screen 2
	CrtOut(27); CrtOut(208); CrtOut(2); CrtOut(24); CrtOut(24);
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
	CrtOut(12);
}

/* Locate the cursor (HOME is 0,0)
   -------------------------------
   void CrtLocate(int row, int col)
*/
CrtLocate(row, col)
int row, col;
{
	CrtOut(27); CrtOut('='); CrtOut(row + 32); CrtOut(col + 32);
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
	CrtOut(27); CrtOut('T');
}

/* Turn on / off reverse video
   ---------------------------
*/
/*
CrtReverse(on)
int on;
{
	CrtOut(27); CrtOut('['); CrtOut(on ? '7' : '0'); CrtOut('m');
}
*/

