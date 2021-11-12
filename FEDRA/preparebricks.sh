#!/bin/bash

for ibrick in $(seq 0 19)
 do
  root -l -q fromsndsw2FEDRA.C\(\"inECC_sndLHC.Genie-TGeant4.root\",$ibrick\)
 done
