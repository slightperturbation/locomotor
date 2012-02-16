# usage:
#  locomotor$ scripts/configure_and_build.sh
#  locomotor$ make -f Makefile_evolverDB

#qmake -makefile -spec macx-llvm  -o Makefile_evolverDB evolverDB.pro
qmake -makefile -spec macx-g++42  -o Makefile_evolverDB evolverDB.pro
echo "Now run make -f Makefile_evolverDB to build.  Result is bin/evolverDB"