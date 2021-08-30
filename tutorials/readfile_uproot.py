'''how to use uproot to read ship conical files'''
import uproot
import numpy as np
import awkward as ak
import matplotlib.pyplot as plt
import root_numpy
import ROOT as r

inputfile = uproot.open("ship.conical.Genie-TGeant4.root")
inputtree = inputfile["cbmsim"]

#MCTrack branches can be directly extracted
nevents = 10000
#we retrieve the interpretation
floatinterpretation = inputtree["MCTrack.fPx"].interpretation
print("Interpretation for track fPx:", floatinterpretation)
#Hit branches (except ints) cannot be accessed this way, interpretation is unknown
print("Interpretation for hit fPx:", inputtree["ScifiPoint.fPx"].interpretation)

htP = r.TH1D("trackP","Track momentum;P[GeV/c]",50,0,10)
hhitP = r.TH1D("hitP","Hit momentum;P[GeV/c",50,0,10)

#cannot read all 100k events in a laptop together, too much memory. Going for a standard loop

#test: computing trimomentum
track_px = inputtree["MCTrack.fPx"].array(floatinterpretation,entry_start=0,entry_stop=nevents)
track_py = inputtree["MCTrack.fPy"].array(floatinterpretation,entry_start=0,entry_stop=nevents)
track_pz = inputtree["MCTrack.fPz"].array(floatinterpretation,entry_start=0,entry_stop=nevents)
track_P = np.sqrt(track_px**2 + track_py**2 + track_pz**2)
#Integer hit branches can be accessed without issues
hit_trackID = inputtree["ScifiPoint.fTrackID"].array(entry_start=0,entry_stop=nevents)
#We need to assign interpretation for float branches

hit_px = inputtree["ScifiPoint.fPx"].array(floatinterpretation,entry_start=0,entry_stop=nevents)
hit_py = inputtree["ScifiPoint.fPy"].array(floatinterpretation,entry_start=0,entry_stop=nevents)
hit_pz = inputtree["ScifiPoint.fPz"].array(floatinterpretation,entry_start=0,entry_stop=nevents)
hit_P = np.sqrt(hit_px**2 + hit_py**2 + hit_pz**2)


c0 = r.TCanvas()
root_numpy.fill_hist(htP, ak.flatten(track_P))
htP.Draw()

c1 = r.TCanvas()
root_numpy.fill_hist(hhitP, ak.flatten(hit_P))
hhitP.Draw()