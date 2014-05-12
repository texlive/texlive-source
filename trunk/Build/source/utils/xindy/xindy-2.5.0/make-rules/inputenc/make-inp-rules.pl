#!/usr/bin/perl

print <<"EOF";
;; this file was generated by make-inp-rules.pl
;; these rules map inputenc generated macros (back) to 8-bit characters

EOF


while (<STDIN>) {
  if (/\\indexentry{(.+)--([8-9a-f][0-9a-f])\}\{1\}/) {
    if (!($1 =~ /inputenc Error|\@inpenc\@undefined/)) {
      $i = hex($2);
      $macro = $1;
      $macro =~ s/\~/~~/g;
      $macro =~ s/\"/~\"/g;
      printf("(merge-rule \"%s\" \"%c\" :string)\n",
        $macro, $i);
      if (@ARGV[0] eq "cyracc" &&
        $macro =~ /(cyra |cyre |cyri |cyro |cyru |cyrerev |cyryu |cyrya )/i) {
        printf("(merge-rule \"\\'{%s}\" \"\\'%c\" :string)\n",
          $macro, $i);
      }
    }
  }
}

print <<"EOF";

;; end of style file.
EOF
