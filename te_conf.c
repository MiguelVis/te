/*	te_ui.c

	Text editor.

	Configuration variables.

	Copyright (c) 2020-2021 Miguel Garcia / FloppySoftware

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

	22 Oct 2020 : Start.
	22 Feb 2021 : Added screen rows and columns.
	04 Apr 2021 : Added key bindings.
	06 Apr 2021 : Added screen characters for various purposes.
	11 May 2021 : Default values.

	Notes:

	This file is shared with TECF, the TE configuration tool.
*/

/* Configuration variables
   -----------------------
*/
extern char cf_name[];
extern unsigned char cf_rows;
extern unsigned char cf_cols;
extern int cf_mx_lines;
extern unsigned char cf_tab_cols;
extern unsigned char cf_num;
extern unsigned char cf_clang;
extern unsigned char cf_indent;
extern unsigned char cf_list;
extern char cf_list_chr[];
extern char cf_cr_name[];
extern char cf_esc_name[];
extern unsigned char cf_keys[];
extern unsigned char cf_keys_ex[];

extern unsigned char cf_rul_chr;
extern unsigned char cf_rul_tab;
extern unsigned char cf_vert_chr;
extern unsigned char cf_horz_chr;
extern unsigned char cf_lnum_chr;

extern unsigned int cf_start;
extern unsigned char cf_version;
extern int cf_bytes;

#asm
cf_start:
	          defb 'TE_CONF', 0     ;  8 bytes > Identifier + ZERO for the configuration block in memory and COM file.

cf_version:   defb 2                ;  1 byte  > Configuration version >= 0. It's not the TE version.

cf_name:      defb 0,0,0,0,0,0,0,0  ; 32 bytes > Configuration name + ZERO.
              defb 0,0,0,0,0,0,0,0
              defb 0,0,0,0,0,0,0,0
              defb 0,0,0,0,0,0,0,0

cf_rows:      defb 25               ;  1 byte  > Screen rows or 0 for auto.
cf_cols:      defb 80               ;  1 byte  > Screen columns or 0 for auto.

cf_mx_lines:  defw 512              ;  2 bytes > Max. number of lines in editor.
                                    ;            Each line takes 2 bytes (1 word) of memory.

cf_tab_cols:  defb 4                ;  1 byte  > How many spaces a tabulation inserts.

cf_num:       defb 4                ;  1 byte  > Number of digits for line numbers (see cf_mx_lines) plus 1
                                    ;            for the separator. Set to ZERO for no line numbers.

cf_clang:     defb 1                ;  1 byte  > C language auto completion: [], {}, (), "", '', /**/.

cf_indent:    defb 1                ;  1 byte  > Automatic indent according to the indent of previous line.

cf_list:      defb 1                ;  1 byte  > Automatic list.
cf_list_chr:  defb '-*>',0,0,0,0,0  ;  8 byte  > Legal characters for automatic lists + ZERO.

cf_rul_chr:   defb '.'              ;  1 byte  > Ruler: Character.
cf_rul_tab:   defb '!'              ;  1 byte  > Ruler: Tab stop character.

cf_vert_chr:  defb '|'              ;  1 byte  > Char. for vertical draws -- ie: for line numbers.
cf_horz_chr:  defb '-'              ;  1 byte  > Char. for horizontal draws -- ie: for status line.

cf_lnum_chr:  defb '|'              ;  1 byte  > Char. between line numbers and text.

cf_cr_name:   defb 0,0,0,0,0,0,0,0  ;  8 byte  > CR key name
cf_esc_name:  defb 0,0,0,0,0,0,0,0  ;  8 byte  > ESC key name

cf_keys:      defb 0,0,0,0,0,0,0,0  ; 29 bytes > 1st character in key binding. Must to be equal to KEYS_MAX.
              defb 0,0,0,0,0,0,0,0
			  defb 0,0,0,0,0,0,0,0
			  defb 0,0,0,0,0
			  
cf_keys_ex:   defb 0,0,0,0,0,0,0,0  ; 29 bytes > 2nd character in key binding. Must to be equal to KEYS_MAX.
              defb 0,0,0,0,0,0,0,0
			  defb 0,0,0,0,0,0,0,0
			  defb 0,0,0,0,0

cf_bytes:     defw $ - cf_start + 2 ;  2 bytes > Block configuration size in bytes.

#endasm

