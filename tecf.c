/*	tecf.c

	TE configuration tool.

	Copyright (c) 2021 Miguel Garcia / FloppySoftware

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 3, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	To compile with MESCC under CP/M:

		cc tecf
		ccopt tecf
		zsm tecf
		hextocom tecf

	Usage:

		tecf patch [filename[.COM] [filename[.CF]]]
		tecf dump  [filename[.COM]] [> filename.CF]

	Revisions:

		02 Jan 2021 : Start.
		18 Jan 2021 : v1.00.
		22 Feb 2021 : Added screen.rows and screen.columns.
		22 Feb 2021 : v1.10.
		04 Apr 2021 : Added key bindings.
		06 Apr 2021 : Added screen characters for various purposes.
		08 Apr 2021 : Added configuration name, key names.
		09 Jun 2021 : Remove unused code. Min. screen height of 8 lines.
		30 Jun 2021 : Added "auto" as legal value for screen.rows and screen.columns.
		01 Jul 2021 : v1.20.

	Notes:

		Needs my CF READER library to read configuration files.
*/

/* MESCC libraries
   ---------------
*/
#define CC_STDIO      // Support for stdin, stdout, stderr.
#define CC_REDIR      // Support for command line redirection - needs CC_STDIO.
#define CC_FREAD      // Include fread().
#define CC_FWRITE     // Include fwrite().
#define CC_FGETS      // Include fgets().
#define CC_FSIZE      // Include fsize().

#include <mescc.h>
#include <alloc.h>
#include <conio.h>
#include <ctype.h>
#include <fileio.h>
#include <fprintf.h>
#include <mem.h>
#include <printf.h>
#include <string.h>
#include <redir.h>

/* External libraries
   ------------------
*/
#include "cfreader.h"

/* TE shared libraries
   -------------------
*/
#include "te_keys.h"
#include "te_conf.c"

/* Project defs.
   -------------
*/
#define APP_NAME    "TECF"
#define APP_VERSION "v1.20 / 01 Jul 2021"
#define APP_COPYRGT "(c) 2021 Miguel Garcia / FloppySoftware"
#define APP_INFO    "TE configuration tool."
#define APP_USAGE   "tecf action arguments..."
#define APP_ACTION1 "patch COM file from CF file:"
#define APP_ACTION2 "\tpatch [[filename[.COM]] [filename[.CF]]]"
#define APP_ACTION3 "dump CF values from COM file:"
#define APP_ACTION4 "\tdump [filename[.COM]] [> filename.CF]"
#define APP_ACTION5 "Default value for \"filename\" is \"TE\"."

#define CF_MAX_NAME    31
#define CF_MIN_ROWS    8
#define CF_MAX_ROWS    255
#define CF_MIN_COLS    64
#define CF_MAX_COLS    255
#define CF_MIN_LINES   64
#define CF_MAX_LINES   4096
#define CF_MIN_TABSIZE 1
#define CF_MAX_TABSIZE 16
#define CF_MAX_BULLETS 7
#define CF_MAX_KEYNAME 7

/* Globals
   -------
*/
char com_fname[FILENAME_MAX];   // COM filename
char *tmp_fname = "TE.$$$";     // TEMPORARY filename
char cf_fname[FILENAME_MAX];    // CF filename

unsigned char *com_buf;         // Buffer for COM file data
int com_size;                   // Size of COM file in bytes
int com_cf;                     // Shift of configuration block in com_buf

int conf_code;                  // Code returned by CF READER
int conf_line;                  // Line number returned by CF READER
char *conf_key;                 // Key string returned by CF READER
char *conf_subkey;              // Subkey string from conf_key
char *conf_val;                 // Value string return by CF READER

