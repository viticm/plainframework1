#!/bin/sh

files=`find ./ -name "*.cc" -o -name "*.h" -o -name "*.inl"`

for filename in $files 
do
  echo $filename
  sed -i 's;PS_COMMON;PF;g' $filename
  sed -i 's;common/;pf/;g' $filename
  sed -i 's;PLAIN SERVER Engine;PLAIN FRAMEWORK;g' $filename
  sed -i 's;plainserver;plainframework;g' $filename
  sed -i 's;ps_common_;pf_;g' $filename
  sed -i 's;plianserver;plainframework;g' $filename
done
