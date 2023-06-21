/*	te_osb1.c

	Text editor -- version for the Osborne 1.

	Adapted by Oleg Farenyuk from a version for the Kaypro II,
		adapted by Stephen S. Mitchell from a version for the Amstrad PCW,
        written by Miguel Garcia / Floppy Software.
		
	Copyright (c) 2015-2021 Miguel Garcia / FloppySoftware
	Copyright (c) 2023 Oleg Farenyuk aka Indrekis

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

	te_osb1 [filename]

	Compilation:

	cc te_osb1
	ccopt te_osb1
	zsm te_osb1
	hextocom te_osb1
	[tecf patch te_osb1 te_osb1]

	Changes:

	Original code: July 4, 2015.
	
	13 Apr 2023 : Oleg Farenyuk : copied and modified te_osbe.c

	Notes:

	-
*/

/* Default configuration values
   ----------------------------
 Correct columns value is 52, but it leads 
 to horizontal scrolling for the last symbol
 (some bug in code?), so as a quick fix   
*/
#define CRT_DEF_ROWS 24
#define CRT_DEF_COLS 51

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
	ld   c,l
	ld   a,l
	cp   10
	jp   nz,BiosConout
	call BiosConout
	ld   c,13
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
	CrtOut(0x1A);
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
	CrtOut(27); CrtOut(84);
}

/* Turn on / off reverse video
   ---------------------------
*/
CrtReverse(on)
int on;
{
	/* Used underline -- Osborne 1 does not have a reverse mode */
	CrtOut(27); CrtOut(on ? 'l' : 'm');
}