/* Program entry
   -------------
*/
main(argc, argv)
int argc;
unsigned int argv[]; // char *argv[] - unsupported by MESCC (yet?)
{
	char *def_name;

	// Show usage if there are no arguments
	if(argc == 1) {
		usage();
	}

	// Default name for file names
	def_name = "TE";

	// Execute action
	if(!strcmp(argv[1], "PATCH")) {
		if(argc > 4) {
			err_args();
		}

		do_patch(argc >= 3 ? argv[2] : def_name, argc >= 4 ? argv[3] : def_name);
	}
	else if(!strcmp(argv[1], "DUMP")) {
		if(argc > 3) {
			err_args();
		}

		do_dump(argc >= 3 ? argv[2] : def_name);
	}
	else {
		error("Unknown action");
	}

	// Success
	exit(0);
}

/* Action PATCH
   ------------
*/
do_patch(com_arg, cf_arg)
char *com_arg, *cf_arg;
{
	// Get filenames
	get_com_fname(com_fname, com_arg);
	get_cf_fname(cf_fname, cf_arg);

	// Read COM file and get configuration
	read_com();

	// Read CF file and set configuration
	read_cf();

	// Write COM file with new configuration
	write_com();
}

/* Action DUMP
   -----------
*/
do_dump(com_arg)
char *com_arg;
{
	// Get filename
	get_com_fname(com_fname, com_arg);

	// Read COM file and get configuration
	read_com();

	// Dump configuration
	dump_cf();
}

/* Get filename, adding filetype if missing
   ----------------------------------------
*/
get_fname(dest, fname, ftype)
char *dest, *fname, *ftype;
{
	int len_type;

	len_type = (strchr(fname, '.') ? 0 : strlen(ftype) + 1);

	if(strlen(fname) + len_type >= FILENAME_MAX) {
		err_fname();
	}

	strcpy(dest, fname);

	if(len_type) {
		strcat(dest, "."); strcat(dest, ftype);
	}
}

/* Get COM filename, adding filetype if missing
   --------------------------------------------
*/
get_com_fname(dest, fname)
char *dest, *fname;
{
	get_fname(dest, fname, "COM");
}

/* Get CF filename, adding filetype if missing
   -------------------------------------------
*/
get_cf_fname(dest, fname)
char *dest, *fname;
{
	get_fname(dest, fname, "CF");
}

/* Read COM file into memory, and get the configuration block
   ----------------------------------------------------------
*/
read_com()
{
	FILE *fp;
	int i;
	unsigned char *p;

	// Get the file size in bytes
	if((com_size = fsize(com_fname)) == -1) {
		err_read();
	}

	com_size *= 128;

	// Alloc memory for the whole file
	if(!(com_buf = malloc(com_size))) {
		err_mem();
	}

	// Read the file into memory
	if(!(fp = fopen(com_fname, "rb"))) {
		err_read();
	}

	if(fread(com_buf, com_size, 1, fp) != 1) {
		err_read();
	}

	fclose(fp);

	// Find configuration block
	com_cf = 0;

	for(i = 0; i < com_size; ++i) {
		if(!strcmp(com_buf + i, "TE_CONF")) {
			com_cf = i;
			break;
		}
	}

	if(!com_cf) {
		error("Configuration block not found");
	}

	// Check configuration version
	p = com_buf + com_cf + strlen(com_buf + com_cf) + 1;

	if(cf_version != *p) {
		error("Configuration version mismatch");
	}

	// Get configuration block
	memcpy(&cf_start, com_buf + com_cf, cf_bytes);
}

/* Write COM file from memory with new configuration
   -------------------------------------------------
*/
write_com()
{
	FILE *fp;

	// Set new configuration
	memcpy(com_buf + com_cf, &cf_start, cf_bytes);

	// Write temporary file (new COM file)
	if(!(fp = fopen(tmp_fname, "wb"))) {
		err_write();
	}

	if(fwrite(com_buf, com_size, 1, fp) != 1) {
		err_write();
	}

	if(fclose(fp)) {
		err_write();
	}

	// Delete the old COM file
	if(remove(com_fname)) {
		error("Can't delete file");
	}

	// Rename the temporary file to the old COM filename
	if(rename(tmp_fname, com_fname)) {
		error("Can't rename file");
	}

	// Free buffer memory and invalidate values (not really needed, but...)
	free(com_buf);

	com_buf = NULL;
	com_size = com_cf = 0;
}

