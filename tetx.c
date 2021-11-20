/*	tetx.c

	TE text tool.

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

		http://www.floppysoftware.es/
		https://cpm-connections.blogspot.com
		floppysoftware@gmail.com

	To compile with MESCC under CP/M:

		cc tetx
		ccopt tetx
		zsm tetx
		hextocom tetx

	Usage:

		tetx

	Revisions:

		03 Nov 2021 : Start.
		04 Nov 2021 : v1.00.
		20 Nov 2021 : Refactorized in order to read full lines. Add -S option.
		20 Nov 2021 : v1.01.

	Notes:

		-
*/

/* MESCC libraries
   ---------------
*/
#define CC_STDIO      /* Support for stdin, stdout, stderr */
#define CC_REDIR      /* Support for command line redirection - needs CC_STDIO */
#define CC_FGETS      /* Include fgets() */

#include <mescc.h>
#include <ctype.h>
#include <conio.h>
#include <fileio.h>
#include <printf.h>
#include <fprintf.h>
#include <redir.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "TETX"
#define APP_VERSION "v1.01 / 20 Nov 2021"
#define APP_COPYRGT "(c) 2021 Miguel Garcia / FloppySoftware"
#define APP_INFO    "TE text tool."
#define APP_USAGE   "tetx [-options...] [fname...] [> fname]"
#define APP_OPT_E   "-e     Skip empty lines."
#define APP_OPT_L   "-l     Convert to lowercase."
#define APP_OPT_N   "-n     Print line numbers."
#define APP_OPT_T   "-t[N]  Convert tab to spaces (default = 4)."
#define APP_OPT_U   "-u     Convert to uppercase."
#define APP_OPT_W   "-wN    Max. width of lines (32..255)."
#define APP_OPT_S   "-s     Trim spaces on the right."
#define APP_EX_1    "tetx -ne mydoc.txt"
#define APP_EX_2    "tetx -t4u -n letter1.txt letter2.txt > letters.txt"

#define TAB_SPACES  4   /* How many spaces are a tab by default */
#define TAB_MIN     1   /* Minimum number of spaces for a tab */
#define TAB_MAX     16  /* Maximum number of spaces for a tab */

#define WIDTH_MIN   32  /* Minimum width of lines */
#define WIDTH_MAX   255 /* Maximum width of lines */

#define BUF_SIZE    257 /* WIDTH_MAX + \n + \0 */

/* Options flags
   -------------
*/
#define FLAG_E 1  /* Skip empty lines ? */
#define FLAG_L 2  /* Convert to lowercase */
#define FLAG_N 4  /* Print line numbers */
#define FLAG_T 8  /* Convert tab to spaces */
#define FLAG_U 16 /* Convert to uppercase */
#define FLAG_W 32 /* Max. width of lines */
#define FLAG_S 64 /* Trim spaces on the right */

/* Globals
   -------
*/
int flags;
int line;
int tab_spaces;
int width;
int col;
char buf[BUF_SIZE];

/* Program entry
   -------------
*/
main(argc, argv)
int argc;
unsigned int argv[]; // char *argv[] - unsupported by MESCC (yet?)
{
	int i, n;
	char *p;

	/* Show usage if there are no arguments */
	if(argc == 1) {
		usage();
	}

	/* Defaults */
	flags = 0;
	line = 1;
	tab_spaces = TAB_SPACES;

	/* Check arguments */
	for(i = 1; i < argc; ++i) {
		p = argv[i];

		if(*p == '-') {
			while(*++p) {
				switch(*p) {
					case 'E' : flags |= FLAG_E; break;
					case 'L' : flags |= FLAG_L; break;
					case 'N' : flags |= FLAG_N; break;
					case 'T' : flags |= FLAG_T;
						n = 0;

						if(isdigit(*(p+1))) {
							do {
								n = n * 10 + (*(++p) - '0');
							} while(isdigit(*(p+1)));

							if(n >= TAB_MIN && n <= TAB_MAX) {
								tab_spaces = n;
							}
							else {
								error("Illegal number of spaces");
							}
						}

						break;
					case 'U' : flags |= FLAG_U; break;
					case 'W' : flags |= FLAG_W;
						n = 0;

						if(isdigit(*(p+1))) {
							do {
								n = n * 10 + (*(++p) - '0');
							} while(isdigit(*(p+1)));

							if(n >= WIDTH_MIN && n <= WIDTH_MAX) {
								width = n;
							}
							else {
								error("Illegal width of lines");
							}
						}

						break;
					case 'S' : flags |= FLAG_S; break;
					default  : error("Unknown option");
				}
			}
		}
		else {
			break;
		}
	}

	/* Process files */
	if(i == argc) {
		tx_file("-");
	}
	else {
		for(i = i; i < argc; ++i) {
			if(tx_file(argv[i]))
				return -1;
		}
	}

	/* Return success */
	return 0;
}

