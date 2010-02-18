#!/usr/bin/env perl
# $Id: bibtex-openout-test.pl 16507 2009-12-25 01:19:05Z karl $
# Public domain.  Originally written 2010, Karl Berry.
# Check that dvips quotes external command arguments.

BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `dirname $0`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main
{
  # create the weirdly-named file which dvips executes with popen.
  # quotecmd.tex itself also creates it, but we don't want to run TeX in
  # this test, nor do we want such a weirdly-named file in our
  # repository, so create it here.  Leave it in place, so we can run the
  # program under the debugging if we need to.
  my $weirdf = ' 2>&1 | echo badnews >pwned.txt #.gz';
  unlink ($weirdf);  # ensure no leftover
  open (WEIRDF, ">", $weirdf);
  close (WEIRDF) || die "open(weird file name) failed: $!";
  
  $badfile = "pwned.txt";  # will be created if program is broken
  unlink ($badfile);       # ensure no leftover from previous test
  
  my @args = ("$srcdir/testdata/quotecmd.dvi", qw(-o /dev/null));
  my $ret = &test_run ("./dvips", @args);

  my $bad = -f $badfile;  # file should not have been created
  return $bad;
}
