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

	05 Apr 2021 : Moved from te.h.
	11 May 2021 : Remove default key names.
	
	Notes:

	This file is shared with TECF, the TE configuration tool.
*/

/* Key function codes
   ------------------
*/
#define K_UP	    1000
#define K_DOWN	    1001
#define K_LEFT	    1002
#define K_RIGHT	    1003
#define K_BEGIN	    1004
#define K_END	    1005
#define K_TOP       1006
#define K_BOTTOM    1007
#define K_PGUP	    1008
#define K_PGDOWN    1009
#define K_TAB       1010
#define K_CR	    1011
#define K_ESC	    1012
#define K_RDEL	    1013
#define K_LDEL      1014
#define K_CUT       1015
#define K_COPY      1016
#define K_PASTE     1017
#define K_DELETE    1018
#define K_CLRCLP    1019
#define K_FIND      1020
#define K_NEXT      1021
#define K_GOTO      1022
#define K_LWORD     1023
#define K_RWORD     1024
#define K_BLK_START 1025
#define K_BLK_END   1026
#define K_BLK_UNSET 1027
#define K_MACRO     1028

#define KEYS_MAX    29   /* Max. # of key bindings */

/* Control characters
   ------------------
*/
#define CTL_A 1
#define CTL_B 2
#define CTL_C 3
#define CTL_D 4
#define CTL_E 5
#define CTL_F 6
#define CTL_G 7
#define CTL_H 8
#define CTL_I 9
#define CTL_J 10
#define CTL_K 11
#define CTL_L 12
#define CTL_M 13
#define CTL_N 14
#define CTL_O 15
#define CTL_P 16
#define CTL_Q 17
#define CTL_R 18
#define CTL_S 19
#define CTL_T 20
#define CTL_U 21
#define CTL_V 22
#define CTL_W 23
#define CTL_X 24
#define CTL_Y 25
#define CTL_Z 26

#define ZERO  0
#define ESC   27
#define DEL   127

