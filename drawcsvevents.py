import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import dfeventdisplay

brickID = 11
MCEventID = 1
df = pd.read_csv("/home/simsndlhc/Simulations_sndlhc/sim_fedra/muonbkg_1E2cm2/b0000{}/brick{}.csv".format(brickID,brickID))
trackeddf = df.query("FEDRATrackID>=0")
dfevent = df.query("MCEvent=={}".format(MCEventID))

def plotevent(MCEventID):
 '''plotting MCEventID, all segments with angles'''
 figxz,axisxz = plt.subplots()
 figyz,axisyz = plt.subplots()
 dfeventdisplay.displayevent(df,MCEventID,"b",axisyz,axisxz)
 dfeventdisplay.displayevent(trackeddf,MCEventID,"r",axisyz,axisxz)

 plt.show()
