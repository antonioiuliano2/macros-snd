#!/bin/bash
#launched with deepcopy option in run_simScript.py
ProcId=$2
STARTEVENT=$((ProcId*1000))
LSB_JOBINDEX=$((ProcId+1))
echo $LSB_JOBINDEX

SHIPBUILD_mymaster=/afs/cern.ch/work/a/aiuliano/public/SHIPBuild
export ALIBUILD_WORK_DIR=$SHIPBUILD_mymaster/sw #for alienv

echo "SETUP"
source /cvmfs/ship.cern.ch/SHiP-2020/latest/setUp.sh
eval `alienv load FairShip/latest`

echo "Starting script, from neutrino number "
echo $STARTEVENT 
INPUTFILES=/afs/cern.ch/work/a/aiuliano/public/Generate_GENIEinput_AnnaritaSND/GenieEvents
echo "From file "
echo $INPUTFILES

OUTPUTDIR=/afs/cern.ch/work/a/aiuliano/public/sim_snd/numuCCDIS_09_11_2020
python $SHIPBUILD_mymaster/FairShip/macro/run_simScript.py --shiplhc --Genie -f $INPUTFILES/numu/numu_CCDIS_FairShip.root -i $STARTEVENT -n 1000 -o $OUTPUTDIR/$LSB_JOBINDEX
