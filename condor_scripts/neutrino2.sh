#!/bin/bash
#launched with deepcopy option in run_simScript.py
ProcId=$2
Run=$3
NEVENTS=10000
STARTEVENT=$((Run*NEVENTS))
LSB_JOBINDEX=$((Run+1))
echo $LSB_JOBINDEX

sleep $ProcId

SNDLHC_mymaster=/afs/cern.ch/work/a/aiuliano/public/SNDLHCBuild
export ALIBUILD_WORK_DIR=$SNDLHC_mymaster/sw #for alienv

echo "SETUP"
source /cvmfs/ship.cern.ch/SHiP-2021/latest/setUp.sh
eval `alienv load sndsw/latest`

echo "Starting script, from neutrino number "
echo $STARTEVENT 
INPUTFILES=/afs/cern.ch/work/a/aiuliano/public/Generate_GENIEinput_AnnaritaSND/GenieEventsTP/
echo "From file "
echo $INPUTFILES

OUTPUTDIR=/afs/cern.ch/work/a/aiuliano/public/sim_snd/nueNCDIS_SND_7_January_2020
/cvmfs/ship.cern.ch/SHiP-2021/2020/November/16/sw/slc7_x86-64/Python/v3.6.8-1/bin/python3 $SNDLHC_mymaster/sndsw/macro/run_simScript.py --shiplhc --Genie -f $INPUTFILES/NCDIS/nue/nue_NCDIS_FairShip.root -i $STARTEVENT -n $NEVENTS -o $OUTPUTDIR/$LSB_JOBINDEX
