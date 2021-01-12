#!/bin/bash
echo "Input file name is $1"
FILE=$(basename $1)
filename="${FILE%.*}"
echo "file name: $filename"

export JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-amd64
WORK_PATH=/home/shubham/merging-nballerina

NBALLERIAN_PATH=$WORK_PATH/nballerina/BIR2llvmIR
TESTBUILD_PATH=$WORK_PATH/nballerina/build/llvm/test/CodeGen/NBallerina

ballerina build --dump-bir --dump-bir-file=$filename-bir-dump $1 1>out.log 2>err.log

if [ -s ./err.log ]
then
  echo "JBallerina error . Check err.log"
  exit 1
fi

$NBALLERIAN_PATH/./nballerinacc $filename-bir-dump  2>err.log

if [ -s ./err.log ]
then
  echo "nballerinacc error. Check err.log"
  exit 1
fi


clang -O0 -o $filename.out $filename-bir-dump.ll 2>err.log

./$filename.out
echo RETVAL=$?

rm $filename-bir-dump.ll $filename-bir-dump $filename.jar ./$filename.out
