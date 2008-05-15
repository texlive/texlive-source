# source this file; set up for tests

# Skip this test if the shell lacks support for functions.
unset function_test
eval 'function_test() { return 11; }; function_test'
if test $? != 11; then
  echo "$0: /bin/sh lacks support for functions; skipping this test." 1>&2
  (exit 77); exit 77
fi

skip_test_()
{
  echo "$0: skipping test: $@" 1>&2
  (exit 77); exit 77
}

uid_is_privileged_()
{
  # Make sure id -u succeeds.
  my_uid=`id -u` \
    || { echo "$0: cannot run \`id -u'" 1>&2; return 1; }

  # Make sure it gives valid output.
  case $my_uid in
    0) ;;
    *[!0-9]*)
      echo "$0: invalid output (\`$my_uid') from \`id -u'" 1>&2
      return 1 ;;
    *) return 1 ;;
  esac
}

skip_if_()
{
  case $1 in
    root) skip_test_ must be run as root ;;
    non-root) skip_test_ must be run as non-root ;;
    *) ;;  # FIXME?
  esac
}

require_selinux_()
{
  case `ls -Zd .` in
    '? .'|'unlabeled .')
      skip_test_ "this system (or maybe just" \
        "the current file system) lacks SELinux support"
    ;;
  esac
}

skip_if_root_() { uid_is_privileged_ && skip_test_ "must be run as non-root"; }
error_() { echo "$0: $@" 1>&2; (exit 1); exit 1; }
framework_failure() { error_ 'failure in testing framework'; }

test_dir_=`pwd`

this_test_() { echo "./$0" | sed 's,.*/,,'; }
this_test=`this_test_`

# This is a stub function that is run upon trap (upon regular exit and
# interrupt).  Override it with a per-test function, e.g., to unmount
# a partition, or to undo any other global state changes.
cleanup_() { :; }

t_=`$abs_top_srcdir/tests/mkdtemp $test_dir_ lzma-$this_test.XXXXXXXXXX` \
    || error_ "failed to create temporary directory in $test_dir_"

# Run each test from within a temporary sub-directory named after the
# test itself, and arrange to remove it upon exception or normal exit.
trap 'st=$?; cleanup_; d='"$t_"';
    cd '"$test_dir_"' && chmod -R u+rwx "$d" && rm -rf "$d" && exit $st' 0
trap '(exit $?); exit $?' 1 2 13 15

cd $t_ || error_ "failed to cd to $t_"

if ( diff --version < /dev/null 2>&1 | grep GNU ) 2>&1 > /dev/null; then
  compare() { diff -u "$@"; }
elif ( cmp --version < /dev/null 2>&1 | grep GNU ) 2>&1 > /dev/null; then
  compare() { cmp -s "$@"; }
else
  compare() { cmp "$@"; }
fi

# Local Variables:
# indent-tabs-mode: nil
# End:
