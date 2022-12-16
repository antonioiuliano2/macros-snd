#!/bin/bash

for i in $(seq 1 28)
 do
  root -l -q /home/utente/Lavoro/macros-snd/FEDRA/invertalign.C\($i+1,$i,431,0,0\)
 done
