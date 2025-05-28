#! /bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR

if [ "$1" == "clean" ]; then
  echo "Cleaning $PWD/out"
  rm -rf out
  exit
fi

clear

cmake -B out

if [ "$?" != "0" ]; then
  echo ERROR!
  exit
fi

cd out

make
if [ "$?" != "0" ]; then
  echo ERROR!
  exit
fi

if [ "$1" == run ]; then
  ./Clavis
fi

