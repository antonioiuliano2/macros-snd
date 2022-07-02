#!/bin/bash

#hash list of pdgmaterials
declare -A pdgmaterials

pdgmaterials['H1']='1000010010'
pdgmaterials['B11']='1000050110'
pdgmaterials['C11']='1000060110'
pdgmaterials['C12']='1000060120'
pdgmaterials['N14']='1000070140'
pdgmaterials['O16']='1000080160'
pdgmaterials['F19']='1000090190'
pdgmaterials['Na23']='1000110230'
pdgmaterials['Mg24']='1000120240'
pdgmaterials['Al27']='1000130270'
pdgmaterials['Si28']='1000140280'
pdgmaterials['S32']='1000160320'
pdgmaterials['Ar40']='1000180400'
pdgmaterials['K39']='1000190390'
pdgmaterials['Ca40']='1000200400'
pdgmaterials['Ti48']='1000220480'
pdgmaterials['Fe56']='1000260560'
pdgmaterials['Br80']='1000350800'
pdgmaterials['Sr88']='1000380880'
pdgmaterials['Ag108']='1000471080'
pdgmaterials['I127']='1000531270'
pdgmaterials['Ba137']='1000561370'
pdgmaterials['W184']='1000741840'

MATERIAL=$3
PDG=${pdgmaterials[$MATERIAL]}

echo $PDG

#SNDSW configuration
source /cvmfs/sndlhc.cern.ch/SNDLHC-2022/June10/setUp.sh
SNDLHC_mymaster=/afs/cern.ch/work/a/aiulian/public/SNDLHCBuild
export ALIBUILD_WORK_DIR=$SNDLHC_mymaster/sw
eval `alienv load sndsw/latest`

echo "Launching gmkspl for material "$MATERIAL" with code "$PDG
OUTPUTFOLDER=/afs/cern.ch/work/a/aiulian/public/genie_sims/crisconversion/tunedsplines
gmkspl -p 12,-12,14,-14,16,-16 -t $PDG -n 100 -e 5000 -o $OUTPUTFOLDER/$MATERIAL"_splines.xml" --tune SNDG18_02a_01_000
