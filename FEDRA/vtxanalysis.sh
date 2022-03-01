#!/bin/bash

filename=vertextree.root

brickIDs=(11 12 13 14 21 22 23 24 31 32 33 34 41 42 43 44 51 52 53 54)

for ibrick in $(seq 0 19)
	do
		cd b0000${brickIDs[ibrick]}
	 	if [ ! -f "$filename" ]
	 		then  
	 			echo "++$filename doesn't exist in: $(pwd)! Skipping++"
	 			cd .. 
	 			continue
		fi
		root -l -q /home/simsndlhc/macros-snd/FEDRA/vtx_analysis.C\(\"b0000${brickIDs[ibrick]}\"\)
		cd ..
done
echo "++Now merging all histos..++"
hadd hvtx.allbricks.root b0000*/hvtx*
