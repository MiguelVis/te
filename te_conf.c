/*	te_ui.c

	Text editor.

	Configuration variables.

	Copyright (c) 2015-2020 Miguel Garcia / FloppySoftware

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
*/

/* Configuration block
   -------------------
*/
#asm
	          defb 'TE_CONF', 0       ;  8 bytes > Clue to know where the configuration block starts in memory and COM file.

	          defb 0                  ;  1 byte  > Configuration version. Starts from 0. It's not the TE version number.

cf_name:      defb '0123456789ABCDEF' ; 32 bytes > Configuration name,
              defb '0123456789ABCDE'
              defb 0

cf_rows:      defb 25                 ;  1 byte  > Screen rows.
cf_cols:      defb 80                 ;  1 byte  > Screen columns.

cf_lines:     defw 512                ;  2 bytes > Max. number of lines in editor.

cf_tab_spc:   defb 8                  ;  1 byte  > How many spaces a tabulation fills.

cf_rul_tab:   defb '!'                ;  1 byte  > Ruler: Tab stop character.
cf_rul_chr:   defb '.'                ;  1 byte  > Ruler: Character.

cf_sys_chr:   defb '-'                ;  1 byte  > System line separator.

cf_keys:      defb 29                 ; 29 bytes > 1st character in key binding. Must to be equal to KEYS_MAX.
cf_keys_ex:   defb 29                 ; 29 bytes > 2nd character in key binding. Must to be equal to KEYS_MAX.

#endasm

