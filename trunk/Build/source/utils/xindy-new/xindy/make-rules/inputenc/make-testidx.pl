#!/usr/bin/perl

print <<"EOF";
\\documentclass{article}
% this file was generated by make-testidx.pl
\\usepackage[@ARGV[0]]{fontenc}
\\usepackage[@ARGV[1]]{inputenc}
\\makeindex
\\begin{document}
% test of index writing with inputenc
.
EOF

for ($i = 128; $i < 256; $i++) {
  printf("\\index{%c--%2x}\n", $i, $i);
}

print <<"EOF";
\\end{document}
EOF
