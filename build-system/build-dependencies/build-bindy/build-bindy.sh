#!/bin/sh
set -e
if [ "$1" = "clean" ] ; then
    echo Cleaning bindy...
    rm -rf "$DEPSDIR/bindy"
    rm -f  "$LIBSDIR/libbindy.so"
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo '                    BUILDING BINDY                  '
echo '                                                    '
echo '----------------------------------------------------'

BINDYVER=$(sed '3q;d' version)
[ -z "$BINDYVER" ] && BINDYVER=master
echo Bindy version:     $BINDYVER

echo '----------------------------------------------------'
echo '              Checking dependencies...              '
echo '----------------------------------------------------'
echo No dependencies are needed!
echo '------- Dependencies checking has been done --------'

URL_BINDY="https://github.com/EPC-MSU/Bindy.git"
echo "+++++++++++++ Building bindy..."
[ -d "$DEPSDIR/bindy" ] && rm -rf "$DEPSDIR/bindy"
git clone --recursive $URL_BINDY "$DEPSDIR/bindy"
cd "$DEPSDIR/bindy"
git checkout "$BINDYVER"
cmake $DEPENDENCIES_CMAKE_OPTIONS . && $MAKE
cd - >/dev/null

if [ "$OS_NAME" = "Darwin" ] ; then
    echo "No copying will be performed for Darwin OS. All necessary actions will be done by xcode build process."
    echo ""
    echo '----------------------------------------------------'
    echo '            BUILDING BINDY IS COMPLETED             '
    echo '----------------------------------------------------'
    echo '----------------------------------------------------'
    exit 0
else
    echo Copying the result to output directory...
    mkdir -p "$LIBSDIR"
    cp "$BASEDIR/deps/bindy/libbindy.so" "$LIBSDIR"
    echo '----------------------------------------------------'
    echo '            BUILDING BINDY IS COMPLETED             '
    echo '----------------------------------------------------'
    echo 'Output:                                             '
    echo "* $LIBSDIR/libbindy.so                              "
    echo '----------------------------------------------------'
fi