/* Read CF file and patch configuration values
   -------------------------------------------
*/
read_cf()
{
	int code, line;
	int n;

	// Read CF file
	code = CfReader(cf_fname, read_var, &line);

	// Check result
	if(code) {
		conf_line = line;

		switch(code) {
			case CFR_CANTOPEN :
				err_conf("can't read");
			case CFR_OVERFLOW :
				err_conf("line too long");
			case CFR_BADNAME :
				err_conf("illegal key name");
			case CFR_NOEQUAL :
				err_conf("equal symbol missing");
			case CFR_BADQUOTES :
				err_conf("missing quote");
			default :
				err_conf("unknown error");
		}
	}

	// Set number of digits + separator for line numbers
	if(cf_num) {
		cf_num = 1;

		for(n = cf_mx_lines; n; n /= 10) {
			++cf_num;
		}
	}
}

/* Read key / value pair from CR file
   ----------------------------------
*/
read_var(line, key, val)
int line;
char *key, *val;
{
	
	/*printf("%s = %s\n", key, val);*/

	conf_line = line;
	conf_key = key;
	conf_val = val;

	if(prefix_match("te")) {
		if(subkey_match("confName")) {
			get_str(cf_name, CF_MAX_NAME);
		}
	}
	else if(prefix_match("screen")) {
		if(subkey_match("rows")) {
			cf_rows = (chk_str_auto() ? 0 : get_uint(CF_MIN_ROWS, CF_MAX_ROWS));
		}
		else if(subkey_match("columns")) {
			cf_cols = (chk_str_auto() ? 0 : get_uint(CF_MIN_COLS, CF_MAX_COLS));
		}
		else if(subkey_match("rulerChar")) {
			cf_rul_chr = get_char();
		}
		else if(subkey_match("rulerTabChar")) {
			cf_rul_tab = get_char();
		}
		else if(subkey_match("vertChar")) {
			cf_vert_chr = get_char();
		}
		else if(subkey_match("horizChar")) {
			cf_horz_chr = get_char();
		}
		else if(subkey_match("lineNumbersChar")) {
			cf_lnum_chr = get_char();
		}
	}
	else if(prefix_match("editor")) {
		if(subkey_match("maxLines")) {
			cf_mx_lines = get_uint(CF_MIN_LINES, CF_MAX_LINES);
		}
		else if(subkey_match("tabSize")) {
			cf_tab_cols = get_uint(CF_MIN_TABSIZE, CF_MAX_TABSIZE);
		}
		else if(subkey_match("lineNumbers")) {
			cf_num = get_bool();
		}
		else if(subkey_match("c_language")) {
			cf_clang = get_bool();
		}
		else if(subkey_match("autoIndent")) {
			cf_indent = get_bool();
		}
		else if(subkey_match("autoList")) {
			cf_list = get_bool();
		}
		else if(subkey_match("listBullets")) {
			get_str(cf_list_chr, CF_MAX_BULLETS);
		}
	}
	else if(prefix_match("keyname")) {
		if(subkey_match("newLine")) {
			get_str(cf_cr_name, CF_MAX_KEYNAME);
		}
		else if(subkey_match("escape")) {
			get_str(cf_esc_name, CF_MAX_KEYNAME);
		}
	}
	else if(prefix_match("key")) {
		if(subkey_match("up")) {
			get_key(K_UP);
		}
		else if(subkey_match("down")) {
			get_key(K_DOWN);
		}
		else if(subkey_match("left")) {
			get_key(K_LEFT);
		}
		else if(subkey_match("right")) {
			get_key(K_RIGHT);
		}
		else if(subkey_match("begin")) {
			get_key(K_BEGIN);
		}
		else if(subkey_match("end")) {
			get_key(K_END);
		}
		else if(subkey_match("top")) {
			get_key(K_TOP);
		}
		else if(subkey_match("bottom")) {
			get_key(K_BOTTOM);
		}
		else if(subkey_match("pgUp")) {
			get_key(K_PGUP);
		}
		else if(subkey_match("pgDown")) {
			get_key(K_PGDOWN);
		}
		else if(subkey_match("indent")) {
			get_key(K_TAB);
		}
		else if(subkey_match("newLine")) {
			get_key(K_CR);
		}
		else if(subkey_match("escape")) {
			get_key(K_ESC);
		}
		else if(subkey_match("delRight")) {
			get_key(K_RDEL);
		}
		else if(subkey_match("delLeft")) {
			get_key(K_LDEL);
		}
		else if(subkey_match("cut")) {
			get_key(K_CUT);
		}
		else if(subkey_match("copy")) {
			get_key(K_COPY);
		}
		else if(subkey_match("paste")) {
			get_key(K_PASTE);
		}
		else if(subkey_match("delete")) {
			get_key(K_DELETE);
		}
		else if(subkey_match("clearClip")) {
			get_key(K_CLRCLP);
		}
		else if(subkey_match("find")) {
			get_key(K_FIND);
		}
		else if(subkey_match("findNext")) {
			get_key(K_NEXT);
		}
		else if(subkey_match("goLine")) {
			get_key(K_GOTO);
		}
		else if(subkey_match("wordLeft")) {
			get_key(K_LWORD);
		}
		else if(subkey_match("wordRight")) {
			get_key(K_RWORD);
		}
		else if(subkey_match("blockStart")) {
			get_key(K_BLK_START);
		}
		else if(subkey_match("blockEnd")) {
			get_key(K_BLK_END);
		}
		else if(subkey_match("blockUnset")) {
			get_key(K_BLK_UNSET);
		}
		else if(subkey_match("macro")) {
			get_key(K_MACRO);
		}
	}

	return 0;
}

