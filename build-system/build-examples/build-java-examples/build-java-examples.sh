#!/bin/sh
set -e

NAME="test_Java"

if [ "$1" = "clean" ] ; then
    set +e
    echo Cleaning $NAME examples...
    git --version 1>/dev/null 2>/dev/null
    if [ ! $? -eq 0 ] ; then
        echo "WARNING: git cannot be found! Cleaning won't be complete."
    else
        git clean -xdf "$EXAMPLES_SRC_DIR/$NAME"
    fi
    rm -rf "$EXAMPLES_DIST_DIR/$NAME"
    exit 0
fi

echo '-----------------------------------------------------'
echo '                                                     '
echo '              BUILDING Java examples...              '
echo '                                                     '
echo '-----------------------------------------------------'

echo '-----------------------------------------------------'
echo '              Checking dependencies...               '
echo '-----------------------------------------------------'
echo Checking for make...
if [ -n "$MAKE" ]; then
    echo "make is found: $MAKE"
else
    echo "make is not found. Ensure it's installed and available in PATH."
    exit 1
fi
echo '***** Done!'

echo Checking for libjximc.jar...
if [ -e "$LIBSDIR/libjximc.jar" ]; then
    echo "libjximc.jar is found."
else
    echo "libjximc.jar is not found. Build java wrapper first."
    exit 1
fi
echo '***** Done!'
echo '------- Dependencies checking has been done --------'

cd $EXAMPLES_SRC_DIR/$NAME && $MAKE

echo Copying the result to output directory...
mkdir -p $EXAMPLES_DIST_DIR/$NAME
cp -a $EXAMPLES_SRC_DIR/$NAME/test_Java.jar $EXAMPLES_DIST_DIR/$NAME
cp -a $EXAMPLES_SRC_DIR/$NAME/README.txt    $EXAMPLES_DIST_DIR/$NAME/java-README.txt
echo '---------------------------------------------------'
echo '          BUILDING C EXAMPLES IS COMPLETE          '
echo '---------------------------------------------------'
echo 'Output:                                            '
echo "* $EXAMPLES_DIST_DIR/$NAME/test_Java.jar           "
echo "* $EXAMPLES_DIST_DIR/$NAME/java-README.txt         "
echo '---------------------------------------------------'
