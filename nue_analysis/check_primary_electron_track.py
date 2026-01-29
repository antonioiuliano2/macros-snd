import uproot, awkward
import pandas as pd
import numpy as np
import sys

inputpath = "/eos/experiment/sndlhc/MonteCarlo/FEDRA/nuecc/nuecc_muon1.3E5/b000021/"
NueEvent = int(sys.argv[1])

inputfile = inputpath + "b000021.0.0.{}.trk.root".format(NueEvent+1)

trackfile = uproot.open(inputfile)
tracktree = trackfile["tracks"]

trackinfo = tracktree.arrays(["nseg","npl","s.eFlag","s.eMCEvt","s.eMCTrack","trid","s.eX","s.eY","s.eZ","s.eTX","s.eTY"])

#looking for primary electron
primaryelectron = np.logical_and(trackinfo["s.eMCTrack"]==1,trackinfo["s.eFlag"]==1)
 
primaryelectrons = trackinfo[np.any(primaryelectron,axis=1)]

primaryelectrons["nsegtrue"] = np.sum(np.logical_and(primaryelectrons["s.eMCTrack"]==1,primaryelectrons["s.eFlag"]==1),axis=1)

upstreamtrack = primaryelectrons[np.argmin(primaryelectrons["s.eZ"][:,0])]
upstreamtrack["ntracks_split"] = len(primaryelectrons) #the primary electron was splitted into multiple tracks

outputpath = "/afs/cern.ch/work/a/aiulian/public/revertexing_addtracks/b000021/"

#storing info in output file
file = uproot.recreate(outputpath+"primaryelectroninfo_files/primaryelectroninfo_{}.root".format(NueEvent+1))
outputdata = upstreamtrack.to_list()
file.mktree("treeexample",awkward.zip(outputdata))



#electroninfo = trackinfo[trackinfo["s.eFlag"] == 1]
#electroninfo = electroninfo[electroninfo["s.eMCTrack"]==1]
#print(electroninfo["nseg"])