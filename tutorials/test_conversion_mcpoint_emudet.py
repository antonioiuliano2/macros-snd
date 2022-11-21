import ROOT as r
import fedrarootlogon
import Fedra2sndsw as EmuConv

inputfile = r.TFile.Open("root:://eosuser.cern.ch//eos/user/a/aiulian/sim_snd/numu_sim_activeemu_withcrisfiles_25_July_2022/1/sndLHC.Genie-TGeant4.root")
r.gGeoManager.Import("root:://eosuser.cern.ch//eos/user/a/aiulian/sim_snd/numu_sim_activeemu_withcrisfiles_25_July_2022/1/geofile_full.Genie-TGeant4.root")

inputtree = inputfile.Get("cbmsim") 
hxy = r.TH2D("hxy","XY emulsion film converted;x[mm];y[mm]",195,0,195,195,0,195)
hzy = r.TH2D("hzy","ZY emulsion film converted;z[mm];y[mm]",100,-1e-3,1e-3,195,0,195)

#nentries = inputtree.GetEntries()
nentries = 1
brickID = 31
print ("start loop over ",nentries)
for ientry in range(nentries):
 inputtree.GetEntry(ientry)
 emupoints = inputtree.EmulsionDetPoint


 for emupoint in emupoints:
  print("original angles", emupoint.GetPx()/emupoint.GetPz(), emupoint.GetPy()/emupoint.GetPz())
  emuseg = EmuConv.convertmcpoint(emupoint,ientry)
  print("converted angles ", emuseg.TX(), emuseg.TY(), 1)
  hxy.Fill(emuseg.X()*1e-3,emuseg.Y()*1e-3)
  hzy.Fill(emuseg.Z()*1e-3,emuseg.Y()*1e-3)

  globalsegarr = EmuConv.convertseg(emuseg,brickID)
  print("converted angles back ",globalsegarr[3]/globalsegarr[5], globalsegarr[4]/globalsegarr[5])
  print("converted angles back (no division)",globalsegarr[3], globalsegarr[4], globalsegarr[5])
  print("\n ")

czy = r.TCanvas()
hzy.Draw("COLZ")

cxy = r.TCanvas()
hxy.Draw("COLZ")
