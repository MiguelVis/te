/*	te_keys.c

	Text editor.

	Key bindings.

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

	22 Jan 2019 : Start.
	23 Jan 2019 : Added GetKeyWhat().
	29 Jan 2019 : Added K_CLRCLP.
	26 Dec 2019 : Now K_INTRO is K_CR. Add GetKeyName().
	04 Apr 2021 : Remove customized key names. Use key bindings from configuration.
	11 May 2021 : Update keys purposes.
*/

/* Return key name
   ---------------
*/
GetKeyName(key)
int key;
{
	switch(key)
	{
		case K_CR:  return cf_cr_name;
		case K_ESC: return cf_esc_name;
	}
	
	return "?";
}

/* Return key purpose
   ------------------
*/
GetKeyWhat(key)
int key;
{
	/* Max. length of 8 chars., see MenuHelp() */
	 
	switch(key)
	{
		case K_UP:      return "Up";
		case K_DOWN:    return "Down";
		case K_LEFT:    return "Left";
		case K_RIGHT:   return "Right";
		case K_BEGIN:   return "Begin";
		case K_END:     return "End";
		case K_TOP:     return "Top";
		case K_BOTTOM:  return "Bottom";
		case K_PGUP:    return "PgUp";
		case K_PGDOWN:  return "PgDown";
		case K_TAB:     return "Indent";
		case K_CR:      return "NewLine";
		case K_ESC:     return "Escape";
		case K_RDEL:    return "DelRight";
		case K_LDEL:    return "DelLeft";
		case K_CUT:     return "Cut";
		case K_COPY:    return "Copy";
		case K_PASTE:   return "Paste";
		case K_DELETE:  return "Delete";
		case K_CLRCLP:  return "ClearClip";
#if OPT_FIND
		case K_FIND:    return "Find";
		case K_NEXT:    return "FindNext";
#endif
#if OPT_GOTO
		case K_GOTO:    return "GoLine";
#endif
#if OPT_LWORD
		case K_LWORD:   return "WordLeft";
#endif
#if OPT_RWORD
		case K_RWORD:   return "WordRight";
#endif
#if OPT_BLOCK
		case K_BLK_START:  return "BlockStart";
		case K_BLK_END:    return "BlockEnd";
		case K_BLK_UNSET:  return "BlockUnset";
#endif
#if OPT_MACRO
		case K_MACRO:   return "Macro";
#endif
	}

	return "?";
}

/* Set key binding, if not previously set by TECF
   ----------------------------------------------
   The parameter 'key1' should be a control character or 0x7F.
   The parameter 'key2' should be a printable character and is case insensitive (set it to 0 for single keys).
*/
SetKey(func, key1, key2)
int func, key1, key2;
{
	func -= 1000;
	
	if(!cf_keys[func]) {
		cf_keys[func] = key1;
		cf_keys_ex[func] = toupper(key2);
	}
}

/* Return key from keyboard, according to key bindings
   ---------------------------------------------------
*/
GetKey()
{
	int c, x, i, k;

	c = CrtIn();

	if(c > 31 && c != 127) {
		return c;
	}

	for(i = 0; i < KEYS_MAX; ++i) {
		if(cf_keys[i]) {
			if(c == cf_keys[i]) {
				if(cf_keys_ex[i]) {
					x = toupper(CrtIn());
					
					/* TODO: try to optimize the following */

					for(k = i; k < KEYS_MAX; ++k) {
						if(c == cf_keys[k]) {
							if(x == cf_keys_ex[k]) {
								return k + 1000;
							}
						}
					}
					break;
				}
				else {
					return i + 1000;
				}
			}
		}
	}

	return '?';
}

