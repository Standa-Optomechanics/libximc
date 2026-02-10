#!/bin/bash
set -e

# Determine library version
VER=$LIBRARY_VERSION
if [ -z "$VER" ] ; then
    echo "ERROR: Library version is not set!"
    exit 1
fi

BASEDIR=$(pwd)
DIST=dist/ximc-$VER
DISTLIB=$DIST/ximc
DISTEXAM=$DIST/examples

if [ -d "$DIST" ] ; then
    echo "Dist already exists"
    exit 1
fi
mkdir -p $DISTLIB $DISTEXAM # Automatically creates DIST

# OSX kludge...
COPYFILE_DISABLE=true
export COPYFILE_DISABLE

echo ++++++++++++++++
echo   Copying docs
echo ++++++++++++++++
cp -R ximc/doc-en   $DISTLIB/
cp -R ximc/doc-ru   $DISTLIB/
cp -R ximc/doc-java $DISTLIB/

echo ++++++++++++++++
echo  Copying driver
echo ++++++++++++++++
cp -R driver $DIST/

echo ++++++++++++++++++++++++++
echo   Packing Python wrapper
echo ++++++++++++++++++++++++++
PYTHON_BINROOT_DIR=$BASEDIR/wrappers/python
PYTHON_LIBFILESDIR=$BASEDIR/wrappers/python/src/libximc/library-files

ARCH_LIST="debian-amd64 macosx win32 win64" # TODO: Add debian-armhf arch!
for arch in $ARCH_LIST ; do
    LIBSDIR=$BASEDIR/ximc/$arch
    mkdir -p $PYTHON_LIBFILESDIR/$arch
    if [[ $arch == win* ]] ; then
        WILDCARDS="*.dll *.lib"
    elif [[ $arch == debian* ]]; then
        WILDCARDS="*.so *.so.*"
    elif [[ $arch == macosx ]] ; then
        WILDCARDS="libximc.framework"
    else
        echo "Unknown architecture: $arch"
        exit 1
    fi

    for WILDCARD in $WILDCARDS ; do
        # We need to dereference symlinks when copying libraries because of stupid shit called python build package.
        # It seems that rusty combination of very old python and build cannot handle symlinks properly. Shame! Shame!
        cp -v -rL $LIBSDIR/$WILDCARD $PYTHON_LIBFILESDIR/$arch/
    done
done

echo Copying prepared python wrapper...
mkdir -p $DISTLIB/crossplatform/wrappers/python
cp -Rv $PYTHON_BINROOT_DIR/src/libximc               $DISTLIB/crossplatform/wrappers/python/
cp -v $PYTHON_BINROOT_DIR/where-did-pyximc.py-go.txt   $DISTLIB/crossplatform/wrappers/python/

echo Packing python wrapper into a wheel
cd $PYTHON_BINROOT_DIR
sed -i -e "s/^version = \"X.X.X\"$/version = \"$VER\"/" pyproject.toml
# Temporarily move explaining pyximc.py substitution *.txt file out of building dir
mv where-did-pyximc.py-go.txt ..
echo Packing the binding to a wheel...
# TODO: remove me later
echo $(ls -lR)
echo $(python3 --version)
echo $(python3 -m build --version)
python3 -m build --skip-dependency-check
# Move explaining pyximc.py substitution *.txt file back
mv ../where-did-pyximc.py-go.txt .
cd -

echo ++++++++++++++++++++++
echo   Copying c-profiles
echo ++++++++++++++++++++++
if [ -d "ximc/../c-profiles" ] ; then
    cp -R ximc/../c-profiles $DISTLIB/
fi

echo +++++++++++++++++++++++
echo Copying python-profiles
echo +++++++++++++++++++++++
if [ -d "ximc/../python-profiles" ] ; then
    cp -R ximc/../python-profiles $DISTLIB/
fi

