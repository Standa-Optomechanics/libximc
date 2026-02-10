#!/bin/sh
set -e
if [ "$1" = "clean" ] ; then
    set +e
    echo Cleaning python wrapper...
    rm -rf "$DISTDIR/ximc/crossplatform/wrappers/python"
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo '                PACK PYTHON WRAPPER                 '
echo '                                                    '
echo '----------------------------------------------------'

echo '----------------------------------------------------'
echo '              Checking dependencies...              '
echo '----------------------------------------------------'
set +e
echo Checking libximc, libbindy and libxiwrapper...
case "$(uname -s)" in
    Darwin)
        # TODO: Add MacOS support
        ;;
    Linux|Debian)
        for file in libximc.a libximc.so libximc.so.7 libximc.so.7.0.0 libbindy.so libxiwrapper.so ; do
            [ ! -e "$LIBSDIR/$file" ] && echo "ERROR: $LIBSDIR/$file cannot be found!" && exit 1
            echo "$LIBSDIR/$file is found."
        done
        ;;
esac
echo "***** Done!"
set -e
echo '------- Dependencies checking has been done --------'
mkdir -p "$DISTDIR/ximc/crossplatform/wrappers/python/libximc"

# Copy the binding
cp -r "$BASEDIR/wrappers/python/src/libximc/"* "$DISTDIR/ximc/crossplatform/wrappers/python/libximc"

# Copy required libraries
OUTDIR=$DISTDIR/ximc/crossplatform/wrappers/python/libximc/library-files
case "$(uname -s)" in
    Darwin)
        # TODO: Add MacOS support
        ;;
    Linux|Debian)
        mkdir -p "$OUTDIR/debian-armhf"
        for file in libximc.a libximc.so libximc.so.7 libximc.so.7.0.0 libbindy.so libxiwrapper.so ; do
            cp "$LIBSDIR/$file" "$OUTDIR/debian-armhf/"
        done
        ;;
esac

# Copy where-did-pyximc.py-go.txt
cp "$BASEDIR/wrappers/python/where-did-pyximc.py-go.txt" "$DISTDIR/ximc/crossplatform/wrappers/python/"
echo '----------------------------------------------------'
echo '           PACK PYTHON WRAPPER IS COMPLETE          '
echo '----------------------------------------------------'
echo 'Output:                                             '
echo "* $DISTDIR/ximc/crossplatform/wrappers/python       "
echo '----------------------------------------------------'
