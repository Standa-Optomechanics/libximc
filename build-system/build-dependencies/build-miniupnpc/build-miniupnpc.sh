#!/bin/sh
set -e
if [ "$1" = "clean" ] ; then
    echo Cleaning miniupnpc...
    [ -d "$DEPSDIR/miniupnpc" ] && rm -rf "$DEPSDIR/miniupnpc"
    [ -d "$DEPSDIR/miniupnpc" ] && rm -rf "$DEPSDIR/miniupnpc"
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo '                 BUILDING MINIUPNPC                 '
echo '                                                    '
echo '----------------------------------------------------'

MINIUPNPCVER="8ddbb71"
echo "miniupnpc version: $MINIUPNPCVER"

echo '----------------------------------------------------'
echo '              Checking dependencies...              '
echo '----------------------------------------------------'
echo No dependencies are needed!
echo '------- Dependencies checking has been done --------'

URL_MINIUPNPC="https://github.com/EPC-MSU/miniupnpc.git"
echo "+++++++++++++ Building miniupnpc..."
[ -d "$DEPSDIR/miniupnpc" ] && rm -rf "$DEPSDIR/miniupnpc"
git clone --recursive $URL_MINIUPNPC "$DEPSDIR/miniupnpc"
cd "$DEPSDIR/miniupnpc"
git checkout "$MINIUPNPCVER"
cmake $DEPS_CMAKE_OPT \
		-DUPNPC_BUILD_TESTS=OFF -DUPNPC_BUILD_SAMPLE=OFF -DUPNPC_BUILD_SHARED=OFF \
        -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_PREFIX=$DEPSDIR/miniupnpc $* .
cmake $DEPENDENCIES_CMAKE_OPTIONS --build .
cmake --build . --target install
cd - >/dev/null
echo '----------------------------------------------------'
echo '          BUILDING MINIUPNPC IS COMPLETED           '
echo '----------------------------------------------------'
echo 'Output:                                             '
echo "* $DEPSDIR/miniupnpc/lib/libminiupnpc.a             "
echo "* $DEPSDIR/miniupnpc/include/miniupnpc/             "
echo "* $DEPSDIR/miniupnpc                                "
echo '----------------------------------------------------'
