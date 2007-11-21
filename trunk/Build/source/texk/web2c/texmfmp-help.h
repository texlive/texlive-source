/* The help messages for TeX & MF family of programs.

Copyright 2001-05 Olaf Weber.
Copyright 1995, 96 Karl Berry.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef TEXMFMP_HELP
#define TEXMFMP_HELP

#ifdef Aleph
const_string ALEPHHELP[] = {
    "Usage: aleph [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: aleph [OPTION]... \\FIRST-LINE",
    "   or: aleph [OPTION]... &FMT ARGS",
    "  Run Aleph on TEXNAME, usually creating TEXNAME.dvi.",
    "  Any remaining COMMANDS are processed as Aleph input, after TEXNAME is read.",
    "  If the first line of TEXNAME is %&FMT, and FMT is an existing .fmt file,",
    "  use it.  Else use `NAME.fmt', where NAME is the program invocation name,",
    "  most commonly `aleph'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of Aleph input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "-etex                   enable e-TeX extensions",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-fmt=FMTNAME            use FMTNAME instead of program name or a %& line",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be inialeph, for dumping formats; this is implicitly",
    "                          true if the program name is `inialeph'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
#ifdef IPC
    "-ipc                    send DVI output to a socket as well as the usual",
    "                          output file",
    "-ipc-start              as -ipc, and also start the server at the other end",
#endif /* IPC */
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to the",
    "                          bits of NUMBER",
    "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=tex/tfm)",
    "-output-comment=STRING  use STRING for DVI file comment instead of date",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-progname=STRING        set program (and fmt) name to STRING",
    "-recorder               enable filename recorder (always on)",
    "[-no]-shell-escape      disable/enable \\write18{SHELL COMMAND}",
    "-src-specials           insert source specials into the DVI file",
    "-src-specials=WHERE     insert source specials in certain places of",
    "                          the DVI file. WHERE is a comma-separated value",
    "                          list: cr display hbox math par parend vbox",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* Aleph */

