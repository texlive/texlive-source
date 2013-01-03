#!/usr/bin/perl

# /****************************************************************************\
#  Part of the XeTeX typesetting system
#  Copyright (c) 1994-2008 by SIL International
#  Copyright (c) 2009 by Jonathan Kew
# 
#  SIL Author(s): Jonathan Kew
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
# FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
# Except as contained in this notice, the name of the copyright holders
# shall not be used in advertising or otherwise to promote the sale,
# use or other dealings in this Software without prior written
# authorization from the copyright holders.
# \****************************************************************************/

# create unicode-letters.tex by processing Unicode data files
#	UnicodeData.txt
#	EastAsianWidth.txt
#	LineBreak.txt

# The intent of unicode-letters.tex is to initialize basic properties for most
# of the "letters" in Unicode, as follows:
#
# \catcode = 11 for everything with GC=L*
# \sfcode  = 999 for everything that has a to-lowercase mapping
# \uccode, \lccode based on the case mapping fields
# \XeTeXmathcode = (7 1 #) for BMP letters, (0 1 #) for SMP
# \XeTeXspacingclass values based on a few of the Unicode line-break classes:
#	'CM' => 256,	# combining marks = transparent
#	'ID' => 1,		# ideograph
#	'OP' => 2,		# opener
#	'CL' => 3,		# closer
#	'NS' => 3,		# non-starter
#	'EX' => 3,		# exclamation
#	'IS' => 3,		# infix sep (comma etc)
#	'QU' => 4,		# ambiguous quote

die "usage: perl $0 UnicodeData.txt EastAsianWidth.txt LineBreak.txt > unicode-letters.tex\n"
	unless $#ARGV == 2;

open Unidata, $ARGV[0] or die "can't read $ARGV[0]";
while (<Unidata>) {
	chomp;
	@u = split(/;/);
	unless ($u[1] =~ /</) {
		$lccode{$u[0]} = $u[13] if $u[13] ne '';
		$lccode{$u[0]} = $u[0]  if $u[13] eq '' and ($u[2] =~ /^L/ or $u[12] ne '');
		$uccode{$u[0]} = $u[12] if $u[12] ne '';
		$uccode{$u[0]} = $u[0]  if $u[12] eq '' and ($u[2] =~ /^L/ or $u[13] ne '');
		if ($u[2] =~ /^L/) {
			push(@letters, $u[0]);
		}
		elsif ($u[2] =~ /^M/) {
			push(@marks, $u[0]);
		}
		elsif (exists $lccode{$u[0]} or exists $uccode{$u[0]}) {
			push(@casesym, $u[0]);
		}
	}
}
close Unidata;

$date = `date`;
chomp $date;
print << "__EOT__";
% Do not edit this file!
% Created from UnicodeData.txt, EastAsianWidth.txt, LineBreak.txt
% by unicode-char-prep.pl on $date.
% In case of errors, fix the Perl script instead.
__EOT__

print << '__EOT__';

