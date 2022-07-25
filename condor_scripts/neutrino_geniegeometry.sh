#!/bin/bash
ProcId=$2
NEVENTS=750
LSB_JOBINDEX=$((ProcId+1))
echo $LSB_JOBINDEX

sleep $ProcId

SNDLHC_mymaster=/afs/cern.ch/work/a/aiulian/public/SNDLHCBuild
export ALIBUILD_WORK_DIR=$SNDLHC_mymaster/sw #for alienv

echo "SETUP"
source /cvmfs/sndlhc.cern.ch/SNDLHC-2022/June10/setUp.sh
eval `alienv load sndsw/latest`

echo "Starting script, from neutrino number "
echo $STARTEVENT 
INPUTFILES=/eos/user/c/cvilela/SND_MC_June21/neutrino/sndlhc_13TeV_down_volTarget_100fb-1_SNDG18_02a_01_000/
echo "From file "
echo $INPUTFILES/$LSB_JOBINDEX/sndlhc_+volTarget_0.781e16_SNDG18_02a_01_000.0.gst.root

OUTPUTDIR=/afs/cern.ch/work/a/aiulian/public/sim_snd/condor_sims/numu_sim_activeemu_withcrisfiles_25_July_2022

/cvmfs/sndlhc.cern.ch/SNDLHC-2022/June10/sw/slc7_x86-64/Python/v3.6.8-local1/bin/python3 $SNDLHC_mymaster/sndsw/shipLHC/run_simSND.py --Genie 4 -f $INPUTFILES/$LSB_JOBINDEX/sndlhc_+volTarget_0.781e16_SNDG18_02a_01_000.0.gst.root -n $NEVENTS -o $OUTPUTDIR/$LSB_JOBINDEX
