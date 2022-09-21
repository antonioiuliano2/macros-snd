import ROOT as r
from array import array
import fedrarootlogon

#importing geometry file
r.gGeoManager.Import("geofile_sndlhc_TI18.root")
#getting class instance
emureader = r.EmulsionDet()

def convertpos(testseg,emureader):
 '''convert position from an EdbSegP into the global system'''
 detID = int(brickID * 1e3 + testseg.Plate())
 print("reading film in ",detID, "with positions ",testseg.X(),testseg.Y(),testseg.Z(),"angles ",testseg.TX(),testseg.TY())

 localarr = array('d',[testseg.X(), testseg.Y(), 0.])
 globalarr = array('d',[0., 0., 0.])

 emureader.GetPosition(detID,localarr,globalarr)

 print("we got positions",globalarr[0],globalarr[1],globalarr[2])

 anglocalarr = array('d',[testseg.TX(), testseg.TY(), 1.])
 angglobalarr = array('d',[0., 0., 0.])

 emureader.GetAngles(detID,anglocalarr,angglobalarr)
 print("we got angles",angglobalarr[0],angglobalarr[1],angglobalarr[2])


 return globalarr

brickID = 31 #our name
#initializing empty reference system
dproc = r.EdbDataProc()
ali = r.EdbPVRec()
scancond = r.EdbScanCond()
ali.SetScanCond(scancond)

dproc.ReadTracksTree(ali,"tracks_11plates_smallarea.root","nseg>10")

tracklist = ali.eTracks



graphlocal = r.TGraph()
graphglobal = r.TGraph()

for itrack in range(100):
    track = tracklist.At(itrack)
    nseg = track.N()
    print("new track")

    for iseg in range(nseg):
        myseg = track.GetSegment(iseg)
        graphlocal.AddPoint(myseg.Z(),myseg.Y())
        globalarr = convertpos(myseg,emureader)
        graphglobal.AddPoint(globalarr[2],globalarr[1])

cloc = r.TCanvas()
graphlocal.SetTitle("Track position in local ref system;z[#mu m];y[#mu m]")
#graphlocal.SetMarkerStyle(20)
graphlocal.Draw("AP*")
cglob = r.TCanvas()
graphglobal.SetTitle("Track position in global ref system;z[cm];y[cm]")
#graphglobal.SetMarkerStyle(20)
graphglobal.Draw("AP*")