\begingroup
\catcode`\{=1 \catcode`\}=2 \catcode`\#=6

\message{loading Unicode properties}

% test whether xetex is new enough to set codes on supplementary-plane chars
% (requires version > 0.996)
\let\ifSupplementary=\iffalse
\def\Supplementarytrue{\let\ifSupplementary=\iftrue}
\ifnum\XeTeXversion>0 \Supplementarytrue
\else
  \def\getrevnumber.#1.#2-#3\end{\count255=#1 }
  \expandafter\getrevnumber\XeTeXrevision.-\end
  \ifnum\count255>996 \Supplementarytrue \fi
\fi
% definitions for classes and case mappings based on UnicodeData.txt
\def\C #1 #2 #3 {\global\uccode"#1="#2 \global\lccode"#1="#3 } % case mappings (non-letter)
\def\L #1 #2 #3 {\global\catcode"#1=11 % category: letter
  \C #1 #2 #3 % with case mappings
  \ifnum"#1="#3 \else \global\sfcode"#1=999 \fi % uppercase letters have sfcode=999
  \ifnum"#1<"10000 \global\XeTeXmathcode"#1="7"01"#1 % BMP letters default to class 7 (var), fam 1
  \else \global\XeTeXmathcode"#1="0"01"#1 % SMP letters class 0 (regular), fam 1
  \fi}
\def\l #1 {\L #1 #1 #1 } % letter without case mappings
\let\m=\l % combining mark - treated as uncased letter

__EOT__

$supp = 0;
for (@letters) {
	if ((hex $_ > 0xffff) and ($supp == 0)) {
		print "\\ifSupplementary\n";
		$supp = 1;
	}
	if (exists $uccode{$_} or exists $lccode{$_}) {
		if (($lccode{$_} eq $_) and ($uccode{$_} eq $_)) {
			print "\\l $_\n";
		}
		else {
			print "\\L $_ ";
			print exists $uccode{$_} ? $uccode{$_} : "0";
			print " ";
			print exists $lccode{$_} ? $lccode{$_} : "0";
			print "\n";
		}
	}
	else {
		print "\\l $_\n";
	}
}
if ($supp == 1) {
	print "\\fi % end ifSupplementary\n";
	$supp = 0;
}

for (@casesym) {
	if ((hex $_ > 0xffff) and ($supp == 0)) {
		print "\\ifSupplementary\n";
		$supp = 1;
	}
	print "\\C $_ ";
	print exists $uccode{$_} ? $uccode{$_} : "0";
	print " ";
	print exists $lccode{$_} ? $lccode{$_} : "0";
	print "\n";
}
if ($supp == 1) {
	print "\\fi % end ifSupplementary\n";
	$supp = 0;
}

for (@marks) {
	if ((hex $_ > 0xffff) and ($supp == 0)) {
		print "\\ifSupplementary\n";
		$supp = 1;
	}
	print "\\m $_\n";
}
if ($supp == 1) {
	print "\\fi % end ifSupplementary\n";
	$supp = 0;
}

print << '__EOT__';

% set \sfcode for Unicode closing quotes
\sfcode"2019=0
\sfcode"201D=0

% check whether the interchar toks features are present
\ifx\XeTeXinterchartoks\XeTeXcharclass
  \def\next{\endgroup\endinput}
\else
  \let\next\relax
\fi
\next

__EOT__

open EAW, $ARGV[1] or die "can't read $ARGV[1]";
while (<EAW>) {
	chomp;
	s/ *#.*//;
	s/ +$//;
	if (m/([0-9A-F]{4,6})(?:\.\.([0-9A-F]{4,6}))?;(.+)/) {
		$s = $1;
		$e = $2;
		$w = $3;
		$e = $s if $e eq '';
		for ($i = hex "0x$s"; $i <= hex "0x$e"; ++$i) {
			$width[$i] = $w;
		}
	}
}
close EAW;

%lineBreakClass = (
	'CM' => 256,	# combining marks = transparent
	'ID' => 1,		# ideograph
	'OP' => 2,		# opener
	'CL' => 3,		# closer
	'NS' => 3,		# non-starter
	'EX' => 3,		# exclamation
	'IS' => 3,		# infix sep (comma etc)
);

print << '__EOT__';
\message{and character classes}

% definitions for script classes based on LineBreak.txt
__EOT__

foreach (keys %lineBreakClass) {
	print "\\def\\$_ #1 #2 {\\set{#1}{#2}{\\global\\XeTeXcharclass\\n=$lineBreakClass{$_}";
	print " \\global\\catcode\\n=11" if m/ID/;
	print " }}\n";
}

print << '__EOT__';
\countdef\n=255
\def\set#1#2#3{% execute setting "#3" with \n set to each value from #1 to #2
  \n="#1 \loop #3 \ifnum\n<"#2 \advance\n by 1 \repeat}
% copy \loop etc from plain.tex
\def\loop#1\repeat{\def\body{#1}\iterate}
\def\iterate{\body \let\next\iterate \else\let\next\relax\fi \next}
\let\repeat=\fi % this makes \loop...\if...\repeat skippable

__EOT__

open LineBreak, $ARGV[2] or die "can't read $ARGV[2]";
while (<LineBreak>) {
	chomp;
	s/ *#.*//;
	s/ +$//;
	if (m/([0-9A-F]{4,6})(?:\.\.([0-9A-F]{4,6}))?;(..)/) {
		$s = $1;
		$e = $2;
		$lb = $3;
		$e = $s if $e eq '';
		if (exists $lineBreakClass{$lb}) {
			if ($lineBreakClass{$lb} == 1) {
				# ideographs: set whole range to class 1
				print "\\$lb $s $e\n";
			}
			else {
				# punctuation, etc: only set these by default for chars with EAW = W, F, H
				for ($i = hex "0x$s"; $i <= hex "0x$e"; ++$i) {
					if ($width[$i] =~ /^[WFH]$/) {
						printf "\\$lb %04X %04X\n", $i, $i;
					}
				}
			}
		}
	}
}
close LineBreak;

print << '__EOT__';

% basic line-breaking support for CJK etc; override for more sophisticated spacing,
% automatic font changes, etc.

\gdef\xtxHanGlue{\hskip0pt plus 0.1em\relax} % between ideographs
\gdef\xtxHanSpace{\hskip0.2em plus 0.2em minus 0.1em\relax} % before/after runs of CJK

\global\XeTeXinterchartoks 0 1 = {\xtxHanSpace}
\global\XeTeXinterchartoks 0 2 = {\xtxHanSpace}
\global\XeTeXinterchartoks 0 3 = {\nobreak\xtxHanSpace}

\global\XeTeXinterchartoks 1 0 = {\xtxHanSpace}
\global\XeTeXinterchartoks 2 0 = {\nobreak\xtxHanSpace}
\global\XeTeXinterchartoks 3 0 = {\xtxHanSpace}

\global\XeTeXinterchartoks 1 1 = {\xtxHanGlue}
\global\XeTeXinterchartoks 1 2 = {\xtxHanGlue}
\global\XeTeXinterchartoks 1 3 = {\nobreak\xtxHanGlue}

\global\XeTeXinterchartoks 2 1 = {\nobreak\xtxHanGlue}
\global\XeTeXinterchartoks 2 2 = {\nobreak\xtxHanGlue}
\global\XeTeXinterchartoks 2 3 = {\xtxHanGlue}

\global\XeTeXinterchartoks 3 1 = {\xtxHanGlue}
\global\XeTeXinterchartoks 3 2 = {\xtxHanGlue}
\global\XeTeXinterchartoks 3 3 = {\nobreak\xtxHanGlue}

\endgroup

__EOT__
