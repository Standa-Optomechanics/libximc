#!/bin/sh
set -e
if [ "$1" = "clean" ] ; then
    echo Cleaning xigen...
    [ -d "$DEPSDIR/xigen" ] && rm -rf "$DEPSDIR/xigen"
    [ -d "$DEPSDIR/xigen" ] && rm -rf "$DEPSDIR/xigen"
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo '                    BUILDING XIGEN                  '
echo '                                                    '
echo '----------------------------------------------------'

XIGENVERTAG=v1.1.0
echo xigen version:     $XIGENVERTAG

echo '----------------------------------------------------'
echo '              Checking dependencies...              '
echo '----------------------------------------------------'
echo No dependencies are needed!
echo '------- Dependencies checking has been done --------'

# Cleaning
rm -rf $DEPSDIR/xigen
mkdir -p $DEPSDIR
cd $DEPSDIR

# Downloading
git clone --branch $XIGENVERTAG "https://github.com/Standa-Optomechanics/xigen" --depth 1

# Building
echo "+++++++++++++ Building xigen..."
cd $DEPSDIR/xigen
cmake $DEPENDENCIES_CMAKE_OPTIONS . && $MAKE
cd $BASEDIR
echo '----------------------------------------------------'
echo '            BUILDING XIGEN IS COMPLETED             '
echo '----------------------------------------------------'
echo 'Output:                                             '
echo "* $DEPSDIR/xigen/src/xigen                          "
echo '----------------------------------------------------'
