#!/bin/bash

# Bash script wrapping all of "FairShip2Fedra" conversion tools
# you'll need to copy in this folder the sim file you want to
# convert in FEDRA
SIMFILE=sndLHC.Ntuple-TGeant4-*.root
[ ! -d "$SIMFILE" ] && echo "++ You are about to convert $SIMFILE ++"
read -p "Please confirm (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	exit 1
fi
cp /home/simsndlhc/macros-snd/FEDRA/preparebricks.sh .
cp /home/simsndlhc/macros-snd/FEDRA/doreco.sh .
cp /home/simsndlhc/macros-snd/FEDRA/csvconversion.sh .
cp /home/simsndlhc/macros-snd/FEDRA/addvertexinfo.sh .
cp /home/simsndlhc/macros-snd/FEDRA/fromsndsw2FEDRA.C .
cp /home/simsndlhc/macros-snd/FEDRA/FairShip2Fedra.rootrc .
echo "++ Bricks and plates folders will be created in the current directory ++"
read -p "Please confirm (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	exit 1
fi
for i in $(seq 1 5)
	do
		mkdir b0000${i}{1..4}
		mkdir b0000${i}{1..4}/p0{01..60}
	done
echo "++++++++++++++++"
cat FairShip2Fedra.rootrc
echo "++ Check the settings above !! ++"
read -p "Do you wish to proceed ? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	exit 1
fi
for ibrick in $(seq 0 19)
	do
		root -l -q fromsndsw2FEDRA.C\(\"$SIMFILE\",$ibrick\)
	done
echo "++ Proceeding to doreco.sh ++"
cat /home/simsndlhc/macros-snd/FEDRA/track.rootrc
echo "++ The settings written above will be used for track reco ++"
read -p "Do you wish to proceed ? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	exit 1
fi
source doreco.sh
echo "++ Done ++"
echo "++ Now performing csvconversion ++"
source csvconversion.sh
echo "++ Done ++"
echo "++ Now adding vertex informations ++"
source addvertexinfo.sh
#unsure on the following "hadd vertextree_allbricks.root */vertextrree.root"
echo "++ SND2FEDRA conversion successfully ended ++"