/* Compare current key name from read_var()
   ----------------------------------------
   Returns non zero if matchs, else 0
*/
key_match(s)
char *s;
{
	return strcmp(conf_key, s) == 0;
}

/* Compare current key prefix name from read_var()
   and set conf_subkey on success.
   -----------------------------------------------
   Returns non zero if matchs, else 0
*/
prefix_match(s)
char *s;
{
	char *pk;
	
	pk = conf_key;
	
	while(*s) {
		if(*s != *pk) {
			break;
		}
		
		++s;
		++pk;
	}
	
	if(*s == '\0' && *pk == '.') {
		conf_subkey = ++pk;
		
		return 1;
	}
	
	return 0;
}

/* Compare current subkey name from read_var()
   -------------------------------------------
   Returns non zero if matchs, else 0
*/
subkey_match(s)
char *s;
{
	return strcmp(conf_subkey, s) == 0;
}

/* Compare string from current key value
   -------------------------------------
*/
chk_str(s)
char *s;
{
	return strcmp(conf_val, s) == 0;
}

/* Compare string "auto" from current key value
   --------------------------------------------
*/
chk_str_auto()
{
	return chk_str("auto");
}

/* Get unsigned int from current key value: 0..9999
   ------------------------------------------------
*/
get_uint(n_min, n_max)
int n_min, n_max;
{
	int n;
	char *p;

	if(strlen(conf_val) <= 4) {

		p = conf_val;

		while(isdigit(*p)) {
			++p;
		}

		if(!(*p)) {

			n = atoi(conf_val);

			if(n >= n_min && n <= n_max) {
				return n;
			}
		}
	}

	err_conf("not a number or out of range");
}

/* Get string from current key value, length: 1..max_len
   -----------------------------------------------------
*/
get_str(s, max_len)
char *s;
int max_len;
{
	int k;

	k = strlen(conf_val);

	if(k > 0 && k <= max_len) {
		strcpy(s, conf_val);

		return s;
	}

	err_conf("empty or too large string");
}