/* Process file
   ------------
*/
tx_file(fn)
char *fn;
{
	FILE *fp;
	int len;

	/* Open */
	if(*fn == '-' && !fn[1]) {
		fp = stdin;
	}
	else if((fp = fopen(fn, "r")) == NULL) {
		error_fname(fn);
	}

	/* Process lines */
	for(;;) {
		if(!fgets(buf, BUF_SIZE, fp)) {
			break;
		}

		len = strlen(buf);

		if(buf[len - 1] == '\n') {
			buf[--len] = '\0';
		}
		else if(len > WIDTH_MAX)
		{
			error("Line too long");
		}

		tx_line(len);
	}

	/* Close */
	if(fp != stdin) {
		fclose(fp);
	}

	/* Success */
	return 0;
}

/* Process line
   ------------
*/
tx_line(len)
int len;
{
	int i;

	/* Setup */
	col = 0;

	/* Trim spaces on the right ? */
	if(flags & FLAG_S) {
		while(len) {
			if(buf[len - 1] != ' ') {
				break;
			}

			buf[--len] = '\0';
		}
	}

	/* Lowercase ? */
	if(flags & FLAG_L) {
		for(i = 0; i < len; ++i) {
			buf[i] = tolower(buf[i]);
		}
	}

	/* Uppercase ? */
	if(flags & FLAG_U) {
		for(i = 0; i < len; ++i) {
			buf[i] = toupper(buf[i]);
		}
	}

	/* Process characters */
	for(i = 0; i < len; ++i) {
		tx_out(buf[i]);
	}

	tx_out('\n');
}

/* Process character and output result
   -----------------------------------
*/
tx_out(ch)
int ch;
{
	int k;

	/* New line ? */
	if(ch == '\n') {

		/* Empty line ? */
		if(col == 0) {

			/* Skip empty lines ? */
			if(flags & FLAG_E) {
				return;
			}
		}
	}

	/* Start of line ? */
	if(col == 0) {

		/* Print line numbers */
		if(flags & FLAG_N) {
			printf("%5d ", line++);

			col += 6;
		}
	}

	/* New line ? */
	if(ch == '\n') {
		putchar('\n');

		col = 0;

		return;
	}

	/* Tab ? */
	if(ch == '\t') {
		if(flags & FLAG_T) {
			k = tab_spaces - (col % tab_spaces);

			/* Max. width ? */
			if(flags & FLAG_W) {
				if(col + k >= width) {
					tx_out('\n');
					tx_out('\t');

					return;
				}
			}

			col += k;

			while(k--) {
				putchar(' ');
			}

			return;
		}
	}

	/* Max. width ? */
	if(flags & FLAG_W) {
		if(col + 1 > width) {
			tx_out('\n');
			tx_out(ch);

			return;
		}
	}

	/* Print character */
	putchar(ch);

	++col;
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
		"Options:\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
		APP_OPT_E,
		APP_OPT_L,
		APP_OPT_N,
		APP_OPT_T,
		APP_OPT_U,
		APP_OPT_W,
		APP_OPT_S
	);

	fprintf(stderr, "Examples:\n\t%s\n\t%s\n",
		APP_EX_1,
		APP_EX_2
	);

	exit(0);
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

/* Print error and exit
   --------------------
*/
error_fname(msg, fname)
char *msg, *fname;
{
	fprintf(stderr, "%s: %s \"%s\".\n", APP_NAME, msg, fname);

	exit(-1);
}

