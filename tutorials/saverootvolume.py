import ROOT as r
import sys
#usage: python -i drawrootvolume.py geofile.root
r.TGeoManager.Import(sys.argv[1])
outputfile = r.TFile("detectorandtunnel.root","RECREATE")
mytarget = r.gGeoManager.GetVolume("Detector")
outputfile.cd()
mytarget.Write()
mytarget = r.gGeoManager.GetVolume("Tunnel")
mytarget.Write()
outputfile.Close()
