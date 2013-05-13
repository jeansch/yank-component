dnl
dnl YANK_GNOME_COMPONENT(component-name, version-string, display-name, error-if-not-found)
dnl
dnl Checks to see if the GNOME component <component-name> is installed, using
dnl gnome-config. Checks that component has a version at least <version-string>.
dnl The version is evaluated with awk, evaluating four terms of the version number,
dnl each being multiplied by (100 ^ (version-position - 4)). So something that
dnl reports version 1.2 has a value of 102000, 1.2.3 is 1020300, and 1.2.3.4 is
dnl 1020304. 7.99.4.17 is 7990417. (NB: obviously, this doesn't cope with versions
dnl being larger than 99). Anyway, this evaluation is compared with the number
dnl generated in the same manner by <version-at-least>.
dnl
dnl The name of the package shown to the user is <display-name>.
dnl
dnl The version reported by gnome-config has leading nonnumeric characters stripped.
dnl The versioning can be ignored by setting the environment variable 
dnl <component-name-in-capital-letters>_VERSION_OVERRIDE to non-empty.
dnl
dnl If <error-if-not-found> is equal to "FAIL", then AC_MSG_ERROR will be called if
dnl the component is not found.
dnl
dnl If the component is successfully discovered, the environment variable 
dnl YANK_FOUND_COMPONENTS will have " <component-name>" appended to it, or if unset,
dnl it will be set to <component-name>.
dnl

#serial 2
AC_DEFUN([YANK_GNOME_COMPONENT],[
	AC_MSG_CHECKING([for $3 >= $2])

	cap_name=`echo $1 |tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' |sed -e 's,-,_,g'`
	envar="${cap_name}_VERSION_OVERRIDE"
	found=unset

	if test x`eval echo \\$${envar}` != x ; then
		AC_MSG_RESULT([overriding to yes])
		found=yes
	else
		if gnome-config --libs $1 >/dev/null 2>&1 ; then
			dnl Quote the arguments for M4
			desired=`echo $2 |awk 'BEGIN { FS = "."; } { print [$]1 * 1000000 + [$]2 * 10000 + [$]3 * 100 + [$]4; }'`
			#str=`gnome-config --modversion $1 |sed -e "s,$1-,," -e 's,cvs$,,' -e 's,pre$,,'`
			str=`gnome-config --modversion $1 |sed -e 's,^[[^0-9]]*,,'`
			vers=`echo $str |awk 'BEGIN { FS = "."; } { print [$]1 * 1000000 + [$]2 * 10000 + [$]3 * 100 + [$]4; }'`

			if test "$vers" -ge "$desired" ; then
				found=yes
				AC_MSG_RESULT([yes, found $str])
			else
				AC_MSG_RESULT([too old, found $str])
				found=no
				error="You appear to have $3, version $str, installed. You need version $2 or greater."
			fi
		else
			AC_MSG_RESULT([not installed])
			found=no
			error="You either do not have $3, version $2 or greater, installed, or you need to install its development package."
		fi
	fi

	if test x"$found" = xno ; then
		if test x"$4" = xFAIL ; then
			AC_MSG_ERROR([$error])
		fi
	else
		if test x"$YANK_FOUND_COMPONENTS" = x ; then
			YANK_FOUND_COMPONENTS="$1"
		else
			YANK_FOUND_COMPONENTS="$YANK_FOUND_COMPONENTS $1"
		fi
	fi
])

dnl
dnl YANK_GNOME_FLAGS()
dnl
dnl Calls gnome-config --libs $YANK_FOUND_COMPONENTS and
dnl gnome-config --cflags $YANK_FOUND_COMPONENTS and stores the
dnl results into GNOME_COMPONENT_LIBS and GNOME_COMPONENT_CFLAGS
dnl and AC_SUBSTS those variables.

#serial 1
AC_DEFUN([YANK_GNOME_FLAGS],[
	GNOME_COMPONENT_LIBS="`gnome-config --libs $YANK_FOUND_COMPONENTS`"
	GNOME_COMPONENT_CFLAGS="`gnome-config --cflags $YANK_FOUND_COMPONENTS`"
	AC_SUBST(GNOME_COMPONENT_LIBS)
	AC_SUBST(GNOME_COMPONENT_CFLAGS)
])

dnl
dnl YANK_GNOME_SOME_FLAGS(<components>, <variable-root>)
dnl
dnl Calls gnome-config --libs <components> and
dnl gnome-config --cflags <component> and stores the
dnl results into <variable-root>_LIBS and <variable-root>_CFLAGS
dnl and AC_SUBSTS those variables. If a given component in
dnl <components> doesn't exist, it will be skipped. $YANK_FOUND_COMPONENTS
dnl is used to check this.

#serial 1
AC_DEFUN([YANK_GNOME_SOME_FLAGS],[
	use=

	dnl check for existing components
	for component in $1 ; do
		echo "$YANK_FOUND_COMPONENTS" |grep " $component " >/dev/null 2>&1 &&  use="$use $component" && continue
		echo "$YANK_FOUND_COMPONENTS" |grep "^$component " >/dev/null 2>&1 &&  use="$use $component" && continue 
		echo "$YANK_FOUND_COMPONENTS" |grep " $component\$" >/dev/null 2>&1 &&  use="$use $component" && continue 
		echo "$YANK_FOUND_COMPONENTS" |grep "^$component\$" >/dev/null 2>&1 &&  use="$use $component" && continue 
	done

	dnl slurp
	$2_LIBS="`gnome-config --libs $use`"
	$2_CFLAGS="`gnome-config --cflags $use`"
	AC_SUBST($2_LIBS)
	AC_SUBST($2_CFLAGS)
])

