#!/bin/bash
# echo "Input file name is $1"
FILE=$(basename $1)
filename="${FILE%.*}"

if [ -z "$JAVA_HOME" ]
then
  echo "\$JAVA_HOME not set."
  exit 1
fi

ballerina build --dump-bir-file=$filename-bir-dump $1 1>bal_out.log 2>bal_err.log

#Ignoring Ballerina compiler errors due to the use of undefined external functions for print routines
#if [ -s ./err.log ]
#then
#  echo "JBallerina error . Check err.log"
#  exit 1
#fi

$2 $filename-bir-dump  2>nbal_err.log

if [ -s ./nbal_err.log ]
then
  echo "nballerinacc error. Error msg: "
  echo ./nbal_err.log
  exit 1
fi

clang -O0 -o $filename.out $filename-bir-dump.ll -L../../../runtime/target/release/ -lballerina_rt 2>clang_err.log

if [ -s ./clang_err.log ]
then
  echo "clang error/warning. Error msg: "
  echo ./clang_err.log
  exit 1
fi

export LD_LIBRARY_PATH=../../../runtime/target/release

./$filename.out

rm $filename-bir-dump.ll $filename.out
