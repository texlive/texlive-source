#!/usr/bin/env perl
#
# ToDos/unsolved problems:
# * collision of symbol links
# * texmf.cnf (path settings, variables)
# * aliases
# * binaries, libraries
#
use strict;
$^W=1;

my $prj = 'mkjobtexmf';
my $version = '0.6';
my $date = '2008/04/05';
my $author = 'Heiko Oberdiek';
my $copyright = "Copyright 2007, 2008 $author";

my $cmd_tex        = 'pdflatex';
my $cmd_kpsewhich  = 'kpsewhich';
my $cmd_texhash    = 'texhash';
my $cmd_strace     = 'strace';
my $ext_tex        = '.tex';
my $ext_recorder   = '.fls';
my $ext_strace     = '.strace';
my $ext_mkjobtexmf = '.mjt';
my $jobname = '';
my $texname = '';
my $destdir = '';
my @args = ();
my @texopt = ();
my $verbose = 0;
my $output = 0;
my $strace = 0;
my $copy = 0;
my $flat = 0;
my $needs_texhash = 0;
my @texmf;
my %files;
my %links;
my %flat_ignore = (
    'ls-R' => '',
    'aliases' => '',
);

my $title = "\U$prj\E $date v$version, $copyright\n";

print $title;

sub die_error ($) {
    my $msg = shift;
    die "!!! Error: $msg!\n";
}

sub warning ($) {
    my $msg = shift;
    print "!!! Warning: $msg!\n";
}

sub verbose (@) {
    my @msg = @_;
    print "* @msg\n" if $verbose;
}

sub value ($) {
    my $value = $_[0];
    "[$value]";
}

sub die_usage {
    my $msg = $_[0];
    pod2usage(
        -exitstatus => 2,
        -msg        => "\n==> $msg!\n");
}

use Getopt::Long;
use Pod::Usage;

my $man = 0;
my $help = 0;

GetOptions(
    'jobname=s'       => \$jobname,
    'texname=s'       => \$texname,
    'texopt=s'        => \@texopt,
    'destdir=s'       => \$destdir,
    'cmd-tex=s'       => \$cmd_tex,
    'cmd-kpsewhich=s' => \$cmd_kpsewhich,
    'cmd-texhash=s'   => \$cmd_texhash,
    'strace'          => \$strace,
    'copy'            => \$copy,
    'flat'            => \$flat,
    'verbose'         => \$verbose,
    'output'          => \$output,
    'help|?'          => \$help,
    'man'             => \$man,
) or die_usage('Unknown option');
pod2usage(1) if $help;
pod2usage(-exitstatus => 0, -verbose => 2) if $man;

if (@ARGV > 0) {
    $strace = 1;
    $texname = '';
    my @args = @ARGV;
}
$jobname or die_usage('Missing jobname');
$texname = "$jobname$ext_tex" unless $texname;
$destdir = "$jobname$ext_mkjobtexmf" unless $destdir;

verbose "jobname: " . value $jobname;
verbose "texname: " . value $texname if $texname;
verbose "command: " . value "@args" if @args;
verbose "destdir: " . value $destdir;

if (!$copy) {
    my $symlink_exists = eval { symlink('', ''); 1 };
    if ($symlink_exists) {
        verbose "symbolic linking: supported";
    }
    else {
        $copy = 1;
        verbose "symbolic linking: unsupported";
    }
}
my $umask = umask;
if (defined($umask)) {
    verbose "umask: " . sprintf("%04o", $umask);
}
else {
    $umask = 0;
    verbose "umask: unsupported";
}

if ($copy) {
    use File::Copy;
}
if ($flat) {
    use File::Basename;
}

sub check_child_error () {
    if ($? != 0) {
        if ($? == -1) {
            die_error "Failed to execute: $!";
        }
        elsif ($? & 127) {
            die_error sprintf "Child died with signal %d, %s coredump",
                      ($? & 127), ($? & 128) ? 'with' : 'without';
        }
        else {
            die_error sprintf "Child exited with value %d", $? >> 8;
        }
    }
    verbose "child exit: ok";
}

