#!/bin/bash
brickIDs=(11 12 13 14 21 22 23 24 31 32 33 34 41 42 43 44 51 52 53 54) #ehi, I have written an array in BASH!
for ibrick in $(seq 0 19)
 do
  cd b0000${brickIDs[ibrick]}
  cp /afs/cern.ch/work/a/aiulian/public/macros-snd/scipy_conversion/csvconversion.py ./
  echo "now converting " b0000${brickIDs[ibrick]}
  python csvconversion.py ${brickIDs[ibrick]}
  cd ..
 done
