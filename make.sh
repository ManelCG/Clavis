#! /bin/bash

Usage() {
  echo -e "Usage:"
  echo -e "$0 [arg]"
  echo -e "    help               -> Show this menu"
  echo -e "    clean              -> Clean all output to prepare for a clean build"
  echo -e "    install            -> Build and install Clavis (Linux: system-wide; macOS: build .app + .dmg)"
  echo -e "    archlinux          -> Build and setup an AUR package structure. Only to be used in an AUR PKGBUILD"
  echo -e "    windows-installer  -> Build and package a Windows installer .exe (MSYS2 + Inno Setup 6 required)"
  echo -e "    deps [depname]     -> Install the required dependencies for [depname]"
  echo -e "     -> mingw          -> Install dependencies for MinGW inside MSys2 in Windows"
  echo -e "     -> ubuntu         -> Install dependencies for Ubuntu"
  echo -e "     -> macos          -> Install dependencies via Homebrew on macOS"
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

# Homebrew formulae required to build, package and run Clavis on macOS.
MACOS_BREW_DEPS=(cmake pkg-config gtkmm4 gpgme gnupg pinentry-mac create-dmg)

# Print the subset of MACOS_BREW_DEPS that appears to be missing (space separated).
MissingMacosDeps() {
  local missing=()
  command -v cmake        >/dev/null 2>&1 || missing+=(cmake)
  command -v create-dmg   >/dev/null 2>&1 || missing+=(create-dmg)
  command -v pinentry-mac >/dev/null 2>&1 || missing+=(pinentry-mac)
  command -v gpg          >/dev/null 2>&1 || missing+=(gnupg)

  if command -v pkg-config >/dev/null 2>&1; then
    pkg-config --exists gtkmm-4.0 || missing+=(gtkmm4)
    pkg-config --exists gpgme     || missing+=(gpgme)
  else
    missing+=(pkg-config gtkmm4 gpgme)
  fi

  echo "${missing[@]}"
}

InstallMacosDeps() {
  if ! command -v brew >/dev/null 2>&1; then
    echo "ERROR: Homebrew is required. Install it from https://brew.sh and re-run."
    exit 1
  fi
  echo "Installing macOS dependencies via Homebrew: ${MACOS_BREW_DEPS[*]}"
  brew install "${MACOS_BREW_DEPS[@]}"
  exit
}

# Lightweight dependency check used before a macOS install. Prompts to install any
# missing Homebrew formulae, and warns (without aborting) if the Xcode Command Line
# Tools compiler is missing.
CheckMacosDepsOrPrompt() {
  if ! xcode-select -p >/dev/null 2>&1; then
    echo "WARNING: Xcode Command Line Tools not detected. If the build fails, run:"
    echo "    xcode-select --install"
  fi

  local missing
  missing=$(MissingMacosDeps)
  if [ -z "$missing" ]; then
    echo "All macOS dependencies are present."
    return
  fi

  echo "Missing dependencies: ${missing}"
  if ! command -v brew >/dev/null 2>&1; then
    echo "ERROR: Homebrew is required to install them. See https://brew.sh"
    exit 1
  fi

  read -r -p "Install them now with Homebrew? [Y/n] " reply
  if [[ -z "$reply" || "$reply" =~ ^[Yy]$ ]]; then
    brew install ${missing}
    ERROR=$?
    if [ "$ERROR" != "0" ]; then echo "ERROR ${ERROR}: dependency install failed."; exit ${ERROR}; fi
  else
    echo "Cannot continue without the required dependencies."
    exit 1
  fi
}

Deps() {
  [ -z "$1" ] && echo -e "ERROR: requires second argument\n" && Usage && exit

  [ "$1" == "mingw" ] && InstallMingwDeps
  [ "$1" == "ubuntu" ] && InstallUbuntuDeps
  [ "$1" == "macos" ] && InstallMacosDeps
}

BuildWindowsInstaller() {
  if [ "${CURRENT_OS}" != "WINDOWS" ]; then
    echo "ERROR: windows-installer is only supported on Windows (MSYS2)"
    exit 1
  fi

  cmake -B out -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
  ERROR=$?
  if [ "$ERROR" != "0" ]; then echo "ERROR ${ERROR}!"; exit ${ERROR}; fi

  cmake --build out -- -j"$(nproc)"
  ERROR=$?
  if [ "$ERROR" != "0" ]; then echo "ERROR ${ERROR}!"; exit ${ERROR}; fi

  ISCC=$(command -v ISCC.exe 2>/dev/null || \
         command -v iscc.exe 2>/dev/null || \
         echo "/c/Program Files (x86)/Inno Setup 6/ISCC.exe")

  if [ ! -f "$ISCC" ]; then
    echo "ERROR: Inno Setup compiler (ISCC.exe) not found."
    echo "Install Inno Setup 6 from https://jrsoftware.org/isdl.php"
    echo "Then ensure ISCC.exe is in your PATH or at the default install location."
    exit 1
  fi

  CLAVIS_VERSION=$(sed -n 's/^set(CLAVIS_VERSION "\([^"]*\)").*/\1/p' "${SCRIPT_DIR}/CMakeLists.txt")
  VERSION_PATCH=$(sed -n 's/^set(VERSION_PATCH "\([^"]*\)").*/\1/p' "${SCRIPT_DIR}/CMakeLists.txt")
  FULL_VERSION="${CLAVIS_VERSION}-${VERSION_PATCH}"

  WIN_BUILD_DIR=$(cygpath -w "${SCRIPT_DIR}/out")
  WIN_SOURCE_DIR=$(cygpath -w "${SCRIPT_DIR}")
  WIN_ISS=$(cygpath -w "${SCRIPT_DIR}/bundled/windows/clavis_installer.iss")

  echo "Building Windows installer for Clavis ${FULL_VERSION}..."
  "$ISCC" \
    "/DMyAppVersion=${FULL_VERSION}" \
    "/DBuildDir=${WIN_BUILD_DIR}" \
    "/DSourceDir=${WIN_SOURCE_DIR}" \
    "/DOutputDir=${WIN_BUILD_DIR}" \
    "${WIN_ISS}"

  ERROR=$?
  if [ "$ERROR" != "0" ]; then echo "ERROR ${ERROR}!"; exit ${ERROR}; fi
  echo "Installer: out/clavis_setup_win64_${FULL_VERSION}.exe"
  exit
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

if [[ "$OSTYPE" == "darwin"* ]]; then
  CURRENT_OS="MACOS"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
  CURRENT_OS="LINUX"
elif [[ "$OSTYPE" == "msys"* ]] || [[ "$OSTYPE" == "cygwin"* ]]; then
  CURRENT_OS="WINDOWS"
else
  echo "Unknown OS!"
  exit 1
fi


SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR

[ "$1" == "help" ] && Usage
[ "$1" == "clean" ] && Clean
[ "$1" == "deps" ] && Deps "$2"
[ "$1" == "uninstall" ] && Uninstall
[ "$1" == "windows-installer" ] && BuildWindowsInstaller

# On a macOS install, verify dependencies are present before building and offer to
# install any that are missing.
if [ "$1" == "install" ] && [ "${CURRENT_OS}" == "MACOS" ]; then
  CheckMacosDepsOrPrompt
fi

clear

cmake -B out -G "Unix Makefiles"
ERROR=$?
if [ "$ERROR" != "0" ]; then
  echo ERROR ${ERROR}!
  exit ${ERROR}
fi

cd out || exit 1

if [ ${CURRENT_OS} == "MACOS" ]; then
    NTHREADS=$(sysctl -n hw.ncpu)
else
    NTHREADS=$(nproc)
fi

make -j"${NTHREADS}"

ERROR=$?
if [ "$ERROR" != "0" ]; then
  echo ERROR ${ERROR}!
  exit ${ERROR}
fi

if [ "$1" == run ]; then
  if [ "${CURRENT_OS}" == "MACOS" ]; then
    open ./clavis.app
  else
    ./clavis
  fi
elif [ "$1" == "install" ]; then
  if [ "${CURRENT_OS}" == "MACOS" ]; then
    OUT_FOLDER="${SCRIPT_DIR}/out"

    # Package the freshly built .app into a .dmg, then open it so the user can drag
    # Clavis into /Applications manually.
    rm -f "${OUT_FOLDER}/Clavis.dmg"
    create-dmg \
      --volname "Clavis" \
      --window-pos 200 120 \
      --window-size 500 300 \
      --icon-size 60 \
      --icon "clavis.app" 125 150 \
      --app-drop-link 375 150 \
      "${OUT_FOLDER}/Clavis.dmg" "${OUT_FOLDER}/clavis.app"
    ERROR=$?
    if [ "$ERROR" != "0" ]; then echo "ERROR ${ERROR}: dmg creation failed."; exit ${ERROR}; fi

    echo "Opening ${OUT_FOLDER}/Clavis.dmg — drag Clavis into Applications to finish."
    open "${OUT_FOLDER}/Clavis.dmg"
  else
    sudo make install
  fi
elif [ "$1" == "archlinux" ]; then
  make archlinux
fi
