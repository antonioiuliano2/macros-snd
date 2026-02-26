import ROOT as r
import pandas as pd
import numpy as np

#read the RNtuple
#rdf = r.RDataFrame("tracks_NTuple","volumetracks_rdataframe.root")
#read the TTree and convert it on the fly, no intermediate file
rdf = r.RDataFrame("vtx","b000006.0.0.0.vtx.root")
#convert into a pandas dataframe
df = pd.DataFrame(rdf.AsNumpy(columns=["vID","n","TrackID","incoming"]))
#explode into 1D (i.e. each row is a track, not a vertex)
#everything related to tracks and not vertex
df_exploded = df.explode(["TrackID","incoming"]) 
