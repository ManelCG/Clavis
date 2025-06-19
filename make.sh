#! /bin/bash

Usage() {
  echo -e "Usage:"
  echo -e "$0 [arg]"
  echo -e "    help           -> Show this menu"
  echo -e "    clean          -> Clean all output to prepare for a clean build"
  echo -e "    deps [depname] -> Install the required dependencies for [depname]"
  echo -e "     -> mingw      -> Install dependencies for MinGW inside MSys2 in Windows"
  echo -e "     -> ubuntu     -> Install dependencies for Ubuntu"
  exit
}

Clean() {
  echo "Cleaning $PWD/out"
  rm -rf out
  exit
}

InstallMingwDeps(){
    pacman -Syu --needed \
      git                                       \
      base-devel                                \
      mingw-w64-x86_64-toolchain                \
      mingw-w64-x86_64-cmake                    \
      mingw-w64-x86_64-gtkmm4                   \
      mingw-w64-x86_64-gpgme
    exit
}

InstallUbuntuDeps() {
  sudo apt-get install -y \
    build-essential                             \
    cmake                                       \
    pkg-config                                  \
    libgtkmm-4.0-dev                            \
    libgpgme-dev

  exit
}

Deps() {
  [ -z "$1" ] && echo -e "ERROR: requires second argument\n" && Usage && exit

  [ "$1" == "mingw" ] && InstallMingwDeps
  [ "$1" == "ubuntu" ] && InstallUbuntuDeps
}

Uninstall() {
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
}

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR

[ "$1" == "help" ] && Usage
[ "$1" == "clean" ] && Clean
[ "$1" == "deps" ] && Deps "$2"
[ "$1" == "uninstall" ] && Uninstall

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
  ./clavis
elif [ "$1" == "install" ]; then
  sudo make install
elif [ "$2" == "archlinux" ]; then
  make archlinux
fi

