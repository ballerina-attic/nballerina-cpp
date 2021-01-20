#!/bin/bash
echo "Input file name is $1"
FILE=$(basename $1)
filename="${FILE%.*}"

JAVA_HOME=$(realpath $JAVA_HOME)

if [ -z "$JAVA_HOME"]
then
  echo "\$JAVA_HOME not set."
  exit 1
fi

export JAVA_HOME=$JAVA_HOME

WORK_PATH=$(realpath $WORK_PATH)

if [ -z "$WORK_PATH"]
then
  echo "\$WORK_PATH not set."
  exit 1
fi

ballerina build --dump-bir-file=$filename-bir-dump $1 1>out.log 2>err.log

if [ -s ./err.log ]
then
  echo "JBallerina error . Check err.log"
  exit 1
fi

$WORK_PATH/./nballerinacc $filename-bir-dump  2>err.log

if [ -s ./err.log ]
then
  echo "nballerinacc error. Check err.log"
  exit 1
fi

clang -O0 -o $filename.out $filename-bir-dump.ll 2>err.log

./$filename.out
echo RETVAL=$?

rm $filename-bir-dump.ll $filename-bir-dump $filename.jar ./$filename.out