sub run_generic (@) {
    my @args = @_;
    my $cmd = $_[0];
    verbose "exec: " . value "@args";
    print '>' x 79, "\n";
    system $cmd @args;
    print '<' x 79, "\n";
    check_child_error;
}

sub run_tex {
    if ($strace) {
        my @run_args;
        if (@args) {
            @run_args = @args;
        }
        else {
            @run_args = (
                $cmd_tex,
                '-interaction=nonstopmode',
                @texopt,
                $texname
            );
        }
        run_generic(
            $cmd_strace,
            '-f',
            '-e',
            'trace=open,access', # trace=file
            '-o',
            "$jobname$ext_strace",
            @run_args
        )
    }
    else {
        run_generic(
            $cmd_tex,
            '-recorder',
            "-jobname=$jobname",
            '-interaction=nonstopmode',
            @texopt,
            $texname
        );
    }
}

sub run_texhash {
    return if $flat;
    if ($needs_texhash) {
        run_generic(
            $cmd_texhash,
            "$destdir/texmf"
        );
    }
    else {
        verbose("texhash run skipped, no files added");
    }
}

use Cwd 'abs_path', 'getcwd';

sub get_texmf_trees () {
    return if $flat;
    my $cmdline = "$cmd_kpsewhich -expand-path='\$TEXMF'";
    verbose "exec: " . value($cmdline);
    my $str = `$cmdline`;
    check_child_error;
    chomp $str;
    @texmf = split ':', $str;
    my %texmf;
    foreach my $texmf (@texmf) {
        $texmf{$texmf} = '';
        $texmf{abs_path($texmf)} = '';
    }
    @texmf = sort keys %texmf;
    if ($verbose) {
        if (@texmf) {
            map { verbose 'texmf: ' . value($_) } @texmf;
        }
        else {
            verbose 'texmf: none';
        }
    }
}

sub analyze_recorder {
    my $pwd = getcwd;
    verbose "pwd: " . value($pwd);

    my $file_rec = $jobname . ($strace ? $ext_strace : $ext_recorder);
    verbose 'File with recorded file names: ' . value($file_rec);
    open(IN, '<', $file_rec)
            or die_error "Cannot open `$file_rec'";
    if ($strace) {
        while (<IN>) {
            chomp;
            next if /\)\s+= -\d/; # -1 ENOENT, ...
            next if /\WO_DIRECTORY\W/; # skip directories
            my $type = 'INPUT';
            if ($output) {
                $type = 'OUTPUT' if /\WO_WRONLY\W/;
            }
            else {
                next if /\WO_WRONLY\W/;
            }
            /^\d+\s+\w+\(\"([^"]+)\",/ or warning "Unknown entry `$_'";
            my $file = $1;
            $files{$file} = '';
        }
    }
    else {
        while (<IN>) {
            chomp;
            next if /^PWD /;
            next if not $output and /^OUTPUT /;
            /^(INPUT|OUTPUT) (.*)$/ or warning "Unknown entry `$_'";
            my $type = $1;
            my $file = $2;
            $files{$file} = '';
        }
    }
    close(IN);
}

sub map_files {
    if ($flat) {
        map_files_flat();
    }
    else {
        map_files_texmf();
    }
}

sub map_files_flat {
    my %abs_files;
    my %names;
    my %clashes;

    foreach my $file (keys %files) {
        $abs_files{abs_path($file)} = '';
    }

    foreach my $file (keys %abs_files) {
        my $name = basename($file);
        next if exists $flat_ignore{$name};
        if (defined($names{$name})) {
            push @{$names{$name}}, $file;
            $clashes{$name} = '';
        }
        else {
            my @a = ($file);
            $names{$name} = \@a;
        }
    }

    foreach my $name (sort keys %clashes) {
        print "* file name clash for " . value($name) . "\n";
        my @a = @{$names{$name}};
        foreach my $file (@a) {
            print "  " . value($file) . "\n";
        }
    }

    foreach my $name (sort keys %names) {
        my $file = @{$names{$name}}[0];
        my $clash = $clashes{$name} ? ' (clash)' : '';
        verbose value($name) . ' => ' . value($file) . $clash;
        $links{$name} = $file;
    }
}

