'''vertextree useful functions, using uproot'''

import uproot, awkward
import pandas as pd
import numpy as np
import sys

'''main function, reading vertex tree'''

#reading reconstructed vertices
vtxfile = uproot.open(sys.argv[1])
vtxtree = vtxfile["vtx"]
vtxinfo = vtxtree.arrays(["n","MCTrackPdgCode","MCEventID","MCTrackID","MCMotherID","vx","vy","vz","vID","incoming","flag","TrackID"])

#adding information about brickID
brickinfo = vtxfile["brickinfo"]
vtxinfo["brickID"] = brickinfo["brickID"].array()

ndaughters = []

def count_daughters():
 '''checking how many daughters'''
 for ntracks, incoming in zip(vtxinfo["n"],vtxinfo["incoming"]):
    nd = 0
    for itrk in range(ntracks):
        if(incoming[itrk]==1):
            nd = nd + 1
    ndaughters.append(nd)
 return ndaughters

count_daughters()
vtxinfo["ndaughters"] = ndaughters
#we select only vertices with at least 2 daughters
twodaughtersvtx = vtxinfo[vtxinfo["ndaughters"]>=2]

def findlinkedvertices(myvertexID):
 '''find linked vertices, returns two lists: linkedvertices and trackIDs of linking'''
 myvertex = vtxinfo[vtxinfo["vID"]==myvertexID]

 linkedverticesIDs = []

 othervertices = vtxinfo[vtxinfo["vID"]!= myvertexID]
 for trackid in myvertex["TrackID"][0]:
  islinkedvertex = []
  for othervertextracks in othervertices["TrackID"]:
   islinkedvertex.append(trackid in othervertextracks)
  linkedverticesIDs.append(othervertices[islinkedvertex]["vID"])
 return linkedverticesIDs, myvertex["TrackID"][0]
