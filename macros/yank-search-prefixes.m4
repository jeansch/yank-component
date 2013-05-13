dnl
dnl YANK_SEARCH_PREFIXES(arg-with-name, arg-with-help, default-path, checking-message, tests, error-message, dest-variable)
dnl
dnl Checks a series of prefixes that satisfy some condition
dnl
dnl The series of prefixes is derived from two places: <default-path>, a space-separated list of prefixes
dnl (which naturally breaks on paths with spaces in them), and a --with option, which is specified by
dnl <arg-with-name> and <arg-with-help>, the first two arguments to AC_ARG_WITH. <checking-message> is
dnl the message that is passed to AC_MSG_CHECKING. 
dnl
dnl <tests> is a bit of shell code that checks to see whether the prefix is the desired one. $testpfx is
dnl a shell variable with the prefix, and $pfxok should be set to "yes" or "no" to specify whether the
dnl prefix is the desired one.
dnl
dnl <error-message> is passed to AC_MSG_ERROR if none of the provided directories are satisfactory. A
dnl message is appended telling about the --with option.
dnl
dnl If a satisfying prefix is found, <dest-variable> is set to it and AC_SUBST'd
dnl

#serial 1
AC_DEFUN([YANK_SEARCH_PREFIXES],[
	$7=none
	pfxlist="$3"

	AC_MSG_CHECKING([$4])

	AC_ARG_WITH([$1],[$2],[
		pfxok=no
		testpfx="$withval"

		dnl run tests
		$5

		dnl ok?
		if test x"$pfxok" = xyes ; then
			AC_MSG_RESULT([$withval])
			$7="$withval"
		else
			AC_MSG_ERROR([
\"$withval\" is not a valid parameter to --with-$1. Make sure you have specified 
the right argument, or let configure guess the value for you.
])
		fi
	],[
		dnl loop through space-separated prefixes
		for testpfx in $pfxlist ; do
			pfxok=no #assume it doesn't match

			dnl run tests
			$5

			dnl was this it?
			if test x"$pfxok" = xyes ; then
				AC_MSG_RESULT([$testpfx])
				$7="$testpfx"
				break
			fi
		done
	])

	dnl didn't find it?
	if test x"$7" = xnone ; then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([$6

Try using --with-$1=/prefix if the directory is not in one of \"$3\".
])
	else
		dnl AC_SUBST expands to nothing
		:
		AC_SUBST([$7])
	fi
])
