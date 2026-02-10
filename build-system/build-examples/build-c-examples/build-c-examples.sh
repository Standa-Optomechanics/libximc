#!/bin/sh
set -e

NAME="test_C"

if [ "$1" = "clean" ] ; then
    set +e
    echo Cleaning $NAME examples...
    git --version 1>/dev/null 2>/dev/null
    if [ ! $? -eq 0 ] ; then
        echo "WARNING: git cannot be found! Cleaning won't be complete."
    else
        git clean -xdf "$BASEDIR/examples/$NAME"
    fi
    rm -rf "$EXAMPLES_DIST_DIR/$NAME"
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo '               BUILDING C examples...               '
echo '                                                    '
echo '----------------------------------------------------'

echo '----------------------------------------------------'
echo '              Checking dependencies...              '
echo '----------------------------------------------------'
echo Checking libximc framework...
if [ -e "$LIBSDIR/libximc.framework" ]; then
    echo "libximc.framework is found."
else
    echo "libximc.framework is not found. Please build libximc first."
    echo "Searching path is $LIBSDIR"
    exit 1
fi
echo '***** Done!'
echo '------- Dependencies checking has been done --------'

EXAMP_LIST="testapp_C testappeasy_C testprofile_C"

for exam in $EXAMP_LIST; do
	cd $EXAMPLES_SRC_DIR/$NAME/$exam && xcodebuild LIBXIMC_LIB_PATH=$LIBSDIR CODE_SIGNING_REQUIRED=NO CODE_SIGN_IDENTITY=""
done

echo Copying the result to output directory...
for exam in $EXAMP_LIST; do
    mkdir -p $EXAMPLES_DIST_DIR/$NAME/$exam
    cp -a $EXAMPLES_SRC_DIR/$NAME/$exam/build/Release/$exam.app $EXAMPLES_DIST_DIR/$NAME/$exam
done
echo '---------------------------------------------------'
echo '          BUILDING C EXAMPLES IS COMPLETE          '
echo '---------------------------------------------------'
echo 'Output:                                            '
for exam in $EXAMP_LIST; do
echo "* $EXAMPLES_DIST_DIR/$NAME/$exam/$exam.app        "
done
echo '---------------------------------------------------'