sub map_files_texmf {
    my @failed;

    foreach my $file (sort keys %files) {
        verbose "file: " . value($file);

        my $abs_file = abs_path($file);

        my $found = '';
        foreach (@texmf) {
            my $texmf = "$_/";
            my $len = length($texmf);
            my $str = substr $file, 0, $len;
            if ($texmf eq $str) {
                $found = 'texmf/' . substr $file, $len;
                if ($found =~ /(^|\/)\.\.\//) {
                    $found = '';
                }
            }
            last if $found;
            my $str = substr $abs_file, 0, $len;
            if ($texmf eq $str) {
                $found = 'texmf/' . substr $abs_file, $len;
                last;
            }
        }
        if (not($found)) {
            if ($file =~ /(^|\/)\.\.\// or $file =~ /^\//) {
                push @failed, $file;
            }
            else {
                $found = $file;
            }
        }
        if ($found) {
            $links{$found} = abs_path($file);
        }
    }

    if ($verbose) {
        foreach (sort keys %links) {
            verbose value($_) . ' => ' . value($links{$_});
        }
    }

    foreach (@failed) {
        print "!!! Failed: " . value($_) . "\n";
    }
}

sub make_dirs ($) {
    my $path = shift;
    my @elems = split /\/+/, $path;
    if (@elems <= 1) {
        return;
    }
    pop @elems;
    my $dir = '';
    foreach my $elem (@elems) {
        $dir .= '/' if $dir;
        $dir .= $elem;
        next if -d $dir;
        verbose 'mkdir: ' . value($dir);
        mkdir $dir or die_error "Cannot make directory `$dir'";
    }
}

sub make_links {

    foreach my $key (sort keys %links) {
        my $source = $links{$key};
        my $dest = "$destdir/$key";
        make_dirs $dest;
        if (-e $dest) {
            my $type = '';
            if (-l $dest) {
                $type .= 'link';
            }
            elsif (-f $dest) {
                $type = 'file';
            }
            elsif (-d $dest) {
                $type = 'directory';
            }
            elsif (-b $dest) {
                $type = 'block device';
            }
            elsif (-c $dest) {
                $type = 'character device';
            }
            elsif (-p $dest) {
                $type = 'pipe';
            }
            elsif (-S $dest) {
                $type = 'socket';
            }
            elsif (-t $dest) {
                $type = 'tty';
            }
            $type = " ($type)" if $type;
            verbose "destination$type exists: " . value($dest);
            next;
        }
        $needs_texhash = 1;
        do_link_copy($source, $dest);
    }
}

sub do_link_copy {
    my $source = shift;
    my $dest = shift;
    my $success = 0;

    if ($copy) {
        if (copy($source, $dest) == 1) {
            $success = 1;
            my ($source_mode, $source_atime, $source_mtime)
                    = (stat($source))[2, 8, 9];
            my ($dest_mode, $dest_atime, $dest_mtime)
                    = (stat($dest))[2, 8, 9];
            # preserve executable permissions if necessary
            my $new_dest_mode = $dest_mode
                                | (($source_mode & 0111) & ~$umask);
            if ($new_dest_mode != $dest_mode) {
                if (chmod($new_dest_mode, $dest) < 1) {
                    print "!!! Setting executive mode failed: "
                          . value($dest) . "\n";
                }
            }
            # preserve file times
            if ($source_atime != $dest_atime
                    || $source_mtime != $dest_mtime) {
                if (utime($source_atime, $source_mtime, $dest) < 1) {
                    print "!!! Setting file times failed: "
                          . value($dest) . "\n";
                }
            }
        }
    }
    else {
        if (symlink($source, $dest) == 1) {
            $success = 1;
        }
    }
    if ($success == 0) {
        my $method = $copy ? 'Copying' : 'Symbolic linking';
        print "!!! $method failed:\n    "
              . value($dest) . ' => ' . value($source) . "\n";
    }
}

