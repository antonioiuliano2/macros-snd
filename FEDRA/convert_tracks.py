import ROOT as r
import Fedra2sndsw as EmuConv
import numpy as np
import fedrarootlogon

gemuzx = r.TGraphErrors()
gemuzy = r.TGraphErrors()

#importing geometry file
EmuConv.importgeofile("geofile_sndlhc_TI18.root")

brickID = 31 #our name
#initializing empty reference system
dproc = r.EdbDataProc()
ali = r.EdbPVRec()
scancond = r.EdbScanCond()
ali.SetScanCond(scancond)

dproc.ReadTracksTree(ali,"tracks_11plates_smallarea.root","nseg>10")

tracklist = ali.eTracks

for itrack in range(100):
    track = tracklist.At(itrack)
    globaltrackarr = EmuConv.converttrack(track,brickID,60)
    #track.PrintNice()
    #print("print track {} in global reference".format(itrack))
    #np.set_printoptions(precision=4,suppress=True)
    #print(globaltrackarr)

    for x,y,z,tx,ty,tz in globaltrackarr:
      gemuzx.AddPoint(z,x)
      gemuzy.AddPoint(z,y)

czx = r.TCanvas()
gemuzx.SetTitle("Segment position in global reference system;z[cm];x[cm]")
gemuzx.Draw("AP*")

czy = r.TCanvas()
gemuzy.SetTitle("Segment position in global reference system;z[cm];y[cm]")
gemuzy.Draw("AP*")
