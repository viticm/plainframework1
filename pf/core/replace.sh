#!/bin/sh

files=`find ./ -name "*.cc" -o -name "*.h" -o -name "*.inl"`

for filename in $files 
do
  echo $filename
  sed -i 's;PS_COMMON;PF;g' $filename
  sed -i 's;common/;pf/;g' $filename
  sed -i 's;PLAIN SERVER Engine;PLAIN FRAMEWORK;g' $filename
  sed -i 's;plainserver;plainframework;g' $filename
  sed -i 's;PS_;PF_;g' $filename
  sed -i 's;ps_common_;pf_;g' $filename
  sed -i 's;plianserver;plainframework;g' $filename
  sed -i 's;_PF_GATEWAY;_VGATEWAY;g' $filename
  sed -i 's;_PF_LOGIN;_VLOGIN;g' $filename
  sed -i 's;_PF_CENTER;_VCENTER;g' $filename
  sed -i 's;_PF_SERVER;_VSERVER;g' $filename
  sed -i 's;char\* ;char \*;g' $filename
  sed -i 's;void\* ;void \*;g' $filename
  sed -i 's;int32_t\& ;int32_t \&;g' $filename
  sed -i 's;PAP Engine;PLAIN FRAMEWORK;g' $filename
done
