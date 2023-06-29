#!/usr/bin/env python
# coding: utf-8

# # Evaluate tracking efficiency from csv file
# 
# We want to compare the provided base tracks of each MCTrack with the reconstructed track
# 
# The functions were initially used in FOOT

# usage: python trackingquality.py foldercontainingbricks

import sys


import pandas as pd
import matplotlib.pyplot as plt
import seaborn #for regplot, equivalent as tprofile

minnseg = 31

simdf = pd.DataFrame()
#brickIDs = [11,12,13,14,21,22,23,24,25,31,32,33,34,41,42,43,44,51,52,53,54]
brickIDs = [11]

prepath = sys.argv[1]


for brID in brickIDs:
    singledf = pd.read_csv(prepath+"/b0000{}/brick{}.csv".format(brID,brID))
    singledf["brickID"] = brID
    #singledf = singledf.query(signalselection) #if we want to see only primary tracks
    simdf = pd.concat([simdf,singledf],axis=0)
#simdf = pd.read_csv("/eos/user/a/aiulian/sim_fedra/SNDnuyield_17_November_2021/b000032/brick32.csv")

simdf.head()


# Counting segments associated to the reconstructed track and to true MCTracks 


nseg = simdf.groupby(["brickID","FEDRATrackID"]).count()["ID"] #number of segments associated to each reconstructed track
nsegsamemc = simdf.groupby(["brickID","FEDRATrackID","MCEvent","MCTrack"]).count()["PID"] #associated to the true MC track


# ## Information for tracked segment only


#taking tracked segments
trackdf = simdf.query("FEDRATrackID>=0")
#computing npl (expected number of segments, assuming one segment for plate)
PIDlast = trackdf.groupby("FEDRATrackID").min()["PID"]
PIDfirst = trackdf.groupby("FEDRATrackID").max()["PID"]
npl = (PIDfirst - PIDlast) + 1

#For each track, take the last segment, accept them if they have at least one segment Monte Carlo
trackdf["simulation"] = trackdf["MCEvent"]>=0 
atleastonemc = trackdf.groupby(["brickID","FEDRATrackID"]).any()["simulation"] #at least one segment coming from simulation

#which are the most frequent MonteCarlo Event and Track for this reconstructed track?
#mostfrequent = trackdf.query("MCEvent>=0").groupby(["brickID",'FEDRATrackID'])['MCEvent','MCTrack'].agg(lambda x:x.value_counts().index[0])

#retrieving most frequent pair of (MCEvent, MTrack) for each FEDRATrackID
mostfrequentpair = simdf.groupby(["brickID","FEDRATrackID"])[["MCEvent","MCTrack"]].apply(lambda x: x.value_counts().index[0]) 

print("replacing MCEvent and MCTrack values in trackdf with information from most common MCEvent and MCTrack")

del trackdf["MCEvent"]
del trackdf["MCTrack"]

#keeping only information about first segment

trackdf = trackdf.groupby(["brickID","FEDRATrackID"]).last()

trackdf["MCEvent"] = [index[0] for index in mostfrequentpair]
trackdf["MCTrack"] = [index[1] for index in mostfrequentpair]


#how many reconstructed track for each true track
nsplit = trackdf.groupby(["brickID","MCEvent","MCTrack"]).count()["ID"]


#hsplit = r.TH1D("hsplit","nsplit",8,0,8)
#root_numpy.fill_hist(hsplit,nsplit)
#csplit = r.TCanvas()
#hsplit.Draw()

plt.figure()
plt.hist(nsplit.values, range = (0,8),bins=8)



#adding computed information to our dataset
trackdf["nseg"] = nseg
trackdf["npl"] = npl
trackdf["fedraeff"] = nseg/npl

#group by same MCTrack and MCEvent
simdf = simdf.query("MCEvent>=0")
nsegtrue = simdf.groupby(["brickID","MCEvent","MCTrack"]).count()["ID"] #true length of the Monte Carlo track

nsegtrue = nsegtrue.reset_index()
nsegsamemc = nsegsamemc.reset_index()

trackdf = trackdf.reset_index().merge(nsegtrue, how = "left", on = ["brickID","MCEvent","MCTrack"])
trackdf = trackdf.merge(nsegsamemc, how = "left", on = ["brickID","FEDRATrackID","MCEvent","MCTrack"])


#renaming labels
trackdf["PID"] = trackdf["PID_x"]
trackdf["ID"] = trackdf["ID_x"]
trackdf["nsegsamemc"] = trackdf["PID_y"]
trackdf["nsegtrue"] = trackdf["ID_y"]

del trackdf["ID_y"]
del trackdf["ID_x"]
del trackdf["PID_y"]
del trackdf["PID_x"]


# ## computing efficiencies


#computing efficiency
trackdf["efficiency"] = trackdf["nsegsamemc"]/trackdf["nsegtrue"]

trackdf.head()


longtracks = trackdf.query("nsegtrue>{}".format(minnseg))

plt.figure()
plt.hist(longtracks["efficiency"],bins=11,range=(0,1.1))


# ## Using root_numpy libraries to fill ROOT histograms with numpy objects 
# 
# Let us start to make a TProfile angle vs efficiency


import numpy as np
longtracks["Theta"] = np.sqrt(longtracks["TX"]**2 + longtracks["TY"]**2)


#profefftheta = r.TProfile("profefftheta","Efficiency for different theta angles;theta[rad];efficiency",11,0,1.1,0.,1.1)

#rootnumpy_myutils.fillprofile2D(profefftheta,longtracks["Theta"],longtracks["efficiency"])

#cefftheta = r.TCanvas()
#profefftheta.Draw()
#cefftheta.Draw()

plt.figure()
seaborn.regplot(x=longtracks["Theta"],y=longtracks["efficiency"],x_bins=11,marker='o',fit_reg=False)

#profeffP = r.TProfile("profP","Efficiency for different momenta;P[GeV/c];efficiency",20,0,1000.,0.,1.1)

#rootnumpy_myutils.fillprofile2D(profeffP,longtracks["P"],longtracks["efficiency"])

plt.figure()
seaborn.regplot(x=longtracks["P"],y=longtracks["efficiency"],x_bins=20,marker='o',fit_reg=False)
plt.title("Efficiency for different momenta of tracks with at least {}".format(minnseg))

singletrackdf = trackdf.groupby(["brickID","MCEvent","MCTrack"]).first()
singletrackdf["nsplit"] = nsplit
#profeffnsplit = r.TProfile("profnsplit","Efficiency for different nsplit;nsplit;efficiency",8,0,8,0.,1.1)

#rootnumpy_myutils.fillprofile2D(profeffnsplit,singletrackdf["nsplit"],singletrackdf["efficiency"])

plt.figure()
seaborn.regplot(x=singletrackdf["nsplit"],y=singletrackdf["efficiency"],x_bins=8,marker='o',fit_reg=False)
plt.title("Efficiency counted to single MCTrack")
#ceffnsplit = r.TCanvas()
#profeffnsplit.Draw()
#ceffnsplit.Draw()

#hsplit_mu = r.TH1D("hsplit_mu","nsplit",8,0,8)
#root_numpy.fill_hist(hsplit_mu,singletrackdf.query("MCTrack==0")["nsplit"])
#cnpslit_mu = r.TCanvas()
#hsplit_mu.Draw()

plt.figure()
plt.hist(singletrackdf.query("MCTrack==0")["nsplit"].values, range = (0,8),bins=8)
plt.title("nsplit for muon tracks")
plt.show()



