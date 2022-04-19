#!/usr/bin/env bash

METHOD="$1"
DIMENSIONS="$2"
FILE="$3"
WORKERS="$4"
echo "$METHOD  $DIMENSIONS $FILE" 


if [[ -z "$METHOD" ]]; then
  echo "Method is empty"
  exit
fi
if [[ -z "$DIMENSIONS" ]]; then
  echo "Dimensions are empty"
  exit
fi
if [[ -z "$FILE" ]]; then
  echo "File is empty"
  exit
fi
if [[ -z "$WORKERS" ]]; then
  echo "No workers defined,set to 2"
  WORKERS ="2"
fi  

[ -d "outputs" ] || mkdir "outputs"
[ -d "outputs/${METHOD}$" ] || mkdir "outputs/${METHOD}"

echo "setting path for the compiler, needs sudo"

sudo update-alternatives --set g++  /usr/bin/g++-6
PATH=/usr/bin/:$PATH
g++ -v

echo "entering parChain/parchain/linkage/framework"
cd parchain/linkage/framework
echo "compile PC... without clean"
# make clean
GCILK=1 make -j

echo "bat to root parChain/parchain/linkage/framework"
cd ../../../
echo "run"
echo "CILK_NWORKERS=$WORKERS numactl -i all ./parchain/linkage/framework/linkage -method avg -r 1 -d $DIMENSIONS -dendro ./outputs/${METHOD}/dendrogaram.txt  ./datasets/$FILE  > outputs/${METHOD}/test.txt"
CILK_NWORKERS=$WORKERS numactl -i all ./parchain/linkage/framework/linkage -method avg -r 1 -d $DIMENSIONS -dendro ./outputs/${METHOD}/dendrogaram.txt  ./datasets/$FILE  > outputs/${METHOD}/test.txt
if [ $? -eq 0 ] 
then 
  echo "DONE" 
else 
  echo "ERROR" 
fi
