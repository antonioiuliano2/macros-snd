#!/bin/bash

for i in $(seq 7 30)
 do
  root -l -q /home/scanner/sndlhc/macros-snd/FEDRA/invertalign.C\($i+1,$i,431,0,0\)
 done