/* Get character from current key value: character or number 32..255
   -----------------------------------------------------------------
*/
get_char()
{
	char s[2];
	
	if(isdigit(*conf_val)) {
		return get_uint(32, 255);
	}
	
	get_str(s, 1);
	
	return s[0];
}

/* Get key binding from current key value: ^A or ^A^X or ^AX
   ---------------------------------------------------------
*/
get_key(key)
int key;
{
	int ch;
	
	key -= 1000;
	
	if(*conf_val == '^') {

		if((ch = get_key_ctl(conf_val[1]))) {
			cf_keys[key] = ch;
			
			if(conf_val[2] == '\0') {
				cf_keys_ex[key] = '\0';
				return;
			}
			else if(conf_val[2] == '^') {			
				if((ch = get_key_ctl(conf_val[3]))) {
					cf_keys_ex[key] = ch;
				}
				
				if(conf_val[4] == '\0') {
					return;
				}
			}
			else if(!islower(conf_val[2])) {
				cf_keys_ex[key] = conf_val[2];
				
				if(conf_val[3] == '\0') {
					return;
				}
			}
		}
	}
	
	err_conf("bad key binding definition");
}

get_key_ctl(ch)
char ch;
{
	/* ^A..Z ^[ ^\ ^] ^^ ^_ */
	if(ch >= 'A' && ch <= '_') {
		return ch - '@';
	}
	
	/* ^? */
	if(ch == '?') {
		return 0x7F;
	}
	
	return 0;
}

/* Get boolean from current key value: 0..1
   ----------------------------------------
*/
get_bool()
{
	if(!strcmp(conf_val, "true")) {
		return 1;
	}
	else if(!strcmp(conf_val, "false")) {
		return 0;
	}

	err_conf("not a boolean");
}

/* Dump (print) configuration with CF format
   -----------------------------------------
*/
dump_cf()
{
	dump_str("te.confName", cf_name);
	
	dump_uint_auto("screen.rows", cf_rows);
	dump_uint_auto("screen.columns", cf_cols);
	dump_char("screen.rulerChar", cf_rul_chr);
	dump_char("screen.rulerTabChar", cf_rul_tab);
	dump_char("screen.vertChar", cf_vert_chr);
	dump_char("screen.horizChar", cf_horz_chr);
	dump_char("screen.lineNumbersChar", cf_lnum_chr);
	
	dump_uint("editor.maxLines", cf_mx_lines);
	dump_uint("editor.tabSize", cf_tab_cols);
	dump_bool("editor.lineNumbers", cf_num);
	dump_bool("editor.c_language", cf_clang);
	dump_bool("editor.autoIndent", cf_indent);
	dump_bool("editor.autoList", cf_list);
	dump_str("editor.listBullets", cf_list_chr);
	
	dump_str("keyname.newLine", cf_cr_name);
	dump_str("keyname.escape", cf_esc_name);
	
	dump_key("up", K_UP);
	dump_key("down", K_DOWN);
	dump_key("left", K_LEFT);
	dump_key("right", K_RIGHT);
	dump_key("begin", K_BEGIN);
	dump_key("end", K_END);
	dump_key("top", K_TOP);
	dump_key("bottom", K_BOTTOM);
	dump_key("pgUp", K_PGUP);
	dump_key("pgDown", K_PGDOWN);
	dump_key("indent", K_TAB);
	dump_key("newLine", K_CR);
	dump_key("escape", K_ESC);
	dump_key("delRight", K_RDEL);
	dump_key("delLeft", K_LDEL);
	dump_key("cut", K_CUT);
	dump_key("copy", K_COPY);
	dump_key("paste", K_PASTE);
	dump_key("delete", K_DELETE);
	dump_key("clearClip", K_CLRCLP);
	dump_key("find", K_FIND);
	dump_key("findNext", K_NEXT);
	dump_key("goLine", K_GOTO);
	dump_key("wordLeft", K_LWORD);
	dump_key("wordRight", K_RWORD);
	dump_key("blockStart", K_BLK_START);
	dump_key("blockEnd", K_BLK_END);
	dump_key("blockUnset", K_BLK_UNSET);
	dump_key("macro", K_MACRO);
}

