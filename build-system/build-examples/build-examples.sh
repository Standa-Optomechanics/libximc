#!/bin/sh
set -e
[ -z "$BASEDIR" ] && export BASEDIR="$(pwd)"
[ -z "$DEPSDIR" ] && export DEPSDIR="$BASEDIR/deps"
[ -z "$DISTDIR" ] && export DISTDIR="$BASEDIR/dist"
[ -z "$EXAMPLES_DIST_DIR" ] && export EXAMPLES_DIST_DIR="$DISTDIR/examples"
[ -z "$EXAMPLES_SRC_DIR" ]  && export  EXAMPLES_SRC_DIR="$BASEDIR/examples"
[ -z "$LIBSDIR" ]           && export LIBSDIR="$DISTDIR/ximc/macosx"

if [ "$(uname -s)" != "Darwin" ] ; then
    echo "This script is only for macOS."
    exit 1
fi

if [ "$1" = "clean" ] ; then
    echo "****************************************************"
    echo "*             CLEAN LIBXIMC'S EXAMPLES             *"
    echo "****************************************************"
    ./build-system/build-examples/build-c-examples/build-c-examples.sh          clean
    ./build-system/build-examples/build-java-examples/build-java-examples.sh    clean
    echo '****************************************************'
    echo '*                       DONE                       *'
    echo '****************************************************'
    exit 0
fi

case $1 in
    "")
        echo "***************************************************"
        echo "*                                                 *"
        echo "*           BUILDING LIBXIMC'S EXAMPLES           *"
        echo "*                                                 *"
        echo "***************************************************"

        ./build-system/build-examples/build-c-examples/build-c-examples.sh
        ./build-system/build-examples/build-java-examples/build-java-examples.sh

        echo "****************************************************"
        echo "*     BUILDING LIBXIMC'S EXAMPLES IS COMPLETED     *"
        echo "****************************************************"
        ;;
    "C")
        echo "Builds libximc's C examples.\n"
        ./build-system/build-examples/build-c-examples/build-c-examples.sh
        ;;
    "Java")
        echo "Builds libximc's Java examples.\n"
        ./build-system/build-examples/build-java-examples/build-java-examples.sh
        ;;
    "--help")
        echo "Builds libximc's examples.\n"
        echo "Usage: $0 [C|Java|clean]"
        echo "  C       - builds C examples"
        echo "  Java    - builds Java examples"
        echo "  clean   - cleans all examples"
        echo "  --help  - shows this help message"
        ;;
    *)
        echo "Unknown option \"$1\"."
        ;;
esac