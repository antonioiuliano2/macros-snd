'''small script to add information to vertextree as a friend TNtuple'''

import ROOT as r
import fedrarootlogon
import sys

brickID = int(sys.argv[1])

vtxfile = r.TFile.Open("b0000"+sys.argv[1]+".0.0.0.trk.root","UPDATE")
vtxtree = vtxfile.Get("tracks")

outputtree = r.TNtuple("brickinfo","Information about the brick","trID:brickID")
#main loop, filling additional information
for vtx in vtxtree:
    outputtree.Fill(vtxtree.trid, brickID)

vtxfile.cd()
outputtree.Write("",r.TObject.kOverwrite)
