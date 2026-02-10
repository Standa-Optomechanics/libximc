#!/bin/sh
set -e
if [ "$1" = "clean" ] ; then
    set +e
    echo Cleaning java wrapper...
    git --version 1>/dev/null 2>/dev/null
    if [ ! $? -eq 0 ] ; then
        echo "WARNING: git cannot be found! Cleaning won't be complete."
    else
        git clean -xdf "$BASEDIR/wrappers/java"
    fi
    rm -f "$LIBSDIR/libjximc.so"
    rm -f "$LIBSDIR/libjximc.so.7"
    rm -f "$LIBSDIR/libjximc.so.7.0.0"
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo '               BUILDING JAVA WRAPPER                '
echo '                                                    '
echo '----------------------------------------------------'

echo '----------------------------------------------------'
echo '              Checking dependencies...              '
echo '----------------------------------------------------'
echo Checking xigen...
set +e
if [ ! -e $DEPSDIR/xigen/src/xigen ] ; then
    echo ERROR: xigen for win32 cannot be found!
    exit 1
else
    echo '***** Done!'
    XIGEN="$DEPSDIR/xigen/src/xigen"
fi
set -e
echo '------- Dependencies checking has been done --------'

cd "$BASEDIR/wrappers/java"
$MAKE
cd -

if [ "$OS_NAME" = "Darwin" ] ; then
    echo "Darwin OS is the devil's trap. Here we need to build .framework file."
    make -C wrappers/java/src/c framework-build
    echo "Darwin - the devil's OS, remember? So we won't perform copying of resultant files here. Instead we will pray"
    echo "that autotools scripts do their job well and copy files instead of us."
    echo "This thing doesn't copy jar file! stupid piece of software!"
    mkdir -p "$LIBSDIR"
    cp -R "$DISTDIR/local/share/java/libjximc.jar" "$LIBSDIR/"
    echo '----------------------------------------------------'
    echo '          BUILDING JAVA WRAPPER IS COMPLETE         '
    echo '----------------------------------------------------'
    echo 'Output:                                             '
    echo "* $LIBSDIR/libjximc.jar                             "
    echo '----------------------------------------------------'
    exit 0
else
    echo Copying the result to output directory...
    mkdir -p "$LIBSDIR"
    mkdir -p "$DISTDIR/ximc/doc-java/html"
    cp "$BASEDIR/wrappers/java/src/c/.libs/libjximc.so.7.0.0" "$LIBSDIR"
    ln -sf "libjximc.so.7.0.0" "$LIBSDIR/libjximc.so.7"
    ln -sf "libjximc.so.7.0.0" "$LIBSDIR/libjximc.so"
    cp -r "$BASEDIR/wrappers/java/doc-java" "$DISTDIR/ximc/"
    echo '----------------------------------------------------'
    echo '          BUILDING JAVA WRAPPER IS COMPLETE         '
    echo '----------------------------------------------------'
    echo 'Output:                                             '
    echo "* $LIBSDIR/libjximc.so                              "
    echo "* $LIBSDIR/libjximc.so.7                            "
    echo "* $LIBSDIR/libjximc.so.7.0.0                        "
    echo "* $DISTDIR/ximc/doc-java                            "
    echo '----------------------------------------------------'
fi
