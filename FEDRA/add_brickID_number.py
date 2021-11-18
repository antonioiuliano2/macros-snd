'''small script to add information to vertextree as a friend TNtuple'''

import ROOT as r
import sys

brickID = sys.argv[1]

vtxfile = r.TFIle.Open("vertextree.root","UPDATE")
vtxtree = vtxfile.Get("vtx")

outputtree = r.TNtuple("vtxbrick","vtxbrick","vID:brickID")
#main loop, filling additional information
for vtx in vtxtree:
    outputtree.Fill(vtx.vID, brickID)

vtxfile.cd()
outputtree.Write()