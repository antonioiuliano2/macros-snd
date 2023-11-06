#!/bin/bash

RED=$'\033[0;31m' #Red coloring                                                                                                                                                  
NC=$'\033[0m' #No coloring
filename=vertextree.root
neutrinofile=$1
brickIDs=(11 12 13 14 21 22 23 24 31 32 33 34 41 42 43 44 51 52 53 54)

for ibrick in $(seq 0 19)
	do
		cd b0000${brickIDs[ibrick]}
	 	if [ ! -f "$filename" ]
	 		then  
	 			echo "${RED}++$filename doesn't exist in: $(pwd)! Skipping++${NC}"
	 			cd .. 
	 			continue
		else
	    	echo "+++ Analysing bkg file in $(pwd) +++"
	    	python /home/simsndlhc/macros-snd/FEDRA/vtx_muon_analysis.py -b ${brickIDs[ibrick]}
	    	cd ..
		fi
done

cd /home/simsndlhc/Simulations_sndlhc/sim_fedra/muonbkg_1E5cm2

hadd -f vertex_muon2.root vertex_muon_*.root
hadd -f hist_muon2.root hist_out_*.root

rm vertex_muon_*.root
rm hist_out_*.root

