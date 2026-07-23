#!/usr/bin/env bash
set -o errexit -o pipefail -o noclobber

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

ProcId=$1
LSB_JOBINDEX=$((ProcId+1))

INPUT=$2
EOSSERVER=root://eospublic.cern.ch/
#OUTPUTDIR=/eos/experiment/sndlhc/users/dancc/AdvSND/2024_sim/numu_AdvSND-August2024/volume_${TOPVOLUME}/$LSB_JOBINDEX
OUTPUTDIR=$3
OUTPUT=$(basename $INPUT .ghep.root).gst.root

set -x


#xrdcp $EOSSERVER/$OUTPUTDIR/$LSB_JOBINDEX/$INPUT ./$INPUT

gntpc -i ${OUTPUTDIR}/${LSB_JOBINDEX}/$INPUT -f gst -o ${OUTPUTDIR}/${LSB_JOBINDEX}/$OUTPUT -c
addAuxiliaryToGST ${OUTPUTDIR}/${LSB_JOBINDEX}/$INPUT ${OUTPUTDIR}/${LSB_JOBINDEX}/$OUTPUT

#xrdcp $OUTPUT $EOSSERVER/$OUTPUTDIR/$LSB_JOBINDEX/$OUTPUT
