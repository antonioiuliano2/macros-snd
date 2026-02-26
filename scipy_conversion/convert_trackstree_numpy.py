import ROOT as r
import pandas as pd
import numpy as np

#read the RNtuple
#rdf = r.RDataFrame("tracks_NTuple","volumetracks_rdataframe.root")
#read the TTree and convert it on the fly, no intermediate file
rdf = r.RDataFrame("tracks","b000006.0.0.0.trk.root")
#convert into a pandas dataframe
df = pd.DataFrame(rdf.AsNumpy(columns = ["s.eID","s.ePID","s.eX","s.eY","s.eZ","s.eTX","s.eTY","s.eMCTrack","s.eP","s.eFlag","trid"]))
#renameing them to remove the s.e prefix
df.columns = ["ID","PID","X","Y","Z","TX","TY","MCTrack","P","Flag","trid"]
#explode into 1D (i.e. each row is a segment, not a track)
#everything except trid, which is already one entry per track
df_exploded = df.explode(["ID","PID","X","Y","Z","TX","TY","MCTrack","P","Flag"]) 
