#!/usr/bin/env bash
#set -o errexit -o pipefail -o noclobber

# Set up SND environment
ADVSNDBUILD_DIR=/afs/cern.ch/work/a/aiulian/public/AdvSND_versione_con_scavo/
#ADVSNDBUILD_DIR=/afs/cern.ch/user/d/dannc/SNDBUILD2/
#source /cvmfs/sndlhc.cern.ch/SNDLHC-2023/Aug30/setUp.sh
source /cvmfs/sndlhc.cern.ch/SNDLHC-2024/June25/setUp.sh
#eval `alienv load -w $ADVSNDBUILD_DIR/sw --no-refresh advsndsw/latest`
#source ${ADVSNDBUILD_DIR}/advsnd_august2024.sh 
#source ${ADVSNDBUILD_DIR}/advsnd_NoExcOpt.sh
#source ${ADVSNDBUILD_DIR}/advsnd_apr25.sh
#source ${ADVSNDBUILD_DIR}/dump_advFullTargetHCAL.sh
source ${ADVSNDBUILD_DIR}/AdvSND_August2024_env.sh
set -o nounset

#GEOFILE="/afs/cern.ch/work/d/dannc/public/AdvSND/2024/geofiles/AdvSND-NoExcOpt.gdml"
#GEOFILE="/afs/cern.ch/work/d/dannc/public/AdvSND/2024/geofiles/AdvSND-46c14598c5d43e7b2e5710907159ec2ae62b43fa.gdml"
#GEOFILE="/afs/cern.ch/work/d/dannc/public/AdvSND/2024/geofiles/AdvSND2024-FullTarget-HACL60-60-70-90_noVertCoil.gdml"
GEOFILE="/afs/cern.ch/work/a/aiulian/public/AdvSND_versione_con_scavo/AdvSND-August2024.gdml"
MPL="/afs/cern.ch/work/d/dannc/public/AdvSND/2024/auxiliary/mympl_plus.xml"
XSECTION="/afs/cern.ch/work/d/dannc/public/AdvSND/2024/splines/genie_splines_GENIE_v32_ADVSNDG18_02a_01_000_2_plus.xml"
FLUX="/eos/experiment/sndlhc/MonteCarlo/FLUKA/AdvSND/FAR/neutrino/HL-LHC_neutrinos_TI18_20e6pr.gsimple.root"
NEVENTS=$2
TUNE=SNDG18_02a_01_000
EVENTGENLIST=CCDIS
TOPVOLUME=$4
OUTPUTFILE="sndlhc_"$TOPVOLUME"_"$NEVENTS"_ADV"$TUNE".0.ghep.root"
NEUTRINO=$3

ProcId=$1
LSB_JOBINDEX=$((ProcId+1))
SEED=$LSB_JOBINDEX

EOSSERVER=root://eospublic.cern.ch/
OUTPUTDIR=$5

#set -x

gevgen_fnal -f "$FLUX,,-$NEUTRINO,$NEUTRINO" \
    -g $GEOFILE \
    -t $TOPVOLUME \
    -L "cm" \
    -D "g_cm3" \
    -n $NEVENTS \
    -o $(basename $OUTPUTFILE .0.ghep.root) \
    --tune $TUNE \
    --cross-sections $XSECTION \
    --message-thresholds $GENIE/config/Messenger_laconic.xml \
    --seed $SEED \
    -z -3 \
    --event-generator-list $EVENTGENLIST \
    #-m $MPL

xrdcp $OUTPUTFILE $EOSSERVER/$OUTPUTDIR/$LSB_JOBINDEX/$OUTPUTFILE
