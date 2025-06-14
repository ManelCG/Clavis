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

if [ "$1" == "uninstall" ]; then
  sudo rm /usr/bin/clavis
  sudo rm /usr/share/applications/clavis.desktop
  sudo rm /usr/share/pixmaps/clavis.png
  sudo rm /usr/share/icons/hicolor/256x256/apps/clavis.png
  sudo rm /usr/share/icons/hicolor/128x128/apps/clavis.png
  sudo rm /usr/share/icons/hicolor/64x64/apps/clavis.png
  sudo rm /usr/share/icons/hicolor/32x32/apps/clavis.png
  sudo rm /usr/share/icons/hicolor/16x16/apps/clavis.png
  sudo rm -rf /usr/lib/clavis
  exit
fi

clear

cmake -B out -G "Unix Makefiles"
ERROR=$?
if [ "$ERROR" != "0" ]; then
  echo ERROR ${ERROR}!
  exit ${ERROR}
fi

cd out

make -j$(nproc)
ERROR=$?
if [ "$ERROR" != "0" ]; then
  echo ERROR ${ERROR}!
  exit ${ERROR}
fi

if [ "$1" == run ]; then
  ./Clavis
elif [ "$1" == "install" ]; then
  sudo make install
elif [ "$2" == "archlinux" ]; then
  make archlinux
fi

