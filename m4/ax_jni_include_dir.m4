# ===========================================================================
#    http://www.gnu.org/software/autoconf-archive/ax_jni_include_dir.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_JNI_INCLUDE_DIR
#
# DESCRIPTION
#
#   AX_JNI_INCLUDE_DIR finds include directories needed for compiling
#   programs using the JNI interface.
#
#   JNI include directories are usually in the Java distribution. This is
#   deduced from the value of $JAVA_HOME, $JAVAC, or the path to "javac", in
#   that order. When this macro completes, a list of directories is left in
#   the variable JNI_INCLUDE_DIRS.
#
#   Example usage follows:
#
#     AX_JNI_INCLUDE_DIR
#
#     for JNI_INCLUDE_DIR in $JNI_INCLUDE_DIRS
#     do
#             CPPFLAGS="$CPPFLAGS -I$JNI_INCLUDE_DIR"
#     done
#
#   If you want to force a specific compiler:
#
#   - at the configure.in level, set JAVAC=yourcompiler before calling
#   AX_JNI_INCLUDE_DIR
#
#   - at the configure level, setenv JAVAC
#
#   Note: This macro can work with the autoconf M4 macros for Java programs.
#   This particular macro is not part of the original set of macros.
#
# LICENSE
#
#   Copyright (c) 2008 Don Anderson <dda@sleepycat.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AU_ALIAS([AC_JNI_INCLUDE_DIR], [AX_JNI_INCLUDE_DIR])
AC_DEFUN([AX_JNI_INCLUDE_DIR],[

JNI_INCLUDE_DIRS=""

if test "x$JAVA_HOME" != x; then
    _JTOPDIR="$JAVA_HOME"
else
    if test "x$JAVAC" = x; then
        JAVAC=javac
    fi
    AC_PATH_PROG([_ACJNI_JAVAC], [$JAVAC], [no])
    if test "x$_ACJNI_JAVAC" = xno; then
        AC_MSG_ERROR([cannot find JDK; try setting \$JAVAC or \$JAVA_HOME])
    fi
    _ACJNI_FOLLOW_SYMLINKS("$_ACJNI_JAVAC")
    _JTOPDIR=`echo "$_ACJNI_FOLLOWED" | sed -e 's://*:/:g' -e 's:/[[^/]]*$::'`
fi

_JINC="$_JTOPDIR/include"
_AS_ECHO_LOG([_JTOPDIR=$_JTOPDIR])
_AS_ECHO_LOG([_JINC=$_JINC])

# On Mac OS X 10.6.4, jni.h is a symlink:
# /System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers/jni.h
# -> ../../CurrentJDK/Headers/jni.h.

# get the likely subdirectories for system specific java includes
case "$host_os" in
bsdi*)          _JNI_INC_SUBDIRS="bsdos";;
freebsd*)       _JNI_INC_SUBDIRS="freebsd";;
linux*)         _JNI_INC_SUBDIRS="linux genunix";;
osf*)           _JNI_INC_SUBDIRS="alpha";;
solaris*)       _JNI_INC_SUBDIRS="solaris";;
mingw*)         _JNI_INC_SUBDIRS="win32";;
cygwin*)        _JNI_INC_SUBDIRS="win32";;
darwin*)        _JNI_INC_SUBDIRS="darwin";;
*)              _JNI_INC_SUBDIRS="genunix";;
esac

_ACJNI_CHECK_HEADER([$_JINC],
    [JNI_INCLUDE_DIRS="$ac_acjni_check_header"],
    [
        _JSEARCHDIR2=`echo "$_JTOPDIR" | sed -e 's:/[[^/]]*$::'`/include
        _ACJNI_CHECK_HEADER([$_JSEARCHDIR2],
            [JNI_INCLUDE_DIRS="$ac_acjni_check_header"],
            [AC_MSG_ERROR([cannot find JDK header files])]
        )
    ]
)

])

# _ACJNI_CHECK_HEADER(PATH,[ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ----------------------------------------
# Tries to find compilable jni.h on <path>.
# Sets output variable ac_acjni_check_header
AC_DEFUN([_ACJNI_CHECK_HEADER], [
        _JSEARCHDIR="$1"
        _AS_ECHO_LOG(Searching in $_JSEARCHDIR)

    AC_CHECK_HEADER([$_JSEARCHDIR/jni.h],
        [
            ac_acjni_check_header="$_JSEARCHDIR"
            $2
        ],
        [
            # try to find jni_md.h in one of platform subdirectories
            # reset checking flag, make temporary flags with platform includes
            cache_var=AS_TR_SH([ac_cv_header_$_JSEARCHDIR/jni.h])
            AS_UNSET($cache_var)
            CPPFLAGS_SAVE="$CPPFLAGS"
            for JINCSUBDIR in $_JNI_INC_SUBDIRS ; do
                if test -d "$_JSEARCHDIR/$JINCSUBDIR"; then
                     CPPFLAGS="$CPPFLAGS -I$_JSEARCHDIR/$JINCSUBDIR"
                fi
            done
            _AS_ECHO_LOG([Using CPPFLAGS for jni check $CPPFLAGS])
            AC_CHECK_HEADER([$_JSEARCHDIR/jni.h],
                [
                    # save the main include directory and platform subdirectories
                    ac_acjni_check_header="$_JSEARCHDIR"
                    for JINCSUBDIR in $_JNI_INC_SUBDIRS ; do
                        if test -d "$_JSEARCHDIR/$JINCSUBDIR"; then
                          ac_acjni_check_header="$ac_acjni_check_header $_JSEARCHDIR/$JINCSUBDIR"
                      fi
                    done
                    CPPFLAGS="$CPPFLAGS_SAVE"
                    $2
                ],
                [
                    CPPFLAGS="$CPPFLAGS_SAVE"
                    $3
                ]
            )
      ])
])# _ACJNI_CHECK_HEADER

# _ACJNI_FOLLOW_SYMLINKS <path>
# Follows symbolic links on <path>,
# finally setting variable _ACJNI_FOLLOWED
# ----------------------------------------
AC_DEFUN([_ACJNI_FOLLOW_SYMLINKS],[
# find the include directory relative to the javac executable
_cur="$1"
while ls -ld "$_cur" 2>/dev/null | grep " -> " >/dev/null; do
        AC_MSG_CHECKING([symlink for $_cur])
        _slink=`ls -ld "$_cur" | sed 's/.* -> //'`
        case "$_slink" in
        /*) _cur="$_slink";;
        # 'X' avoids triggering unwanted echo options.
        *) _cur=`echo "X$_cur" | sed -e 's/^X//' -e 's:[[^/]]*$::'`"$_slink";;
        esac
        AC_MSG_RESULT([$_cur])
done
_ACJNI_FOLLOWED="$_cur"
])# _ACJNI
