'''mini script to read TTree produced from stuyMCinteractions.C with PyROOT'''

import ROOT as r
import uproot, awkward
import root_numpy
import numpy as np
import rootnumpy_myutils
import readvertextree
import pandas as pd

compare_withreco = True
global df
global nurecodf
global nutruedf
global totaldf

if (compare_withreco):
 df = pd.read_csv("/home/utente/Simulations/sim_snd/nutracks_files/SNDnuyield_17_November_2021/brickeventlist.csv")
 df = df.query("NBrick>0")
inputchain = r.TChain("sndhits")
#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/anumu_sim_activeemu_8_September_2021/nutracks_SND.root")
#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_7_September_2021/nutracks_SND.root")

#inputchain.Add("/home/utente/Simulations/sim_snd/backup_sim_neutrino_3_November/10/nutracks_SND.root")
inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_10_November_2021/nutracks_SND.root")
inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/anumu_sim_activeemu_15_November_2021/nutracks_SND.root")
inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/nue_sim_activeemu_12_November_2021/nutracks_SND.root")
inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/anue_sim_activeemu_15_November_2021/nutracks_SND.root")

#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/nue_sim_activeemu_10_September_2021/nutracks_SND.root")
hnu_p = r.TH1D("hnu_p","Neutrino momentum;P[GeV/c]",500,0,5000)
hdau_p = r.TH1D("hdau_p","All daughters momentum;P[GeV/c]",500,0,5000)
hl_p = r.TH1D("hl_p","primary lepton momentum;P[GeV/c]",500,0,5000)

hnprimary = r.TH1D("hnprimary","number of tracks at primary vertex from MC simulation;Ntracks",50,0,50)
hdaupdg = r.TH1D("hdaupdg","PdgCode of neutrinod aughters",6000,-3000,3000)

hnprimary_reco = r.TH1D("hnprimary_reco","reconstructed tracks at primary vertex;Ntracks",50,0,50)

ndau_pdg = {}

flavour_multiplier = 1e+5

nprimary_event = {}
for event in inputchain:
    iflavour = event.GetTreeNumber()
    if compare_withreco:
     if (event.ievent+iflavour*flavour_multiplier not in df["Event"].values):
      continue
    nprimary = 0
    hnu_p.Fill(r.TMath.Sqrt(event.nu_px **2 + event.nu_py **2 + event.nu_pz**2))
    for idau,(dau_px, dau_py, dau_pz, dau_pdg, dau_charge) in enumerate(zip(event.dau_px, event.dau_py, event.dau_pz, event.dau_pdg, event.dau_charge)):

      dau_p = r.TMath.Sqrt(dau_px **2 + dau_py **2 + dau_pz**2)
      hdau_p.Fill(dau_p)
      if (dau_pdg in ndau_pdg.keys()): #already present, count + 1
        ndau_pdg[dau_pdg] = ndau_pdg[dau_pdg] + 1
      else:
        ndau_pdg[dau_pdg] = 1
      if ((abs(dau_pdg) == 11 or abs(dau_pdg) == 13) and idau == 0):
        hl_p.Fill(dau_p) #only for lepton
      if (abs(dau_charge) > 0 and dau_p > 0.1):
          nprimary = nprimary + 1
      hdaupdg.Fill(dau_pdg)
    hnprimary.Fill(nprimary)
    nprimary_event[event.ievent+iflavour*flavour_multiplier] = nprimary

global nuvertices
if compare_withreco:
 nuvertices = readvertextree.readvertextree()
 # I want only the vertex with highest multiplicity for this event
 eventIDs = nuvertices["MCEventID"][:,0]
 ndaughters = nuvertices["ndaughters"]
 nurecodf = pd.DataFrame({"eventID":eventIDs,"ndaughters":ndaughters})
 nurecodf = nurecodf.sort_values(["eventID","ndaughters"])
 nurecodf = nurecodf.groupby("eventID").last() 
 nurecodf = nurecodf.reset_index()

 #merging two dataframes, to compare number of daughters
 nutruedf = pd.DataFrame({"eventID":list(nprimary_event.keys()),"ntruedaughters":list(nprimary_event.values())},dtype = int)
 totaldf = pd.merge(nutruedf,nurecodf) 
 #plotting the results
 rootnumpy_myutils.fillhist1D(hnprimary_reco,nurecodf["ndaughters"])

cp = r.TCanvas()
cp.Divide(1,2)
cp.cd(1)
hnu_p.Draw()
cp.cd(2)
hdau_p.Draw()
hl_p.SetLineColor(r.kRed)
hl_p.Draw("SAMES")
cp.GetPad(2).BuildLegend()


cn = r.TCanvas()
hnprimary.Draw()
hnprimary_reco.SetLineColor(r.kRed)
if compare_withreco:
  hnprimary_reco.Draw("SAMES")
cn.BuildLegend()

cpdg = r.TCanvas()
hdaupdg.Draw()