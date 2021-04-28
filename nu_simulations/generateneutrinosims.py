'''generate neutrino GENIE sims for SND'''
import os

process = "CCDIS"

os.chdir(process)

#neutrinopdgs = [12,-12,14,-14,16,-16]
#neutrinonames = ["nue","anue","numu","anumu","nutau","anutau"]
neutrinopdgs = [12,-12,-14]
neutrinonames = ["nue","anue","anumu"]
#loop over neutrinos
for pdg,name in zip(neutrinopdgs, neutrinonames):
 os.makedirs(name)
 os.chdir(name)
 os.system("python /afs/cern.ch/work/a/aiuliano/public/SNDLHCBuild/sndsw/macro/makeSNDGenieEvents.py sim --nupdg "+ str(pdg) +" -p "+ str(process) +" -n 100000 -o ./") 
 os.chdir("..")