#ifdef eTeX
const_string ETEXHELP[] = {
    "Usage: etex [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: etex [OPTION]... \\FIRST-LINE",
    "   or: etex [OPTION]... &FMT ARGS",
    "  Run e-TeX on TEXNAME, usually creating TEXNAME.dvi.",
    "  Any remaining COMMANDS are processed as e-TeX input, after TEXNAME is read.",
    "  If the first line of TEXNAME is %&FMT, and FMT is an existing .fmt file,",
    "  use it.  Else use `NAME.fmt', where NAME is the program invocation name,",
    "  most commonly `etex'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of e-TeX input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "-enc                    enable encTeX extensions such as \\mubyte",
    "-etex                   enable e-TeX extensions",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-fmt=FMTNAME            use FMTNAME instead of program name or a %& line",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be einitex, for dumping formats; this is implicitly",
    "                          true if the program name is `einitex'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
#ifdef IPC
    "-ipc                    send DVI output to a socket as well as the usual",
    "                          output file",
    "-ipc-start              as -ipc, and also start the server at the other end",
#endif /* IPC */
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
    "                          the bits of NUMBER",
    "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=tex/tfm)",
    "-mltex                  enable MLTeX extensions such as \\charsubdef",
    "-output-comment=STRING  use STRING for DVI file comment instead of date",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-progname=STRING        set program (and fmt) name to STRING",
    "-recorder               enable filename recorder",
    "[-no]-shell-escape      disable/enable \\write18{SHELL COMMAND}",
    "-src-specials           insert source specials into the DVI file",
    "-src-specials=WHERE     insert source specials in certain places of",
    "                          the DVI file. WHERE is a comma-separated value",
    "                          list: cr display hbox math par parend vbox",
    "-translate-file=TCXNAME use the TCX file TCXNAME",
    "-8bit                   make all characters printable by default",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* eTeX */

#ifdef MF
const_string MFHELP[] = {
    "Usage: mf [OPTION]... [MFNAME[.mf]] [COMMANDS]",
    "   or: mf [OPTION]... \\FIRST-LINE",
    "   or: mf [OPTION]... &BASE ARGS",
    "  Run Metafont on MFNAME, usually creating MFNAME.tfm and MFNAME.NNNNgf,",
    "  where NNNN is the resolution of the specified mode (2602 by default).",
    "  Any following COMMANDS are processed as Metafont input,",
    "  after MFNAME is read.",
    "  If the first line of MFNAME is %&BASE, and BASE is an existing .base file,",
    "  use it.  Else use `NAME.base', where NAME is the program invocation name,",
    "  most commonly `mf'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of Metafont input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the BASE to read, overriding all else. Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "-base=BASENAME          use BASENAME instead of program name or a %& line",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be inimf, for dumping bases; this is implicitly",
    "                          true if the program name is `inimf'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
    "                          the bits of NUMBER",
    "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=mf)",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-progname=STRING        set program (and base) name to STRING",
    "-recorder               enable filename recorder",
    "-translate-file=TCXNAME use the TCX file TCXNAME",
    "-8bit                   make all characters printable by default",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* MF */

#ifdef MP
const_string MPHELP[] = {
    "Usage: mpost [OPTION]... [MPNAME[.mp]] [COMMANDS]",
    "   or: mpost [OPTION]... \\FIRST-LINE",
    "   or: mpost [OPTION]... &MEM ARGS",
    "  Run MetaPost on MPNAME, usually creating MPNAME.NNN (and perhaps",
    "  MPNAME.tfm), where NNN are the character numbers generated.",
    "  Any remaining COMMANDS are processed as MetaPost input,",
    "  after MPNAME is read.",
    "  If the first line of MPNAME is %&MEM, and MEM is an existing .mem file,",
    "  use it.  Else use `NAME.mem', where NAME is the program invocation name,",
    "  most commonly `mp'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of MetaPost input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the MEM to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be inimpost, for dumping mems; this is implicitly",
    "                          true if the program name is `inimpost'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
    "                          the bits of NUMBER",
    "-mem=MEMNAME            use MEMNAME instead of program name or a %& line",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-progname=STRING        set program (and mem) name to STRING",
    "-recorder               enable filename recorder",
    "-tex=TEXPROGRAM         use TEXPROGRAM for text labels.",
    "-translate-file=TCXNAME use the TCX file TCXNAME",
    "-8bit                   make all characters printable by default",
    "-T, -troff              set the prologues variable, use `makempx -troff'",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* MP */

#ifdef Omega
const_string OMEGAHELP[] = {
    "Usage: omega [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: omega [OPTION]... \\FIRST-LINE",
    "   or: omega [OPTION]... &FMT ARGS",
    "  Run Omega on TEXNAME, usually creating TEXNAME.dvi.",
    "  Any remaining COMMANDS are processed as Omega input, after TEXNAME is read.",
    "  If the first line of TEXNAME is %&FMT, and FMT is an existing .fmt file,",
    "  use it.  Else use `NAME.fmt', where NAME is the program invocation name,",
    "  most commonly `omega'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of Omega input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-fmt=FMTNAME            use FMTNAME instead of program name or a %& line",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be iniomega, for dumping formats; this is implicitly",
    "                          true if the program name is `iniomega'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
#ifdef IPC
    "-ipc                    send DVI output to a socket as well as the usual",
    "                          output file",
    "-ipc-start              as -ipc, and also start the server at the other end",
#endif /* IPC */
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
    "                          the bits of NUMBER",
    "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=tex/tfm)",
    "-output-comment=STRING  use STRING for DVI file comment instead of date",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-progname=STRING        set program (and fmt) name to STRING",
    "-recorder               enable filename recorder (always on)",
    "[-no]-shell-escape      disable/enable \\write18{SHELL COMMAND}",
    "-src-specials           insert source specials into the DVI file",
    "-src-specials=WHERE     insert source specials in certain places of",
    "                          the DVI file. WHERE is a comma-separated value",
    "                          list: cr display hbox math par parend vbox",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* Omega */

#ifdef eOmega
const_string EOMEGAHELP[] = {
    "Usage: eomega [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: eomega [OPTION]... \\FIRST-LINE",
    "   or: eomega [OPTION]... &FMT ARGS",
    "  Run e-Omega on TEXNAME, usually creating TEXNAME.dvi.",
    "  Any remaining COMMANDS are processed as e-Omega input, after TEXNAME is read.",
    "  If the first line of TEXNAME is %&FMT, and FMT is an existing .fmt file,",
    "  use it.  Else use `NAME.fmt', where NAME is the program invocation name,",
    "  most commonly `eomega'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of e-Omega input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-fmt=FMTNAME            use FMTNAME instead of program name or a %& line",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be inieomega, for dumping formats; this is implicitly",
    "                          true if the program name is `inieomega'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
#ifdef IPC
    "-ipc                    send DVI output to a socket as well as the usual",
    "                          output file",
    "-ipc-start              as -ipc, and also start the server at the other end",
#endif /* IPC */
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
    "                          the bits of NUMBER",
    "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=tex/tfm)",
    "-output-comment=STRING  use STRING for DVI file comment instead of date",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-progname=STRING        set program (and fmt) name to STRING",
    "-recorder               enable filename recorder (always on)",
    "[-no]-shell-escape      disable/enable \\write18{SHELL COMMAND}",
    "-src-specials           insert source specials into the DVI file",
    "-src-specials=WHERE     insert source specials in certain places of",
    "                          the DVI file. WHERE is a comma-separated value",
    "                          list: cr display hbox math par parend vbox",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* eOmega */

#ifdef pdfTeX
const_string PDFTEXHELP[] = {
    "Usage: pdftex [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: pdftex [OPTION]... \\FIRST-LINE",
    "   or: pdftex [OPTION]... &FMT ARGS",
    "  Run pdfTeX on TEXNAME, usually creating TEXNAME.pdf.",
    "  Any remaining COMMANDS are processed as pdfTeX input, after TEXNAME is read.",
    "  If the first line of TEXNAME is %&FMT, and FMT is an existing .fmt file,",
    "  use it.  Else use `NAME.fmt', where NAME is the program invocation name,",
    "  most commonly `pdftex'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of pdfTeX input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "-draftmode              switch on draft mode (generates no output PDF)",
    "-enc                    enable encTeX extensions such as \\mubyte",
    "-etex                   enable e-TeX extensions",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-fmt=FMTNAME            use FMTNAME instead of program name or a %& line",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be pdfinitex, for dumping formats; this is implicitly",
    "                          true if the program name is `pdfinitex'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
#ifdef IPC
    "-ipc                    send DVI output to a socket as well as the usual",
    "                          output file",
    "-ipc-start              as -ipc, and also start the server at the other end",
#endif /* IPC */
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
    "                          the bits of NUMBER",
    "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=tex/tfm)",
    "-mltex                  enable MLTeX extensions such as \\charsubdef",
    "-output-comment=STRING  use STRING for DVI file comment instead of date",
    "                          (no effect for PDF)",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "-output-format=FORMAT   use FORMAT for job output; FORMAT is `dvi' or `pdf'",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-progname=STRING        set program (and fmt) name to STRING",
    "-recorder               enable filename recorder",
    "[-no]-shell-escape      disable/enable \\write18{SHELL COMMAND}",
    "-src-specials           insert source specials into the DVI file",
    "-src-specials=WHERE     insert source specials in certain places of",
    "                          the DVI file. WHERE is a comma-separated value",
    "                          list: cr display hbox math par parend vbox",
    "-translate-file=TCXNAME use the TCX file TCXNAME",
    "-8bit                   make all characters printable by default",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* pdfTeX */

#ifdef XeTeX
const_string XETEXHELP[] = {
    "Usage: xetex [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: xetex [OPTION]... \\FIRST-LINE",
    "   or: xetex [OPTION]... &FMT ARGS",
    "  Run XeTeX on TEXNAME, usually creating TEXNAME.pdf.",
    "  Any remaining COMMANDS are processed as XeTeX input, after TEXNAME is read.",
    "  If the first line of TEXNAME is %&FMT, and FMT is an existing .fmt file,",
    "  use it.  Else use `NAME.fmt', where NAME is the program invocation name,",
    "  most commonly `xetex'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of XeTeX input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "-etex                   enable e-TeX extensions",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-fmt=FMTNAME            use FMTNAME instead of program name or a %& line",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be xeinitex, for dumping formats; this is implicitly",
    "                          true if the program name is `xeinitex'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
    "                          the bits of NUMBER",
    "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=tex/tfm)",
    "-mltex                  enable MLTeX extensions such as \\charsubdef",
    "-output-comment=STRING  use STRING for XDV file comment instead of date",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "-output-driver=CMD      use CMD as the XDV-to-PDF driver instead of xdvipdfmx",
    "-no-pdf                 generate XDV (extended DVI) output rather than PDF",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-papersize=STRING       set PDF media size to STRING",
    "-progname=STRING        set program (and fmt) name to STRING",
    "-recorder               enable filename recorder",
    "[-no]-shell-escape      disable/enable \\write18{SHELL COMMAND}",
    "-src-specials           insert source specials into the XDV file",
    "-src-specials=WHERE     insert source specials in certain places of",
    "                          the XDV file. WHERE is a comma-separated value",
    "                          list: cr display hbox math par parend vbox",
    "-translate-file=TCXNAME (ignored)",
    "-8bit                   make all characters printable, don't use ^^X sequences",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* XeTeX */

#ifdef TeX
const_string TEXHELP[] = {
    "Usage: tex [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: tex [OPTION]... \\FIRST-LINE",
    "   or: tex [OPTION]... &FMT ARGS",
    "  Run TeX on TEXNAME, usually creating TEXNAME.dvi.",
    "  Any remaining COMMANDS are processed as TeX input, after TEXNAME is read.",
    "  If the first line of TEXNAME is %&FMT, and FMT is an existing .fmt file,",
    "  use it.  Else use `NAME.fmt', where NAME is the program invocation name,",
    "  most commonly `tex'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of TeX input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "-enc                    enable encTeX extensions such as \\mubyte",
    "[-no]-file-line-error   disable/enable file:line:error style messages",
    "-fmt=FMTNAME            use FMTNAME instead of program name or a %& line",
    "-halt-on-error          stop processing at the first error",
    "-ini                    be initex, for dumping formats; this is implicitly",
    "                          true if the program name is `initex'",
    "-interaction=STRING     set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
#ifdef IPC
    "-ipc                    send DVI output to a socket as well as the usual",
    "                          output file",
    "-ipc-start              as -ipc, and also start the server at the other end",
#endif /* IPC */
    "-jobname=STRING         set the job name to STRING",
    "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
    "                          the bits of NUMBER",
    "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=tex/tfm)",
    "-mltex                  enable MLTeX extensions such as \\charsubdef",
    "-output-comment=STRING  use STRING for DVI file comment instead of date",
    "-output-directory=DIR   use DIR as the directory to write files to",
    "[-no]-parse-first-line  disable/enable parsing of the first line of the",
    "                          input file",
    "-progname=STRING        set program (and fmt) name to STRING",
    "-recorder               enable filename recorder",
    "[-no]-shell-escape      disable/enable \\write18{SHELL COMMAND}",
    "-src-specials           insert source specials into the DVI file",
    "-src-specials=WHERE     insert source specials in certain places of",
    "                          the DVI file. WHERE is a comma-separated value",
    "                          list: cr display hbox math par parend vbox",
    "-translate-file=TCXNAME use the TCX file TCXNAME",
    "-8bit                   make all characters printable by default",
    "-help                   display this help and exit",
    "-version                output version information and exit",
    NULL
};
#endif /* TeX */
#endif /* TEXMFMP_HELP */
