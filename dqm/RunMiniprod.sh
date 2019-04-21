#!/bin/bash

for rootfile in /bundle/data/LZ/MDC3_miniProduction/lz_Miniproduction_1?_lzap.root
  do
  echo "$rootfile"
  fileIDNum=${rootfile:54:2}
  id=${rootfile:39:17}

  echo "$id"
  echo "fileIDNum"
  dataSetMetaData="LZDQ_"$id"/dqsetmeta.xml"
  echo "$dname"

  root -l -b -q ./dqplots.C+(\"$rootfile\",$((10#$fileIDNum))) 
  root -l -b -q ./readaddxmldq.C+(\"dqsets.xml\",\"$dataSetMetaData\",\"dqsets.xml\")
  root -l -b -q ./readaddxmldq.C+(\"refdqsets.xml\",\"$dataSetMetaData\",\"refdqsets.xml\")

done;

 
