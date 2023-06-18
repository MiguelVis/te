/*	te_ansi.c

	Text editor -- version for the ANSI terminal.

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

	te_ansi [filename]

	Compilation:

	cc te_ansi
	ccopt te_ansi
	zsm te_ansi
	hextocom te_ansi

	Changes:

	20 Oct 2020 : 1st version from source for the Takeda Toshiya's CP/M emulator.
	14 Jan 2021 : Remove OPT_NUM.
	22 Feb 2021 : Move CRT_ROWS, CRT_COLS to assembler.
	04 Apr 2021 : Remove key bindings.
	11 May 2021 : Remove CRT configuration values.
	30 Jun 2021 : Added CRT_DEF_ROWS, CRT_DEF_COLS.
	06 Jul 2021 : Optimize CrtOut().
	17 Jun 2023 : Add some delay in CrtInSt() to make real hardware happy.
	18 Jun 2023 : Add input translations for PgUp and PgDn. Add alternate input translations for Begin and End (VT100).

	Notes:

	It emulates a 25x80 ANSI terminal.

	It needs to translate some keyboard codes.
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

   Translates the ANSI key codes into single characters.

   int CrtIn(void)
*/
CrtIn()
{
	int ch, ex;

	ch = CrtInEx();

	/* Translate key codes begining with 0x1B (ESC):
	
	   UP:    ESC [ A   --> ^E
	   DOWN:  ESC [ B   --> ^X
	   RIGHT: ESC [ C   --> ^D
	   LEFT:  ESC [ D   --> ^S
	   HOME:  ESC [ H   --> ^V
	   END:   ESC [ F   --> ^A
	   PGUP:  ESC [ 5 ~ --> ^R
	   PGDN:  ESC [ 6 ~ --> ^C
	   HOME:  ESC [ 7 ~ --> ^V  (VT100)
	   END:   ESC [ 8 ~ --> ^A  (VT100)
	*/

	if(ch == 0x1B)
	{
		if(CrtInSt())
		{
			ch = 0;

			if(CrtInEx() == '[')
			{
				if(CrtInSt())
				{
					switch((ex = CrtInEx()))
					{
						case 'A' : /* UP */
							return CTL_E;
						case 'B' : /* DOWN */
							return CTL_X;
						case 'C' : /* RIGHT */
							return CTL_D;
						case 'D' : /* LEFT */
							return CTL_S;
						case 'H' : /* HOME */
							return CTL_V;
						case 'F' : /* END */
							return CTL_A;
						case '5' : /* PGUP */
						case '6' : /* PGDN */
						case '7' : /* HOME */
						case '8' : /* END */
							if(CrtInSt())
							{
									if(CrtInEx() == '~') {
										if(ex == 5) {
											return CTL_R;
										}
										else if(ex == 6) {
											return CTL_C;
										}
										else if(ex == 7) {
											return CTL_V;
										}
										else {
											return CTL_A;
										}
									}
							}
							break;
					}
				}
			}
		}
	}

	return ch;
}

#asm
CrtInSt:
	ld hl,8200

CrtInSt0:
	dec hl
	
	ld a,h
	or l
	
	jr nz,CrtInSt0
	
	call BiosConst
	ld h,0
	ld l,a
	ret
	
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