echo +++++++++++++++++++++
echo   Copying libraries  
echo +++++++++++++++++++++
echo Debian packages...
ARCH_LIST="amd64" # TODO: use more architectures (debian only! Remember!): "i386 armhf"
mkdir -p $DISTLIB/deb
cp -Rv ximc/deb/*.deb $DISTLIB/deb/

echo Debian libraries...
for arch in $ARCH_LIST ; do
    lib_dir=$(find ximc -name "debian-$arch")
    if [ -n "$lib_dir" ] ; then
        mkdir -p $DISTLIB/debian-$arch
        cp -Rv $lib_dir/* $DISTLIB/debian-$arch/
    else
        echo Warning: No corresponding files for architecture $arch found! Skipping...
    fi
done

echo OS X package...
mkdir -p $DISTLIB/macosx
cp -R ximc/macosx/libximc.framework $DISTLIB/macosx/
cp -R ximc/macosx/libjximc.jar      $DISTLIB/macosx/
cp -R ximc/macosx/libjximc*dylib    $DISTLIB/macosx/

echo Windows packages...
for arch in win32 win64 ; do
    mkdir -p $DISTLIB/$arch
    cp -R ximc/$arch/ximc.h         $DISTLIB    # Yes, ximc.h'll be overwritten by next iter-s. But it must be the same.
    cp -R ximc/$arch/libximc.*      $DISTLIB/$arch/
    cp -R ximc/$arch/bindy.dll      $DISTLIB/$arch/
    cp -R ximc/$arch/bindy.lib      $DISTLIB/$arch/
    cp -R ximc/$arch/xiwrapper.dll  $DISTLIB/$arch/
    cp -R ximc/$arch/libjximc.*     $DISTLIB/$arch/
done

echo ++++++++++++++++++++++
echo  Copying the wrappers
echo ++++++++++++++++++++++
echo C\# wrappers...
for arch in win32 win64 ; do
    mkdir -p $DISTLIB/$arch/wrappers/csharp
    cp ximc/$arch/wrappers/csharp/ximcnet.cs    $DISTLIB/$arch/wrappers/csharp/
    cp ximc/$arch/wrappers/csharp/ximcnet.dll   $DISTLIB/$arch/wrappers/csharp/
done

echo MatLAB wrappers...
for arch in win32 win64 macosx ; do
    mkdir -p $DISTLIB/$arch/wrappers/matlab
    cp ximc/win64/wrappers/matlab/ximcm.h $DISTLIB/$arch/wrappers/matlab/
done
cp -R ximc/win64/wrappers/matlab/*      $DISTLIB/win64/wrappers/matlab/

echo ++++++++++++++++++++++++++++
echo  Copying VC Redistributable
echo ++++++++++++++++++++++++++++
cp -R third-party/vcredist/vcredist_x86.exe $DISTLIB/win32/
cp -R third-party/vcredist/vcredist_x64.exe $DISTLIB/win64/

echo +++++++++++++++++++++
echo    Copying license   
echo +++++++++++++++++++++
cp COPYING $DISTLIB/LICENSE.txt

echo ++++++++++++++++++++++
echo    Copying examples   
echo ++++++++++++++++++++++
echo Copy examples sources...
for example in test_Python test_C test_CSharp test_VBNET test_MATLAB test_Java test_Python; do
    echo Copying example $example
    mkdir -p $DISTEXAM/$example
    cp -R examples/$example/* $DISTEXAM/$example/
done

# Dirtiest hack: remove useless files
# TODO: Make it better later
rm -f   $DISTEXAM/test_Java/MANIFEST.MF
rm -f   $DISTEXAM/Makefile
rm -rf  $DISTEXAM/ru


# There is no python compiled examples, so we copy nothing. Instead just clear a little
rm -f $DISTEXAM/test_Python/Makefile

echo ++++++++++++++++++++++
echo  Copying the sources
echo ++++++++++++++++++++++
git clone . dist/ximc-$VER-src

echo ++++++++++++++++++++++
echo  tarballing artifacts
echo ++++++++++++++++++++++
tar -C dist -czf dist/libximc-$VER-all.tar.gz       ximc-$VER
tar -C dist -czf dist/libximc_src-$VER-all.tar.gz   ximc-$VER-src
cp dist/ximc-$VER-src/ChangeLog dist/libximc-$VER-changelog.txt
cd wrappers/python/dist/ ; 7z a ../../../dist/libximc_bindings_python-$VER.7z * ; cd -

unset COPYFILE_DISABLE