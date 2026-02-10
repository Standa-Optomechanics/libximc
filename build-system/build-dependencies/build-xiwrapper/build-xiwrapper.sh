#!/bin/sh
set -e
if [ "$1" = "clean" ] ; then
    echo Cleaning miniupnpc...
    rm -rf "$DEPSDIR/xiwrapper"
    rm -f  "$LIBSDIR/libxiwrapper.so"
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo '                 BUILDING XIWRAPPER                 '
echo '                                                    '
echo '----------------------------------------------------'

XIWRAPPERVER=$( sed '4q;d' version )
[ -z "$XIWRAPPERVER" ] && XIWRAPPERVER=be6508e
echo xiwrapper version: $XIWRAPPERVER

echo '----------------------------------------------------'
echo '              Checking dependencies...              '
echo '----------------------------------------------------'
echo No dependencies are needed!
echo '------- Dependencies checking has been done --------'

# Cleaning
rm -rf "$DEPSDIR/xiwrapper"
mkdir -p $DEPSDIR
cd $DEPSDIR

# Downloading
git clone "https://github.com/Standa-Optomechanics/libxiwrapper" "xiwrapper"
cd xiwrapper
git checkout $XIWRAPPERVER

# Building
echo "+++++++++++++ Building xiwrapper..."
# I don't understand why do we need NO_RPATH_PACKAGE for xiwrapper. So, maybe it may be removed.
cmake -DBINDY_PATH=$DEPSDIR/bindy $DEPENDENCIES_CMAKE_OPTIONS -DNO_RPATH_PACKAGING=TRUE .
cmake --build .
# Required to have libbindy.so aside with libxiwrapper, at least for debian packaging
cp -av $DEPSDIR/bindy/libbindy.* .

cd $BASEDIR

if [ "$OS_NAME" = "Darwin" ] ; then
    echo "No copying will be performed for Darwin OS. All necessary actions will be done by autotools scripts."
    echo '----------------------------------------------------'
    echo '          BUILDING XIWRAPPER IS COMPLETED           '
    echo '----------------------------------------------------'
    echo '----------------------------------------------------'
    exit 0
else
    echo Copying the result to output directory...
    mkdir -p                                     "$LIBSDIR"
    cp "$BASEDIR/deps/xiwrapper/libxiwrapper.so" "$LIBSDIR"
    echo '----------------------------------------------------'
    echo '          BUILDING XIWRAPPER IS COMPLETED           '
    echo '----------------------------------------------------'
    echo 'Output:                                             '
    echo "* $LIBSDIR/libxiwrapper.so                          "
    echo '----------------------------------------------------'
fi
