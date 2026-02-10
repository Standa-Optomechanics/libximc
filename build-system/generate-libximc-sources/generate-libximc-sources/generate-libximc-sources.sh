#!/bin/sh
set -e
if [ "$1" = "clean" ] ; then
    git --version 1>/dev/null 2>/dev/null
    if [ ! $? -eq 0 ] ; then
        echo "WARNING: git cannot be found! Cleaning won't be done"
    else
        git clean -xdf $BASEDIR/libximc
    fi
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo "            GENERATING LIBXIMC's SOURCES            "
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
    XIGEN_SRC_DIR="$DEPSDIR/xigen/src"
fi
set -e
echo '------- Dependencies checking has been done --------'

echo "++++++++++ Generating libximc's sources..."
$XIGEN --gen-header          -x "$XIMC_VERSION_FILE" -i "$LIBXIMC_SRC_DIR/protocol.xi" -o "$LIBXIMC_INC_DIR/ximc.h"       -t "$LIBXIMC_SRC_DIR/ximc-template.h"
$XIGEN --gen-internal-header -x "$XIMC_VERSION_FILE" -i "$LIBXIMC_SRC_DIR/protocol.xi" -o "$LIBXIMC_SRC_DIR/ximc-gen.h"   -t "$LIBXIMC_SRC_DIR/ximc-gen-template.h"
$XIGEN --gen-code            -x "$XIMC_VERSION_FILE" -i "$LIBXIMC_SRC_DIR/protocol.xi" -o "$LIBXIMC_SRC_DIR/ximc-gen.c"   -t "$LIBXIMC_SRC_DIR/ximc-gen-template.c"
$XIGEN --gen-def             -x "$XIMC_VERSION_FILE" -i "$LIBXIMC_SRC_DIR/protocol.xi" -o "$LIBXIMC_SRC_DIR/libximc.def"  -t "$LIBXIMC_SRC_DIR/libximc-template.def"
$XIGEN --gen-fw-header       -x "$XIMC_VERSION_FILE" -i "$LIBXIMC_SRC_DIR/protocol.xi" -o "$LIBXIMC_SRC_DIR/fwprotocol.h" -t "$XIGEN_SRC_DIR/fwprotocol-template.h"
$XIGEN --gen-fw-lib          -x "$XIMC_VERSION_FILE" -i "$LIBXIMC_SRC_DIR/protocol.xi" -o "$LIBXIMC_SRC_DIR/fwprotocol.c" -t "$XIGEN_SRC_DIR/fwprotocol-template.c"
$XIGEN --gen-jni             -x "$XIMC_VERSION_FILE" -i "$LIBXIMC_SRC_DIR/protocol.xi" -o "$LIBXIMC_JAVA_DIR/c/ru_ximc_libximc_JXimc-gen.c" -t "$LIBXIMC_JAVA_DIR/c/ru_ximc_libximc_JXimc-template.c"
$XIGEN --gen-java            -x "$XIMC_VERSION_FILE" -i "$LIBXIMC_SRC_DIR/protocol.xi" -o "$LIBXIMC_JAVA_DIR/java/ru/ximc/libximc/JXimc.java" -t "$LIBXIMC_JAVA_DIR/java/ru/ximc/libximc/JXimc-template.java"
echo '----------------------------------------------------'
echo "      GENERATING LIBXIMC's SOURCES IS COMPLETED     "
echo '----------------------------------------------------'
echo 'Output:                                             '
echo "* $LIBXIMC_INC_DIR/ximc.h                           "
echo "* $LIBXIMC_SRC_DIR/ximc-gen.h                       "
echo "* $LIBXIMC_SRC_DIR/ximc-gen.c                       "
echo "* $LIBXIMC_SRC_DIR/libximc.def                      "
echo "* $LIBXIMC_SRC_DIR/fwprotocol.h                     "
echo "* $LIBXIMC_SRC_DIR/fwprotocol.c                     "
echo "* $LIBXIMC_JAVA_DIR/c/ru_ximc_libximc_JXimc-gen.c   "
echo "* $LIBXIMC_JAVA_DIR/java/ru/ximc/libximc/JXimc.java "
echo '----------------------------------------------------'
