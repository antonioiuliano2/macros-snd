#!/bin/bash

brickIDs=(11 12 13 14 21 22 23 24 31 32 33 34 41 42 43 44 51 52 53 54)

for ibrick in $(seq 0 19)
	do
		python /home/simsndlhc/macros-snd/FEDRA/vtx_sig_analysis.py -b ${brickIDs[ibrick]}
 	done

cd /home/simsndlhc/Simulations_sndlhc/sim_fedra/numu_sim_activeemu_10_November_2021_withinefficiency_withmomentum

hadd -f vertex_sigmu2.root vertex_sigmu_*.root
hadd -f hist_sigmu2.root hist_out_*.root

rm vertex_sigmu_*.root
rm hist_out_*.root
