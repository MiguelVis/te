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
#include "te_conf.c"

/* Project defs.
   -------------
*/
#define APP_NAME    "TECF"
#define APP_VERSION "v1.00 / 18 Jan 2021"
#define APP_COPYRGT "(c) 2021 Miguel Garcia / FloppySoftware"
#define APP_INFO    "TE configuration tool."
#define APP_USAGE   "tecf action arguments..."
#define APP_ACTION1 "patch COM file from CF file:"
#define APP_ACTION2 "\tpatch [[filename[.COM]] [filename[.CF]]]"
#define APP_ACTION3 "dump CF values from COM file:"
#define APP_ACTION4 "\tdump [filename[.COM]] [> filename.CF]"
#define APP_ACTION5 "Default value for \"filename\" is \"TE\"."

#define CF_MIN_LINES   64
#define CF_MAX_LINES   4096
#define CF_MIN_TABSIZE 1
#define CF_MAX_TABSIZE 16
#define CF_MAX_BULLETS 7

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
	conf_line = line;
	conf_key = key;
	conf_val = val;

	if(key_match("editor.maxLines")) {
		cf_mx_lines = get_uint(CF_MIN_LINES, CF_MAX_LINES);
	}
	else if(key_match("editor.tabSize")) {
		cf_tab_cols = get_uint(CF_MIN_TABSIZE, CF_MAX_TABSIZE);
	}
	else if(key_match("editor.lineNumbers")) {
		cf_num = get_bool();
	}
	else if(key_match("editor.c_language")) {
		cf_clang = get_bool();
	}
	else if(key_match("editor.autoIndent")) {
		cf_indent = get_bool();
	}
	else if(key_match("editor.autoList")) {
		cf_list = get_bool();
	}
	else if(key_match("editor.listBullets")) {
		strcpy(cf_list_chr, get_str(CF_MAX_BULLETS));
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

/* Get unsigned int from current key value: 0..9999
   ------------------------------------------------
*/
get_uint(n_max, n_min)
int n_max, n_min;
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

			if(n >= n_max && n <= n_min) {
				return n;
			}
		}
	}

	err_conf("not a number or out of range");
}

/* Get string from current key value, length: 1..max_len
   -----------------------------------------------------
*/
get_str(max_len)
int max_len;
{
	char *p;
	int k;

	k = strlen(conf_val);

	if(k > 0 && k <= max_len) {
		if((p = malloc(k + 1))) {
			strcpy(p, conf_val);

			return p;
		}

		err_mem();
	}

	err_conf("empty string or too large");
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
	dump_uint("editor.maxLines", cf_mx_lines);
	dump_uint("editor.tabSize", cf_tab_cols);
	dump_bool("editor.lineNumbers", cf_num);
	dump_bool("editor.c_language", cf_clang);
	dump_bool("editor.autoIndent", cf_indent);
	dump_bool("editor.autoList", cf_list);
	dump_str("editor.listBullets", cf_list_chr);
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

/* Dump configuration variable as string
   -------------------------------------
*/
dump_str(key, val)
char *key, *val;
{
	printf("%s = \"%s\"\n", key, val);  // FIXME - escape quotes inside the string? Not suported by CF Reader yet!
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