#!/bin/bash

# Bash script wrapping all of "FairShip2Fedra" conversion tools
# you'll need to copy in this folder the sim file you want to
# convert in FEDRA
RED=$'\033[0;31m' #Red coloring                                                                                                                                                  
NC=$'\033[0m' #No coloring

SIMFILE=$(ls sndLHC.Ntuple-TGeant4-*.root)
[ ! -d "$SIMFILE" ] && echo "${RED}++${NC} You are about to convert $SIMFILE ${RED}++${NC}"
read -p "Please confirm (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	return
fi
echo "${RED}++${NC} Bricks and plates folders will be created in the following directory: $(pwd) ${RED}++${NC}"
read -p "Please confirm (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	return
fi
cp /home/simsndlhc/macros-snd/FEDRA/preparebricks.sh .
cp /home/simsndlhc/macros-snd/FEDRA/doreco.sh .
cp /home/simsndlhc/macros-snd/FEDRA/csvconversion.sh .
cp /home/simsndlhc/macros-snd/FEDRA/addvertexinfo.sh .
cp /home/simsndlhc/macros-snd/FEDRA/fromsndsw2FEDRA.C .
cp /home/simsndlhc/macros-snd/FEDRA/FairShip2Fedra.rootrc .
cp /home/simsndlhc/macros-snd/GetEntries.C .
for i in $(seq 1 5)
	do
		mkdir b0000${i}{1..4}
		mkdir b0000${i}{1..4}/p0{01..60}
	done
echo "++++++++++++++++"
cat FairShip2Fedra.rootrc
echo "${RED}++${NC} Check the settings above !! ${RED}++${NC}"
read -p "Do you wish to proceed ? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	return
fi
for ibrick in $(seq 0 19)
	do
		root -l -q fromsndsw2FEDRA.C\(\"$SIMFILE\",$ibrick\)
	done
echo "${RED}++${NC} Proceeding to doreco.sh ${RED}++${NC}"
source doreco.sh
echo "${RED}++${NC} Done ${RED}++${NC}"
echo "${RED}++${NC} Now performing csvconversion ${RED}++${NC}"
source csvconversion.sh
echo "${RED}++${NC} Done ${RED}++${NC}"
echo "${RED}++${NC} Now adding vertex informations ${RED}++${NC}"
source addvertexinfo.sh
echo "${RED}++${NC} Merging all vertextree files ${RED}++${NC}"
hadd vertextree_allbricks.root b0000*/vertextree.root
echo "${RED}++${NC} SND2FEDRA conversion successfully ended ${RED}++${NC}"
