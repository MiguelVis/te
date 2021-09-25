/*	te_error.c

	Text editor.

	Errors.

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
	25 Sep 2021 : Restore system message and cursor position when editing in ErrLine().
*/

/* Print error message and wait for a key press
   --------------------------------------------
*/
ErrLine(s)
char *s;
{
	SysLineCont(s);
	
	/* Restore system message and cursor position when editing */
	if(editln)
	{
		SysLineEdit();
		
		sysln = 0;
		
		CrtLocate(BOX_ROW + box_shr, box_shc + cf_num);
	}
}

/* No memory error
   ---------------
*/
ErrLineMem()
{
	ErrLine("Not enough memory");
}

/* Line too long error
   -------------------
*/
ErrLineLong()
{
	ErrLine("Line too long");
}

/* Can't open file error
   ---------------------
*/
ErrLineOpen()
{
	ErrLine("Can't open");
}

/* Too many lines error
   --------------------
*/
ErrLineTooMany()
{
	ErrLine("Too many lines");
}

