import fedra2scipy_utils
import sys
import pandas as pd
import ROOT as r

def applyconversion(nbrick):
 '''convert couples ROOT files into a csv'''

 df = fedra2scipy_utils.builddataframe(nbrick)
 df = fedra2scipy_utils.addtrackindex(df,"linked_tracks.root")
 dfvertices = fedra2scipy_utils.addvertexindex(df,"vertextree.root")

 return df, dfvertices 

#the two steps can now be done together, without an intermediate file
nbrick = 21
df,dfvertices = applyconversion(nbrick)

#df = df.drop(columns = ["P","Flag"])
#simfile = r.TFile.Open(sys.argv[2])
#df = desy19.addtrueMCinfo(df,simfile, True)
df.to_csv('brick{}.csv'.format(nbrick),index=False)
dfvertices.to_csv('brick{}_vertices.csv'.format(nbrick),index=False)


