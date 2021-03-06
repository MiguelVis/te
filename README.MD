te
==

v1.70 / 01 Jul 2021 for CP/M

(c) 2015-2021 Miguel Garcia / FloppySoftware

[http://www.floppysoftware.es/](www.floppysoftware.es)

**NOTE**: This project has been extracted from
my [RetroProjects](https://github.com/MiguelVis/RetroProjects) repository.

Introduction
------------

The `te` text editor is small, humble, and useful.

It edits the full text in memory, so your system must leave enough
free TPA for the text, after `te` is loaded and running.

It's screen oriented, and can be easily adapted to a lot of computers.

It's written in the C language, and there are ports for various
operating systems: CP/M, DOS, Windows and GNU/Linux.

I use [MESCC](https://github.com/MiguelVis/mescc) to compile it for CP/M.

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

Macros are not affected by auto-completion, auto-indentation or lists.

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
{Up} {Down} {Left} {Right}     move the cursor one character at a time
{Begin} {End}                  go to the begin / end of the current line
{Top} {Bottom}                 go to the begin / end of the current file
{NewLine}                      insert a break or empty line
{Indent}                       insert spaces as indentation
{DelLeft} {DelRight}           delete the character at the left / right
{Cut} {Copy} {Paste} {Delete}  operation over a line or block of lines
{ClearClip}                    clear the clipboard data
{BlockStart} {BlockEnd}        mark the start / end of a block of lines
{FileName}                     insert the current filename
```

The symbol names are case insensitive - ie: `{ClearClip}` and `{clearclip}`
are equivalents.

Additionally, you can repeat a symbol as in:

```
{up:12}
```

Take into account that line endings in the macro are not translated
to NewLine actions. Use `{NewLine}` for that.

If you need to insert the `{` or `}` characters in the text of the
macro, just write them as `\{` and `\}`.

For example, a macro to insert a banner at the top of the document,
could be written as:

```
{top}{newline}{up}
=============={newline}
FloppySoftware{newline}
=============={newline}
```

Some macro files are included as examples:

```
HEADER.M     Above example.
CFILE.M      C language file documentation block for GDOC.
CFUN.M       C language function documentation block for GDOC.
JOINUP.M     Join current line with the previous one.
JOINDOWN.M   Join current line with the next one.
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
TECF v1.20 / 01 Jul 2021 - (c) 2021 Miguel Garcia / FloppySoftware

TE configuration tool.

Usage:
        tecf action arguments...

Actions:
        patch COM file from CF file:
                patch [[filename[.COM]] [filename[.CF]]]
        dump CF values from COM file:
                dump [filename[.COM]] [> filename.CF]

        Default value for "filename" is "TE".
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


License
-------

This program is freeware, and it's licensed under the GNU GPL license.

See the file `copying.txt` for more details.


About this file
---------------

Of course, this text file has been created and edited with te.
