'''mini script to read TTree produced from stuyMCinteractions.C with PyROOT'''

import ROOT as r
import uproot, awkward
import root_numpy
import numpy as np
import rootnumpy_myutils
import readvertextree
import pandas as pd

compare_withreco = True
vertexfile = "/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_10_November_2021/vertextree_allbricks.root"
brickeventlist = "/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_10_November_2021/brickeventlist.csv"

#vertexfile = "/home/utente/Simulations/sim_snd/nutracks_files/SNDnuyield_17_November_2021/vertextree_allbricks.root"
#brickeventlist = "/home/utente/Simulations/sim_snd/nutracks_files/SNDnuyield_17_November_2021/brickeventlist.csv"
global df
global nurecodf
global nutruedf
global totaldf

if (compare_withreco):
 df = pd.read_csv(brickeventlist)
 df = df.query("NBrick>0")
 #renaming column
 df["eventID"] = df["Event"]
 del df["Event"]
inputchain = r.TChain("sndhits")
#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/anumu_sim_activeemu_8_September_2021/nutracks_SND.root")
#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_7_September_2021/nutracks_SND.root")

#inputchain.Add("/home/utente/Simulations/sim_snd/backup_sim_neutrino_3_November/10/nutracks_SND.root")


inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_10_November_2021/nutracks_SND.root")
#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/anumu_sim_activeemu_15_November_2021/nutracks_SND.root")
#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/nue_sim_activeemu_12_November_2021/nutracks_SND.root")
#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/anue_sim_activeemu_15_November_2021/nutracks_SND.root")

#inputchain.Add("/home/utente/Simulations/sim_snd/nutracks_files/nue_sim_activeemu_10_September_2021/nutracks_SND.root")

# declaring root histograms
hnu_p = r.TH1D("hnu_p","Neutrino momentum;P[GeV/c]",500,0,5000)
hdau_p = r.TH1D("hdau_p","All daughters momentum;P[GeV/c]",500,0,5000)
hl_p = r.TH1D("hl_p","primary lepton momentum;P[GeV/c]",500,0,5000)

hnprimary = r.TH1D("hnprimary","number of tracks at primary vertex from MC simulation;Ntracks",50,0,50)
hdaupdg = r.TH1D("hdaupdg","PdgCode of neutrinod aughters",6000,-3000,3000)

hnprimary_reco = r.TH1D("hnprimary_reco","reconstructed tracks at primary vertex;Ntracks",50,0,50)

hdeltanprimary = r.TH1D("hdeltanprimary","Difference between true and reco molteplicity;DeltaNPrimary",50,-25,25)


ndau_pdg = {}

flavour_multiplier = 1e+5

nprimary_event = {}
hfirst = r.TH1I("hfirst","Number of hits in first emulsion after vertex;Nhits",100,0,100)
hlast = r.TH1I("hlast","Number of hits in last emulsion film of brick;Nhits",100,0,1000)
hfivedown = r.TH1I("hfivedown","Number of hits in five films after vertex;Nhits",100,0,200)
htendown = r.TH1I("htendown","Number of hits in ten films after vertex;Nhits",100,0,400)

profnemu = r.TProfile("profnemu","Number of hits in different films after vertex;NFilms_aftervertex;Nhits",60,0,60,0,400)
# starting main loop
for event in inputchain:
    iflavour = event.GetTreeNumber()
    if compare_withreco:
     if (event.ievent+iflavour*flavour_multiplier not in df["eventID"].values):
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
    #checking emulsion film hits
    nu_filmID = event.nu_filmID    
    if (nu_filmID >= 0): #only accepting events in bricks
      nemupoints = event.nemupoints
      #filling number of points
      hfirst.Fill(nemupoints[nu_filmID])
      hlast.Fill(nemupoints[59])
      if ((nu_filmID + 5) < 60):
       hfivedown.Fill(nemupoints[nu_filmID+5])
      if ((nu_filmID + 10) < 60):
       htendown.Fill(nemupoints[nu_filmID+10])
      for ifilm in range(nu_filmID,60):
       profnemu.Fill(ifilm - nu_filmID, nemupoints[ifilm])


global nuvertices
if compare_withreco:
 nuvertices = readvertextree.readvertextree(vertexfile,brickeventlist)
 # I want only the vertex with highest multiplicity for this event
 eventIDs = nuvertices["MCEventID"][:,0]
 vIDs = nuvertices["vID"]
 ndaughters = nuvertices["ndaughters"]
 brickIDvertex = nuvertices["brickID"]
 nurecodf = pd.DataFrame({"eventID":eventIDs,"brickIDvertex":brickIDvertex,"vID":vIDs,"ndaughters":ndaughters,"hasmuon":nuvertices["hasmuon"]})
 nurecodf = nurecodf.sort_values(["eventID","ndaughters"])
 nurecodf = nurecodf.groupby("eventID").last() 
 nurecodf = nurecodf.reset_index()

 #merging two dataframes, to compare number of daughters
 nutruedf = pd.DataFrame({"eventID":list(nprimary_event.keys()),"ntruedaughters":list(nprimary_event.values())},dtype = int)
 nutruedf = pd.merge(df,nutruedf)
 totaldf = pd.merge(nutruedf,nurecodf,how='left')
 nurecodf_samebrickID = totaldf.query("NBrick==brickIDvertex") 
 #plotting the results
 rootnumpy_myutils.fillhist1D(hnprimary_reco,nurecodf_samebrickID["ndaughters"])
 totaldf["deltandaughters"] = totaldf["ntruedaughters"] - totaldf["ndaughters"]
 rootnumpy_myutils.fillhist1D(hdeltanprimary,totaldf["deltandaughters"])

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

cdeltan = r.TCanvas()
if compare_withreco:
 hdeltanprimary.Draw()

cpdg = r.TCanvas()
hdaupdg.Draw()

cemuhits = r.TCanvas()
cemuhits.Divide(2,1)
cemuhits.cd(1)
hfirst.Draw()
cemuhits.cd(2)
hlast.Draw()

cemuhits2 = r.TCanvas()
cemuhits2.Divide(2,1)
cemuhits2.cd(1)
hfivedown.Draw()
cemuhits2.cd(2)
htendown.Draw()

cemuprof = r.TCanvas()
profnemu.Draw()