/* Dump configuration variable as unsigned int
   -------------------------------------------
*/
dump_uint(key, val)
char *key;
int val;
{
	printf("%s = %d\n", key, val);
}

/* Dump configuration variable as unsigned int or string "auto"
   ------------------------------------------------------------
*/
dump_uint_auto(key, val)
char *key;
int val;
{
	if(val) {
		dump_uint(key, val);
	}
	else {
		dump_str(key, "auto");
	}
}

/* Dump configuration variable as string
   -------------------------------------
*/
dump_str(key, val)
char *key, *val;
{
	printf("%s = \"%s\"\n", key, val);  // FIXME - escape quotes inside the string? Not suported by CF Reader yet!
}

/* Dump configuration variable as character
   ----------------------------------------
*/
dump_char(key, val)
char *key, val;
{
	char s[2];
	
	if(val > 126) {
		dump_uint(key, val);
	}
	else {
		s[0] = val;
		s[1] = '\0';
		
		dump_str(key, s);
	}
}

/* Dump configuration variable as boolean
   --------------------------------------
*/
dump_bool(key, val)
char *key;
int val;
{
	printf("%s = %s\n", key, val ? "true" : "false");
}

/* Dump configuration variable as key binding if set
   -------------------------------------------------
*/
dump_key(name, key)
char *name;
int key;
{
	char def[5];
	
	key -= 1000;
	
	if(cf_keys[key]) {
		def[0] = '^';
		def[1] = (cf_keys[key] != 0x7F ? cf_keys[key] + '@' : '?');
		
		if(cf_keys_ex[key]) {
			if(cf_keys_ex[key] < ' ' || cf_keys_ex[key] == 0x7F) {
				def[2] = '^';
				def[3] = (cf_keys_ex[key] != 0x7F ? cf_keys_ex[key] + '@' : '?');
				def[4] = '\0';
			}
			else {
				def[2] = cf_keys_ex[key];
				def[3] = '\0';
			}
		}
		else {
			def[2] = '\0';
		}
		
		printf("key.%s = \"%s\"\n", name, def);
	}
}

/* Show usage and exit
   -------------------
*/
usage()
{
	fprintf(stderr, "%s %s - %s\n\n", APP_NAME, APP_VERSION, APP_COPYRGT);
	fprintf(stderr, "%s\n\n", APP_INFO);
	fprintf(stderr, "Usage:\n\t%s\n\n", APP_USAGE);
	fprintf(
		stderr,
		"Actions:\n\t%s\n\t%s\n\t%s\n\t%s\n\n\t%s\n",
		APP_ACTION1,
		APP_ACTION2,
		APP_ACTION3,
		APP_ACTION4,
		APP_ACTION5
	);

	exit(0);
}

/* Print configuration error and exit
   ----------------------------------
*/
err_conf(msg)
char *msg;
{
	fprintf(stderr, "%s: Configuration file - %s", APP_NAME, msg);

	if(conf_line) {
		fprintf(stderr, " on line %d", conf_line);
	}

	fprintf(stderr, ".\n");

	exit(-1);
}

/* Print error and exit
   --------------------
*/
error(msg)
char *msg;
{
	fprintf(stderr, "%s: %s.\n", APP_NAME, msg);

	exit(-1);
}

/* Error: wrong number of arguments
   --------------------------------
*/
err_args()
{
	error("Wrong number of arguments");
}

/* Error: filename too long
   ------------------------
*/
err_fname()
{
	error("Filename is too long");
}

/* Error: reading file
   -------------------
*/
err_read()
{
	error("Can't read file");
}

/* Error: writing file
   -------------------
*/
err_write()
{
	error("Can't write file");
}

/* Error: no memory
   ----------------
*/
err_mem()
{
	error("Not enough memory");
}