run_tex;
get_texmf_trees;
analyze_recorder;
map_files;
make_links;
run_texhash;

1;

__DATA__

=head1 NAME

mkjobtexmf -- Generate a texmf tree for a particular job

=head1 SYNOPSIS

The progam B<mkjobtexmf> runs a program and tries to
find the used file names. Two methods are available,
option C<-recorder> of TeX (Web2C) or the program B<strace>.

Then it generates a directory with a texmf tree. It checks
the found files and tries sort them in this texmf tree.

It can be used for archiving purposes or to speed up
following TeX runs.

    mkjobtexmf [options]

This runs TeX that can be configured by options.
Both methods for getting the used file names are available.

    mkjobtexmf [options] -- <cmd> [args]

The latter form runs program I<cmd> with arguments I<args>
instead of TeX. As method only program B<strace> is available.

Options:

    --jobname <name>       Name of the job (mandatory).
                              Usually this is the TeX file
                              without extension
    --texname <file>       Input file for TeX. Default is the
                              job name with extension '.tex'
    --texopt <option>      Option for TeX run
    --destdir <directory>  Destination directory,
                              default is `<jobname>.mjt'
    --output               Add also output files
    --strace               Use strace instead of TeX's
                              option -recorder
    --copy                 Copy files instead of creating
                              symbol links
    --flat                 Junk paths, do not make directories
                              inside the destination directory
    --cmd-tex <cmd>        Command for the TeX compiler
    --cmd-kpsewhich <cmd>  Command for kpsewhich
    --cmd-texhash <cmd>    Command for texhash
    --cmd-strace <cmd>     Command for strace
    --verbose              Verbose output
    --help                 Brief help message
    --man                  Full documentation

=head1 DESCRIPTION

=head2 Running the program

First B<mkjobtexmf> runs a program, usually TeX. The TeX compiler
is configured by option B<--cmd-tex>. Option B<--texname> can
be used, if the file name extension differs from F<.tex>:

    mkjobtexmf --jobname foo --texname foo.ltx

Even more complicate cases are possible:

    mkjobtexmf --jobname foo --texname '\def\abc{...}\input{foo}'

If another program than TeX should be used (dvips, ...),
then this program can be given after C<-->:

    mkjobtexmf --jobname foo -- dvips foo

=head2 File recording

Two methods are available to get the used file names:

=over

=item Recorder of TeX

Some TeX distributions (e.g. Web2C) support the option B<-recorder>
for its TeX compilers. Then the TeX compiler generates a file with
extension F<.fls> that records the used input and output files.

=item Program strace

This program traces system calls and signals. It is used here
to log the used files.

=back

=head2 Analyze and link/copy found files

The result directory F<I<jobname>.mjt> is generated. Inside the
result TEXMF tree is created. Each found file is compared against
a list of paths of TEXMF trees. If a match is found, the file is
linked/copied into the TEXMF tree. The list of paths is generated by
program B<kpsewhich>.

If the file cannot be mapped to a TEXMF tree and the file is
a relative file name, then it is directly linked/copied into the
result directory F<I<jobname>.mjt>. Absolute file names
are not supported and neither paths with links to parent directories.

Symbolic links are created by default. The files are copied
if option --copy is given or symbolic linking is not available.

=head1 OPTIONS

=over

=item B<--jobname>=I<jobname>

It is the name of the job. `<jobname>.tex' serves as default for
the TeX file and <jobname> is used for naming various directories
and files. See section L</FILES>.

=item B<--texname>=I<name>

The name of the TeX input file, if it differs from <jobname>.tex.

=item B<--texopt>=I<opt>

Additional option for the TeX compiler, examples are --ini or
--shell-escape. This option can be given more than once.

=item B<--destdir>=I<directory>

