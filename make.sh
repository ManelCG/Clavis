#! /bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR

if [ "$1" == "clean" ]; then
  echo "Cleaning $PWD/out"
  rm -rf out
  exit
fi

if [ "$1" == "deploymingw" ]; then
  pacman -Syu --needed \
    git                                       \
    base-devel                                \
    mingw-w64-x86_64-toolchain                \
    mingw-w64-x86_64-cmake                    \
    mingw-w64-x86_64-gtkmm4                   \
    mingw-w64-x86_64-gpgme
  exit
fi

clear

cmake -B out -G "Unix Makefiles"

if [ "$?" != "0" ]; then
  echo ERROR!
  exit
fi

cd out

make -j$(nproc)
if [ "$?" != "0" ]; then
  echo ERROR!
  exit
fi

if [ "$1" == run ]; then
  ./Clavis
fi

