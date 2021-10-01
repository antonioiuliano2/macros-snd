import desy19_fedrautils as desy19
import sys
import pandas as pd
import ROOT as r

def applyconversion(nbrick):
 '''convert couples ROOT files into a csv'''

 df = desy19.builddataframe(nbrick)
 df = desy19.addtrackindex(df,"linked_tracks.root")

 return df 

#the two steps can now be done together, without an intermediate file

nrun = int(sys.argv[1])

df = applyconversion(nrun)

#df = df.drop(columns = ["P","Flag"])
#simfile = r.TFile.Open(sys.argv[2])
#df = desy19.addtrueMCinfo(df,simfile, True)
df.to_csv('RUN{}.csv'.format(nrun),index=False)


