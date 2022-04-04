'''producing plots from neutrino simulations with uproot'''

import uproot
import awkward #need for flatten or other operations to nested arrays
import numpy as np
import matplotlib.pyplot as plt #for plots
from particle import Particle #Scikit-HEP equivalent for TDatabasePDG

import uproot_read_functions.py as readfunc #my functions

#open the file, getting the TTree

numufile = uproot.open("/eos/user/a/aiulian/sim_snd/numu_sim_activeemu_7_September_2021/inECC_sndLHC.Genie-TGeant4.root")
numutree = numufile["cbmsim"]

#retrieve track branch

tracks = access_track_branches(numutree)

def getcharge(pdgcode):
 '''retrieving particle charge from pdgcode'''
 charge = 0

 try: #not all particles are known
  charge = Particle.from_pdgid(charge).charge
 except:
  print("Particle {} not recognized, returing charge 0".format(pdgcode))

 return charge
#we need to add the charge, for now we do it slowly (loop-based)

trackcharge = []
for event in range(nevents):
    trackchargeevent = []
    for pdgcode in trackpdgs[event]:
        trackchargeevent.append(getcharge(pdgcode))
    trackcharge.append(trackchargeevent)

awtrackcharge = awkward.Array(trackcharge)
tracks["MCTrack.fCharge"] = awtrackcharge

#retrieving list of neutrino daughters
neutrinodaughters = np.logical_and(tracks["MCTrack.fMotherId"]==0,np.abs(tracks["MCTrack.fCharge"]) > 0)
neutrinodaughters_pdgs = trackpdgs[neutrinodaughters]

#counting molteplicity
molteplicity = []
for event in range(nevents):
    molteplicity.append(len(neutrinodaughters_pdgs[event]))

#plots section

plt.figure()
plt.hist(molteplicity,bins=20,range=[0,20])
plt.xlabel("NTracks")
plt.title("Molteplicity of neutrino vertex")
plt.show()