Specifies the name of the destination directory where the result
is collected. As default a directory is generated in the current
directory with the job name and extension `.mjt'.

=item B<--output>

Also add output files.

=item B<--strace>

Use method with program B<strace>, see L<"DESCRIPTION">.

=item B<--copy>

Files are copied instead of creating symbolic links.

=item B<--flat>

Files are linked or copied without path elements.
The destination directory will contain a flat list of
files or links without directory.

The files `ls-R' and `aliases' are ignored.

=item B<--cmd-tex>=I<cmd>

Command for the TeX compiler. Default is pdflatex.

=item B<--cmd-kpsewhich>=I<cmd>

Command for kpsewhich.

=item B<--cmd-texhash>=I<cmd>

Command for updating the file name database of the generated
texmf tree. Default is texmf.

=item B<--cmd-strace>=I<cmd>

Command for strace.

=item B<--verbose>

Verbose messages.

=item B<--help>

Display help screen.

=back

=head1 EXAMPLES

TeX file F<test.tex> using TeX's recorder method:

    mkjobtexmf --jobname test

TeX file F<test.tex> using LaTeX:

    mkjobtexmf --jobname test --cmd-tex latex

Format generation:

    mkjobtexmf --jobname test --texopt -ini --texname pdflatex.ini

Example, how the new texmf tree (Linux/bash) can be used:

    TEXMF=!!test.mjt/texmf pdflatex test

Example for generating a zip archive (Linux/bash):

    (cd test.mjt && zip -9r ../test .)

Example for generating a tar archive:

    tar cjhvf test.tar.bz2 -C test.mjt .

=head1 UNSOLVED ISSUES, CAVEATS, TODOS

=over

=item Experimental software

Options, defaults, how the program works might change in
future versions.

=item F<texmf.cnf>

Currently the method with B<strace> records this files.
TeX's recorder does not. Useful are F<texmf.cnf> files for
variable settings. Because we have just one TEXMF tree,
the path sections should probably rewritten.

=item Settings in environment variables

They are not stored at all.

=item Collisions

The program uses one destination directory and at most
one TEXMF tree for the result. However, the source files
can come from different directories and TEXMF trees.
Therefore name collisions are possible.

The program follows the strategy not to delete files
in the destination directory. That allows to collect files
from differnt runs. Thus collisions are resolved
in the manner that the first entry that is made in
the destination directory wins.

=item Configuration file

It would save the user from retyping the same options again and again.

=item Uncomplete recording

Bugs in TeX's file recording might result in incomplete
file recording (e.g. pdfTeX 1.40.3 does not record .pfb and
.pk files).

=item ...

=back

=head1 FILES

=over

=item F<E<lt>jobnameE<gt>.mjt/>

Directory where the resulting texmf tree and symbol links
are stored. It can be changed by option --destdir.

=item F<E<lt>jobnameE<gt>.fls>

Name of TeX's recorder file.

=item F<E<lt>jobnameE<gt>.strace>

Log file where the result of B<strace> is stored.

=back

=head1 AUTHOR

Heiko Oberdiek, email: oberdiek at uni-freiburg.de

=head1 COPYRIGHT AND LICENSE

Copyright 2007 by Heiko Oberdiek.

This library is free software; you may redistribute it and/or
modify it under the same terms as Perl itself
(Perl Artistic License/GNU General Public License, version 2).

=head1 HISTORY

=over

=item 2007/04/16 v0.1

* First experimental version.

=item 2007/05/09 v0.2

* Typo in option name fixed.

=item 2007/09/03 v0.3

* New options: --copy, --flat, --destdir

=item 2007/09/04 v0.4

* Bug fix in map_files_texmf.

=item 2007/09/06 v0.5

* Support for `configure' added.
  (Thanks to Norbert Preining for writing a first version of
  the configure stuff.)

=item 2008/04/05 v0.6

* Tiny fix in target `uninstall' in file `Makefile.in'.
  (Thanks to Karl Berry)

=back

=cut
