'''drawing scifi histograms obtained by storeevents'''

import ROOT as r

nuepath = "/afs/cern.ch/work/a/aiuliano/public/sim_snd/nueCCDIS_15_02_2021/2/scifihistos.root" #12 events

numupath = "/afs/cern.ch/work/a/aiuliano/public/sim_snd/numuCCDIS_4000events_22Feb/scifihistos.root" #35 events

inputfile_nue = r.TFile.Open(nuepath)
inputfile_numu = r.TFile.Open(numupath)

hxyfirstscifi_nue = inputfile_nue.Get("hxyscifich[0]")
hxyfirstscifi_numu = inputfile_numu.Get("hxyscifich[0]")
graphxy_nue = inputfile_nue.Get("gvxy_first")

hxyfirstscifi_nue.Add(hxyfirstscifi_numu) 
c1 = r.TCanvas()
hxyfirstscifi_nue.Draw("COLZ")
graphxy_nue.SetMarkerColor(r.kRed)
graphxy_nue.Draw("*SAME")

graphxy_numu = inputfile_numu.Get("gvxy_first")
 
graphxy_numu.SetMarkerColor(r.kBlue)
graphxy_numu.Draw("*SAME")

#max scifi channel

hmaxscifix_nue = inputfile_nue.Get("hmaxscifix")
hmaxscifiy_nue = inputfile_nue.Get("hmaxscifiy")

hmaxscifix_numu = inputfile_numu.Get("hmaxscifix")
hmaxscifiy_numu = inputfile_numu.Get("hmaxscifiy")

hmaxscifix_nue.Add(hmaxscifix_numu)
hmaxscifiy_nue.Add(hmaxscifiy_numu)

c2 = r.TCanvas()
c2.Divide(2,1)
c2.cd(1)
hmaxscifix_nue.Draw()
c2.cd(2)
hmaxscifiy_nue.Draw()
