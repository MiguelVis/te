/*	te.h

	Text editor.

	Definitions.

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

	04 May 2015 : 1st version.
	12 May 2015 : v1.01
	14 May 2015 : v1.02
	15 May 2015 : v1.03
	31 Aug 2015 : v1.04
	02 Jun 2016 : v1.05 : CRT_FILE, CRT_ENGLISH, CRT_SPANISH, etc. are now out of this file.
	10 Jun 2016 : v1.06 : Removed BOX_COL. Added PS_FNAME, PS_INF, PS_LIN_CUR, PS_LIN_NOW,
	                      PS_LIN_MAX, PS_COL_CUR, PS_COL_NOW, PS_COL_MAX.
	14 Jun 2016 : v1.07 : Hack for SamaruX.
	05 Jul 2017 : v1.08
	24 Jan 2018 : v1.09
	20 Feb 2018 : v1.10 : Defines for copyright and macros.
	22 Feb 2018 : v1.11
	26 Dec 2018 : v1.12 : Added MAC_SEP.
	23 Jan 2019 : Added key function and character control codes. Added support for key binding.
	27 Jan 2019 : Added support for macros.
	29 Jan 2019 : Added K_CLRCLP. Show clipboard status on information line.
	30 Jan 2019 : Removed support for SamaruX.
	15 Feb 2019 : v1.13
	24 Dec 2019 : Added MAX_DIGITS.
	26 Dec 2019 : Now K_INTRO is K_CR.
	27 Dec 2019 : v1.20
	29 Feb 2020 : Adjust KEYS_MAX.
	01 Mar 2020 : Added OPT_CLANG. Set default options.
	02 Mar 2020 : Added OPT_INDENT.
	04 Mar 2020 : v1.30
	08 Mar 2020 : v1.40 : Added CRT_CAN_REV.
	22 Dec 2020 : Added MAC_FTYPE.
	31 Dec 2020 : Added OPT_LIST, LIST_CHRS, NUM_SEP. Added default values for layout characters.
	05 Jan 2021 : Remove MAX_DIGITS.
	14 Jan 2021 : Remove OPT_NUM, TAB_COLS.
	18 Jan 2021 : v1.50
	22 Feb 2021 : Remove CRT_COLS.
	22 Feb 2021 : v1.60
	04 Apr 2021 : Added K_CR_NAME, K_ESC_NAME.
	05 Apr 2021 : Move key defs. to te_keys.h.
	09 Jun 2021 : Remove RULER_TAB, RULER_CHR, SYS_LINE_SEP, NUM_SEP.
	30 Jun 2021 : Add CRT_DEF_ROWS, CRT_DEF_COLS.
	01 Jul 2021 : v1.70
	05 Jul 2021 : Added OPT_Z80.
	06 Jul 2021 : Added MAC_SYM_SIZ. Remove MAX_LINES.
	25 Sep 2021 : v1.71

	Notes:

	Change TE_VERSION as required, before compilation.
*/

/* Version
   -------
*/
#define VERSION "v1.71 / 25 Sep 2021 for CP/M"

/* Copyright
   ---------
*/
#define COPYRIGHT "(c) 2015-2021 Miguel Garcia / FloppySoftware"

/* Default options
   ---------------
*/
#ifndef CRT_DEF_ROWS
#define CRT_DEF_ROWS 24 /* Default screen rows */
#endif

#ifndef CRT_DEF_COLS
#define CRT_DEF_COLS 80 /* Default screen columns */
#endif

#ifndef OPT_Z80
#define OPT_Z80    1  /* Write some things as Z80 assembler */
#endif

#ifndef OPT_LWORD
#define OPT_LWORD  1  /* Go to word on the left */
#endif

#ifndef OPT_RWORD
#define OPT_RWORD  1  /* Go to word on the right */
#endif

#ifndef OPT_FIND
#define OPT_FIND   1  /* Find string */
#endif

#ifndef OPT_GOTO
#define OPT_GOTO   1  /* Go to line # */
#endif

#ifndef OPT_BLOCK
#define OPT_BLOCK  1  /* Block selection */
#endif

#ifndef OPT_MACRO
#define OPT_MACRO  1  /* Enable macros */
#endif

/* CRT defs.
   ---------
*/
#ifndef CRT_CAN_REV
#define CRT_CAN_REV 1
#endif

#ifndef CRT_LONG
#define CRT_LONG 1
#endif

/* Layout characters
   -----------------
*/
#if CRT_CAN_REV
#else
#ifndef BLOCK_CHR
#define BLOCK_CHR    '*'  /* Character to mark lines as selected, when CRT_CAN_REV == 0 */
#endif
#endif

/* More defs.
   ----------
*/
#define FORCED_MAX 128   /* Keyboard forced entry buffer size (for paste, tabs, etc.) */

#define FIND_MAX   32    /* Find string buffer size */

#define PS_ROW     0     /* Information position */
#define PS_FNAME   4     /* Filename - position in row */
#define PS_TXT     "--- | Lin:0000/0000/0000 Col:00/00 Len:00"  /* Information layout */
#define PS_INF     (cf_cols - 41)  /* Information layout - position in row */
#define PS_CLP     (cf_cols - 41)  /* Clipboard status */
#define PS_LIN_CUR (cf_cols - 31)  /* Current line # - position in row */
#define PS_LIN_NOW (cf_cols - 26)  /* How many lines # - position in row */
#define PS_LIN_MAX (cf_cols - 21)  /* Max. # of lines - position in row */
#define PS_COL_CUR (cf_cols - 12)  /* Current column # - position in row */
#define PS_COL_NOW (cf_cols -  2)  /* Line length - position in row */
#define PS_COL_MAX (cf_cols -  9)  /* Max. line length - position in row */

#if CRT_LONG
#define BOX_ROW    2        /* Editor box position */
#else
#define BOX_ROW    1
#endif

#define getchr     GetKey   /* Get a character from the keyboard */
#define putchr     CrtOut   /* Print a character on screen */

#if OPT_MACRO

/* Macros
   ------
*/
#define MAC_START   '{'  /* Left delimiter for symbol names in macros */
#define MAC_END     '}'  /* Right delimiter for symbol names in macros */
#define MAC_SEP     ':'  /* Separator between symbol names and # of repeats */
#define MAC_ESCAPE  '\\' /* For escaped characters in macros */
#define MAC_SYM_MAX 10   /* Max. length of macro symbol name in characters */
#define MAC_SYM_SIZ 11   /* MAC_SYM_MAX + '\0' */
#define MAC_FTYPE   ".m" /* Default filetype for macro files */

#endif

