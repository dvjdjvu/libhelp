DEB=deb
DEBIAN=$DEB/DEBIAN
BUILD_DIR=.

PATH_LIB=$DEB/usr/local/lib
PATH_INCLUDE=$DEB/usr/local/include/help
PATH_BIN=$DEB/usr/local/bin

chmod 755 $DEB/*
chmod 755 $DEBIAN/*

mkdir -p $PATH_LIB
mkdir -p $PATH_INCLUDE
mkdir -p $PATH_BIN

strip --strip-unneeded objs/*.so*

cp -r objs/*.so* $PATH_LIB
cp -r src/*.h $PATH_INCLUDE
cp -r objs/lockrun $PATH_BIN

dpkg-deb --build $DEB $BUILD_DIR


