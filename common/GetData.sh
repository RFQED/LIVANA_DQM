#!/bin/sh

Filename="lz_201704010000_001000_lzap.root"

if [ -z "${1}" ]; then
    echo "Wrong usage!!"
    echo "Usage: ./GetData.sh pdsf_username"
    exit
fi

mkdir -p data

scp ${1}@pdsf.nersc.gov:/projecta/projectdirs/lz/data/MDC2/calibration/LZAP-3.1.0-PHYSICS-3.1.0/Na22_700/$Filename data/.

echo "data/$Filename" > RQfile.list

