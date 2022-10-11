#!/bin/bash

BASE_OUT_DIR=$1
N_PP_COLL=$2
TUNE=$3
GSIMPLE_FLUX=$4
TARGET=$5
SEED=$6

echo "RUNNING " $BASE_OUT_DIR $N_PP_COLL $TUNE $GSIMPLE_FLUX $TARGET $SEED

# Check if this solves job crashing problem. Typically run 100 jobs, so at most wait 15 minutes before starting.
#sleep $(( SEED*18 ))s

# Set up SND environment
SNDBUILD_DIR=/afs/cern.ch/work/a/aiulian/public/SNDLHCBuild/sw/
source /cvmfs/sndlhc.cern.ch/SNDLHC-2022/July14/setUp.sh
eval `alienv load -w $SNDBUILD_DIR --no-refresh sndsw/latest-master-release`

export EOSSHIP=root://eosuser.cern.ch/

# Run genie:
gevgen_fnal -f "${GSIMPLE_FLUX},,-12,12,-14,14,-16,16" -g /afs/cern.ch/work/a/aiulian/public/officialsndswgeo/geofile_sndlhc_TI18.gdml -t ${TARGET}  -L "cm" -D "g_cm3" -e $N_PP_COLL -o "sndlhc_${TARGET}_${N_PP_COLL}_${TUNE}" --tune ${TUNE} --cross-sections /eos/home-c/cvilela/genie_splines_GENIE_v32_${TUNE}.xml --message-thresholds $GENIE/config/Messenger_laconic.xml --seed ${SEED}

# Convert file to GST
gntpc -i sndlhc_${TARGET}_${N_PP_COLL}_${TUNE}.0.ghep.root -f gst -c

# Add axiliary variables to GST file
addAuxiliaryToGST sndlhc_${TARGET}_${N_PP_COLL}_${TUNE}.0.ghep.root sndlhc_${TARGET}_${N_PP_COLL}_${TUNE}.0.gst.root

# Run detector simulation
python $SNDSW_ROOT/shipLHC/run_simSND.py --Genie 4 -f ./sndlhc_${TARGET}_${N_PP_COLL}_${TUNE}.0.gst.root -o ./ -n 1000000

# Run digitization
python $SNDSW_ROOT/shipLHC/run_digiSND.py -g ./geofile_full.Genie-TGeant4.root -f ./sndLHC.Genie-TGeant4.root -cpp -n 1000000

# Run muon reconstruction 
python ${SNDSW_ROOT}/shipLHC/run_muonRecoSND.py -g ./geofile_full.Genie-TGeant4.root -f ./sndLHC.Genie-TGeant4_digCPP.root -o

# Copy output
mkdir -p ${BASE_OUT_DIR}/${SEED}/
xrdcp -f ./*.root ${BASE_OUT_DIR}/${SEED}/
xrdcp -f ./*.status ${BASE_OUT_DIR}/${SEED}/
rm -rf ./*.root ./*.status