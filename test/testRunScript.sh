#!/bin/bash
# echo "Input file name is $1"
FILE=$(basename $1)
filename="${FILE%.*}"

export JAVA_HOME=$3
if [ -z "$JAVA_HOME" ]
then
  echo "\$JAVA_HOME not set."
  exit 1
fi

# Skip BIR dump generation if forth input arg is set
if [ -z "$5" ]
then
  ballerina build --dump-bir-file=$filename-bir-dump $1 1>bal_out.log 2>bal_err.log
else
  if [ ! -s $filename-bir-dump ]
  then
    ballerina build --dump-bir-file=$filename-bir-dump $1 1>bal_out.log 2>bal_err.log
  fi
fi

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
  cat ./nbal_err.log
  exit 1
fi

clang-11 --target=x86_64-unknown-linux-gnu -c -O3 -flto=thin -Wno-override-module -o $filename.o $filename-bir-dump.ll 2>clang_err.log

if [ -s ./clang_err.log ]
then
  echo "clang error/warning. Error msg: "
  cat ./clang_err.log
  exit 1
fi

clang-11 -flto=thin -fuse-ld=lld-11 -L../../runtime/$4/ -lballerina_rt -lpthread -ldl -o $filename.out -O3 $filename.o 2>lld_err.log
if [ -s ./lld_err.log ]
then
  echo "Linker error/warning. Error msg: "
  cat ./lld_err.log
  exit 1
fi

./$filename.out

rm $filename-bir-dump.ll $filename.out $filename.o
