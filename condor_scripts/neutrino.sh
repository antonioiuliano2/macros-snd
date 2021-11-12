#!/bin/bash
#launched with deepcopy option in run_simScript.py
ProcId=$2
NEVENTS=10000
STARTEVENT=$((ProcId*NEVENTS))
LSB_JOBINDEX=$((ProcId+1))
echo $LSB_JOBINDEX

sleep $ProcId

SNDLHC_mymaster=/afs/cern.ch/work/a/aiulian/public/SNDLHCBuild
export ALIBUILD_WORK_DIR=$SNDLHC_mymaster/sw #for alienv

echo "SETUP"
source /cvmfs/sndlhc.cern.ch/latest/setUp.sh
eval `alienv load sndsw/latest`

echo "Starting script, from neutrino number "
echo $STARTEVENT 
INPUTFILES=/afs/cern.ch/work/a/aiulian/public/genie_sims
echo "From file "
echo $INPUTFILES

OUTPUTDIR=/afs/cern.ch/work/a/aiulian/public/sim_snd/condor_sims/numu_sim_activeemu_10_November_2021

/cvmfs/sndlhc.cern.ch/SNDLHC-2021/June/15/sw/slc7_x86-64/Python/v3.6.8-local1/bin/python3 $SNDLHC_mymaster/sndsw/shipLHC/run_simSND.py --Genie 1 -f /afs/cern.ch/work/a/aiulian/public/genie_sims/GenieEvents_3_06/CCDIS/numu_CCDIS_FairShip_withFLUKA.root -i $STARTEVENT -n $NEVENTS -o $OUTPUTDIR/$LSB_JOBINDEX
