import uproot
import awkward #need for flatten or other operations to nested arrays
import numpy as np

def access_track_branches(simtree, prefix="MCTrack.f", entry_start=0, entry_stop = -1):
 '''reading MCTracks from entry_start to lastentry'''
 if (entry_stop < 0):
        lastentry = simtree.num_entries
 else:
        lastentry = entry_stop
 #what branches do we want to read
 suffixes = ["PdgCode","StartX","StartY","StartZ","Px","Py","Pz"]
 branchnames = [prefix + suff for suff in suffixes]
 print(branchnames)
 #reading the branches
 tracks = simtree.arrays(branchnames,entry_start,lastentry)
 #returning the produced array
 return tracks

def access_mcpoint_branches(simtree, prefix = "ScifiPoint.f", entry_start=0, entry_stop = -1 ):
 '''reading MCTracks from entry_start to lastentry, to be launched after access_track_branches'''
 if (entry_stop < 0):
        lastentry = simtree.num_entries
 else:
        lastentry = entry_stop   

 suffixes = ["PdgCode","TrackID","X","Y","Z","Px","Py","Pz"]
 branchnames = [prefix + suff for suff in suffixes]
 print(branchnames)
 #reading the branches
 mcpoints = simtree.arrays(branchnames[0:2],entry_start = entry_start, entry_stop = lastentry)
 #adding float interpretation manually
 floatinterpretation = simtree["MCTrack.fStartX"].interpretation
 for name in branchnames[2:]:
  mcpoints[name] = simtree[name].array(floatinterpretation)
 #returning the produced array
 return mcpoints

def eventIDs(simtree):
 '''return list of entries'''
 nevents = simtree.num_entries
 events = np.arange(0,nevents)
 return events

