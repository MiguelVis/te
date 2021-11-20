te
==

v1.73 / 20 Nov 2021 for CP/M

(c) 2015-2021 Miguel Garcia / FloppySoftware

[http://www.floppysoftware.es/](http://www.floppysoftware.es)

Introduction
------------

The `te` text editor is small, humble and useful.

It edits the full text in memory, so your system must leave enough
free TPA for the text, after `te` is loaded and running.

It's screen oriented, and can be easily adapted to a lot of computers.

It's written in the C language, and there are ports for various
operating systems: CP/M, DOS, Windows and GNU/Linux.

I use [MESCC](https://github.com/MiguelVis/mescc) to compile it for CP/M
and the Z80 cpu.

![te image](http://www.floppysoftware.es/images/cpm-te.jpg "te image")

Command line
------------

It's simple (for the PCW adaptation; see below for the name of the rest
of available adaptations):

```
te_pcw [filename]
```

Examples:

```
te_pcw           - To start an empty document.
te_pcw todo.txt - To edit the document 'todo.txt'.
```

You should rename the adaptation of your choice to simply `te`, because
some CCP implementations (including the standard from CP/M), do not like
the character `_` in filenames.


The screen layout
-----------------

Depending of the `te` adaptation, you could see a layout similar to:

```
te: TODO.TXT                   --- | Lin:0012/0029/0512 Col:08/76 Len:07
    !.......!.......!.......!.......!.......!.......!.......!.......!...
  1|This is my TO-DO list for today:
  2|  - Wake up.
  3|  - Make coffee.
  4|  - Have a shower.
  5|  - Get dressed.
  6|  - Go work and:
  7|    1. Revise email.
  8|    2. Call Peter.
  9|    3. Meet the boss.
 10|    4. Plan tomorrow.
 11|  - Have lunch with Sarah.
 12|  - ...
 13|
 14|
 15|
 16|
 17|
 18|
 19|
 20|
 21|
------------------------------------------------------------------------
ESC = menu
```

On the first screen line, you will see something like:

```
te: TODO.TXT - The file name of the document your are editing.
```

And something like:

```
--- | Lin:0015/0042/0512 Col:32/90 Len:36
```

The very first field, can have one of the following values:

```
--- Means the clipboard is empty.
CLP Means the clipboard has data.
```

The numbers following 'Lin' are:

```
0015 - The line number you are editing right now.
0042 - The current total number of lines.
0512 - The maximum number of lines.
```

The numbers following 'Col' are:

```
32 - The column number on which you are.
90 - The number of screen columns.
```

And, finally, the number following 'Len' is:

```
36 - The length of the current line.
```

On the last screen line, you will see program messages and questions.


The options menu
----------------

You can enter to the menu, by pressing the `ESCAPE` key, as shown
in the messages line:

```
ESC = menu
```

Note that the exact name of your `ESCAPE` key may differ, depending of
the program configuration.

The options are:

```
New       - To start a new document
Open      - To open a document
Save      - To save the current document
save As   - To save the current document under another filename.
Insert    - To insert a text file into the current position.
Help      - To show the help screen.
aBout te  - To show some te information.
eXit te   - To quit the program.
```

You can select an option by pressing the character in uppercase.

To leave the menu, you must press `ESCAPE`.

If you select one of the options `New`, `Open` or `eXit`, and there are
any changes that are not saved yet, te will ask you for confirmation
before proceed.

If you start a new document, the `Save` option will redirect to you to
the `save As` option.

The help screen will show the keyboard configuration (or key bindings
if you prefer), which depends of the program configuration.

For example, in the case of TE_WS100 (25x80 VT100 and WordStar keys) you
could see something like this:

```
Up         ^E   | Down       ^X   | Indent     ^I
Left       ^S   | Right      ^D   | NewLine    ^M
Begin      ^QS  | End        ^QD  | Escape     ^[
Top        ^QR  | Bottom     ^QC  | Macro      ^QM
PgUp       ^C   | PgDown     ^R   |
WordLeft   ^A   | WordRight  ^F   |
DelLeft    ^H   | DelRight   ^G   |
BlockStart ^KB  | BlockEnd   ^KK  | BlockUnset ^KU
Cut        ^Y   | Copy       ^O   | Paste      ^W
Delete     ^KY  | ClearClip  ^T   |
Find       ^QF  | FindNext   ^L   | GoLine     ^J
```


The clipboard
-------------

The clipboard contents is not cleared after file operations (new, open,
save...). That makes possible to share text between files.

But it's a good practice to clear the clipboard when you don't need it
anymore, to free some memory.


Key bindings
------------

As you can see in the help screen of example, there are some key
bindings to perform actions when you are editing the file.

The exact keys you must press to do an action, depend on the configuration
you are running, but the meanings are the same:
- Up, Down, Left, Right: to move the cursor one character at a time.
- Begin, End: to move the cursor to the begin or the end of the
  current line.
- Top, Bottom: to move the cursor to the begin or the end of the
  current file.
- PgUp, PgDown: to move the cursor to the previous or next page.
- WordLeft, WordRight: to move the cursor to the previous or next word.
- DelLeft, DelRight: to delete the previous or next character.
- Find, FindNext: to find a string or repeat the search.
- GoLine: to go to a line number.
- BlockStart: to mark the start of a block of lines.
- BlockEnd: to mark the end of a block of lines.
- BlockUnset: to unselect a block of lines.
- Cut, Copy, Paste, Delete: to cut, copy, paste or delete a line
  or block of lines.
- ClearClip: to clear the clipboard data.
- Macro: to run a macro.
- Indent: to insert spaces as indentation. Same as Tab.
- NewLine: to insert a break or empty line. Same as Cr, Enter, Intro
  or Return.
- Escape: to exit the menu or cancel an action. Same as Esc.

Take into account that some actions could not be available in all te
adaptations to save memory space. These actions are: WordLeft, WordRight,
Find, FindNext, GoLine, BlockStart, BlockEnd, BlockUnset and Macro.

If you miss one or more of the mentioned actions, you can edit the exact
file of the te adaptation and recompile it to include them.


Auto-completion
---------------

If the text editor is configured with the C language type auto-completion
option enabled, the input of some characters will cause the automatic input
of some other:

```
[  -->  ]
{  -->  }
(  -->  )
"  -->  "
'  -->  '
/* -->  */
```


Auto-indentation
----------------

The text editor supports automatic indentation of lines, according
to the indentation of the previous one.

The indentation is done by inserting the same number of spaces of
the previous line - ie:

```
  This paragraph is
  indented, according to
  the first line.
    More indentation
    here.
```


Lists
-----

The text editor supports automatic lists, when the first non blank
character of the previous line is one of the "bullets" according
to the configuration.

As automatic indentation is supported, automatic multi level lists
are possible - ie:

```
- Item #1
- Item #2
  * Item #2a
  * Item #2b
    > Item #2b-1
	> Item #2b-2
- Item #3
```


Macros
------

Macros are a powerful way to perform repetitive tasks at editing level.

For example, you could create a macro with your signature to include it
in every letter you write.

Or, being a programmer, you could create a gdoc block macro to document
your own C functions.

Macros are not affected by default by auto-completion, auto-indentation
or lists.

Each time you want to run a macro, just press the corresponding key
for the Macro action, enter the filename you choose when you saved it
to disk, and press the key for the NewLine action. That's all.

The default filetype for macro files is ".M".

The filetype is only required when it's different. So, if you have
saved a macro file as `JOIN.M`, you could just enter `JOIN`:

```
JOIN.M     you could enter JOIN.M or just JOIN
JOIN.MAC   you must enter JOIN.MAC
```

There are some special symbols you can use in your macros:

```
{# comment}                    comments, they are ignored
{Up} {Down} {Left} {Right}     move the cursor one character at a time
{Begin} {End}                  go to the begin / end of the current line
{Top} {Bottom}                 go to the begin / end of the current file
{NewLine}                      insert a break or empty line
{Indent}                       insert spaces as indentation
{DelLeft} {DelRight}           delete the character at the left / right
{Cut} {Copy} {Delete}          operation over a line or block of lines
{Paste}                        paste the clipboard contents
{ClearClip}                    clear the clipboard data
{BlockStart} {BlockEnd}        mark the start / end of a block of lines
{FileName}                     insert the current filename
{AutoIndent}                   enable / disable automatic indentation
{AutoList}                     enable / disable automatic list
```

The symbol names are case insensitive - ie: `{ClearClip}` and `{clearclip}`
are the same.

Additionally, you can repeat a symbol as in:

```
{up:12}
```

Auto indentation or auto list can be enabled by just using the symbol
name or setting a repeat number of `1`:

```
{AutoList}
{AutoList:1}
```

They can be disabled setting a repeat number of `0`:

```
{AutoIndent:0}
```

Take into account that line endings in the macro are not translated
to NewLine actions. Use `{NewLine}` for that.

If you need to insert the `{` or `}` characters in the text of the
macro, just write them as `\{` and `\}`.

For example, a macro to insert a banner at the top of the document,
could be written as:

```
{# this is a macro example}
{top}{newline}{up}
=============={newline}
FloppySoftware{newline}
=============={newline}
```

Some macro files are included as examples:

```
HEADER.M     Above example.
JOINUP.M     Join current line with the previous one.
JOINDOWN.M   Join current line with the next one.
```

There are some macros for the C language:

```
CFILE.M      File documentation block for GDOC.
CFUN.M       Function documentation block for GDOC.
CDO.M        Snippet for "do / while".
CFOR.M       Snippet for "for".
CIF.M        Snippet for "if".
CIFELSE.M    Snippet for "if / else".
CSWITCH.M    Snippet for "switch".
CWHILE.M     Snippet for "while".
```

File backup
-----------

Each time a file is saved, te does a backup of the previous file
contents if it exists.

It is renamed as `te.bkp`.


Adaptations
-----------

The program adaptation is done by editing a small source code file.

In this file, you must specify some configuration values according
to your preferences and / or system, and complete a few C
language functions to clear the screen, etc.

As there are some adaptations already done, it's possible that your
machine is already included between them.

If not, don't worry, you can start from a similar adaptation.

See the source code files for more information.

Current CP/M adaptations are:
- `te_pcw`  : Amstrad PCW and CP/M Plus (31x90 VT52 like terminal).
- `te_cpc3` : Amstrad CPC and CP/M Plus (24x80 VT52 like terminal).
- `te_spec` : Spectrum +3 and CP/M Plus (23x51 VT52 like terminal).
- `te_mur`  : K. Murakami's CP/M emulator (25x80 VT100/Ansi).
- `te_tak`  : Takeda Toshiya's CP/M emulator (25x80 VT100/Ansi).
- `te_ws100`: Generic 25x80 VT100 and WordStar keys.
- `te_kp`   : Kaypro II, 4 and 10 (24x80 ADM-3A like terminal),
              contributed by Stephen S. Mitchell (thanks!).
- `te_px8`  : Epson PX-8 "Geneva".
- `te_ansi` : Generic 25x80 ANSI terminal, including (translated) keyboard
              escape sequences.
- `te_apple`: Apple II with SoftCard, a	24x80 SOROC IQ 120 / 140 terminal
              and WordStar keys.
- `te_c128` : Commodore 128 and CP/M Plus (25x80 ADM-31/3A like terminal).
- `te_sam`  : SAM Coupe and Pro-Dos (24x80 VT52 like terminal).

Adaptations for other OS are (they are not distributed yet):
- Windows 32 bit (25x80), compiled with Pelles C
  and its 'conio.h' library.
- DOS (25x80), compiled with Turbo C, and
  its 'conio.h' library.
- GNU/Linux (24x80), compiled with GCC and ncurses.


Configuration
-------------

The `TECF` configuration tool is supplied in order to adjust
some `TE` generic options to your taste, if needed (its use
is totally optional):

```
TECF v1.21 / 13 Nov 2021 - (c) 2021 Miguel Garcia / FloppySoftware

TE configuration tool.

Usage:
        tecf action arguments...

Actions:
        patch COM file from CF file:
                patch [[filename[.COM]] [filename[.CF]]]
        dump CF values from COM file:
                dump [filename[.COM]] [> filename.CF]
        print key codes:
                key
```

You can configure TE by using the `patch` action - ie:

```
tecf patch te.com myconf.cf
```

The standard file types (`COM`, `CF`) and filename (`TE`) are optionals,
so you could patch `te.com` with `te.cf` just with the command:

```
tecf patch
```

Take into account that the disk has to be enough space for a temporary
file with the exact size as TE.

You can see the current configuration of TE by using
the `dump` action - ie:

```
tecf dump
```

To save the configuration in a file just add a redirection (a `tecf`
feature thanks to `MESCC`):

```
tecf dump > myconf.cf
```

The standard file type (`COM`) and filename (`TE`) are optionals.

If you don't know the character/s assigned to a key, the `key` action
will help you:

```
tecf key
```

For example:

```
TECF v1.21 / 13 Nov 2021 - (c) 2021 Miguel Garcia / FloppySoftware

TE configuration tool.

Press any key to get its code/s (SPACE = exit):

Hex Dec Ctl Chr
--- --- --- ---
61  97      a
04  4   ^D
0D  13  ^M
08  8   ^H
E0  224
53  83      S
0D  13  ^M
```

A `CF` configuration file is just a text file with some key / value
pairs, in a very similar way of `INI` and `conf` files in other
systems.

Just a few rules:
- comments start with the `#` character.
- empty lines are ignored.
- strings can be surrounded by the `"` character.

See my `CF` and `CF READER` projects for more information.

Each adaptation is supplied and patched with an initial configuration
file that includes key bindings, etc.

You should create your own configuration files if needed.

The current supported options are the following (see `TE.CF`):

```
# ========================================
# TE configuration file
# ----------------------------------------
# IMPORTANT! This file is for TECF, the TE
#            configuration tool.
# ========================================

# Configuration name
#te.confName = "My own configuration"

# Screen height: 8..255 or auto for CP/M 3
#screen.rows = 25

# Screen width: 64..255 or auto for CP/M 3
#screen.columns = 80

# Character for normal position in ruler
#screen.rulerChar = "."

# Character for tab stop position in ruler
#screen.rulerTabChar = "!"

# Character for vertical draws -- ie: help columns
#screen.vertChar = "|"

# Character for horizontal draws -- ie: status line
#screen.horizChar = "-"

# Character between line numbers and text
screen.lineNumbersChar = " "

# How many lines can edit: 256..4096 -- each
# empty line occupies 2 bytes of RAM
editor.maxLines = 512

# How many columns (spaces) are a TAB: 1..16
editor.tabSize = 4

# Show line numbers: true / false
editor.lineNumbers = true

# C language type completion: [], {}, (), "", '', /**/
editor.c_language = true

# Automatically indent a new line as the previous one
editor.autoIndent = true

# Automatically start a list when the first non blank
# character in a line is one of "listBullets" -- max.
# 7 characters in long
editor.autoList = true
editor.listBullets = "-*>"

# Some keynames used in the UI
#keyname.newLine = "ENTER"
#keyname.escape = "ESC"

# Key bindings, legal definitions are:
# ^A   -> Ctrl-A         -> one control char.
# ^A^X -> Ctrl-A, Ctrl-X -> two control chars.
# ^AX  -> Ctrl-A, X      -> one control char., one char.
#
# Legal control characters are:
# ^A..^Z ^[ ^\ ^] ^^ ^_ -> 1..31
# ^?                    -> 127
#key.up = ""
#key.down = ""
#key.left = ""
#key.right = ""
#key.begin = ""
#key.end = ""
#key.top = ""
#key.bottom = ""
#key.pgUp = ""
#key.pgDown = ""
#key.indent = ""
#key.newLine = ""
#key.escape = ""
#key.delRight = ""
#key.delLeft = ""
#key.cut = ""
#key.copy = ""
#key.paste = ""
#key.delete = ""
#key.clearClip = ""
#key.find = ""
#key.findNext = ""
#key.goLine = ""
#key.wordLeft = ""
#key.wordRight = ""
#key.blockStart = ""
#key.blockEnd = ""
#key.blockUnset = ""
#key.macro = ""
```


Text tool
---------

The text editor is supplied with `TETX` a text tool in order to help you to process
text files.

```
TETX v1.01 / 20 Nov 2021 - (c) 2021 Miguel Garcia / FloppySoftware

TE text tool.

Usage:
        tetx [-options...] [fname...] [> fname]

Options:
        -e     Skip empty lines.
        -l     Convert to lowercase.
        -n     Print line numbers.
        -t[N]  Convert tab to spaces (default = 4).
        -u     Convert to uppercase.
        -wN    Max. width of lines (32..255).
        -s     Trim spaces on the right.

Examples:
        tetx -ne mydoc.txt
        tetx -t4u -n letter1.txt letter2.txt > letters.txt
```

The `-n` option will print the line number before each line:

```
   25 This is a text line,
   26 and this is another one.
```

The `-t` option will convert tab characters to spaces,
there is a tab each N columns:

```
+   +   +   +   +   +   +   +
This line does not have a tab.
	But this one has one.
		And this one has two.
A tab	here.
```

The `-w` option will split lines longer than N characters:

```
This is a line longer than 32 characters.
```

The command `tetx -w32` will process the line as:

```
This is a line longer than 32 ch
aracters.
```

You can combine options as `-w32nu` or `-w32n -u`:

```
    1 THIS IS A TEXT WITH LINE N
    2 UMBERS. IT HAS A MAXIMUM W
    3 IDTH OF 32 CHARACTERS AND
    4 IT HAS BEEN CONVERTED TO U
    5 PPERCASE.
```

The output is written to `stdout` (the console). So, you
must use output redirection to a file, in order to save
the result:

```
tetx -w32nu draft.txt > final.txt
```

You can omit the input filename, in order to read the input
from `stdin` (the console):

```
tetx -w32nu > letter.txt
```

Or you can combine two or more files (`-` means `stdin`):

```
tetx -w32nu header.txt body.txt footer.txt > letter.txt
tetx -w32nu header.txt - footer.txt > letter.txt
```


License
-------

This program is freeware, and it's licensed under the GNU GPL license.

See the file `copying.txt` for more details.


About this file
---------------

Of course, this text file has been created and edited with te.
