'''mini script to read TTree produced from stuyMCinteractions.C with PyROOT'''

import ROOT as r
import uproot, awkward
import root_numpy
import numpy as np
import rootnumpy_myutils
import readvertextree
import pandas as pd

compare_withreco = False
vertexfile = "/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_10_November_2021_withinefficiency/vertextree_allbricks.root"
#vertexfile = "/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_10_November_2021_withinefficiency/vertextree_downstream_allbricks.root"
brickeventlist = "/home/utente/Simulations/sim_snd/nutracks_files/numu_sim_activeemu_10_November_2021/brickeventlist.csv"

#vertexfile = "/home/utente/Simulations/sim_snd/nutracks_files/SNDnuyield_17_November_2021/vertextree_allbricks.root"
#brickeventlist = "/home/utente/Simulations/sim_snd/nutracks_files/SNDnuyield_17_November_2021/brickeventlist.csv"
global df
global nurecodf
global nutruedf
global totaldf

#visibility requirements
minmom = 0.1
maxtheta = 1.

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

def isinacceptance_borders(nu_filmID, nu_vx, nu_vy):

  mindistance = [0.4,0.2,0.2,0.4] #middle gap will be only 1 mm in final configuration, therefore I lessen the requirements here

  xborders = [-47.6, -28.1, -27.5, -8.0]
  yborders = [15.5,  35.0, 35.6, 55.5]
  #xborders = [-47.6, -27.8, -8.0]
  #yborders = [-47.6, 35.3, -8.0]
  #first, checking if it is close to any border along x or y
  
  for (distance, xborder) in zip(mindistance,xborders):
    if (abs(nu_vx - xborder) < distance):
      return False

  for (distance, yborder) in zip(mindistance,yborders):
    if (abs(nu_vy - yborder) < distance):
      return False     

  #then, checking if it is near the end of the brick
  ntotfilms = 60
  nminfilms = 3 #number of plates required
  if (nu_filmID > (ntotfilms - (nminfilms-1))):
    return False

  return True

# declaring root histograms
hnu_p = r.TH1D("hnu_p","Neutrino momentum;P[GeV/c]",500,0,5000)
hdau_p = r.TH1D("hdau_p","All daughters momentum;P[GeV/c]",500,0,5000)
hl_p = r.TH1D("hl_p","primary lepton momentum;P[GeV/c]",500,0,5000)

hdau_txty = r.TH2D("hdau_txty","All daughters angle;TX;TY",300,-1.5,1.5,300,-1.5,1.5)

hnprimary = r.TH1D("hnprimary","number of tracks at primary vertex from MC simulation;Ntracks",50,0,50)
hdaupdg = r.TH1D("hdaupdg","PdgCode of neutrinod aughters",6000,-3000,3000)

hnprimary_reco = r.TH1D("hnprimary_reco","reconstructed tracks at primary vertex;Ntracks",50,0,50)
hmolt2D = r.TH2D("hmolt2D","reco molt vs true molt;True Molteplicity;Reco Molteplicity",50,0,50,50,0,50)

hdeltanprimary = r.TH1D("hdeltanprimary","Difference between true and reco molteplicity;DeltaNPrimary",50,-25,25)

#position all vs found
hvzall = r.TH1D("hvzall","vz of all considered interactions;vz[cm]",50,-25,25)
hvzmissing = r.TH1D("hvzmissing","vz of missing vertices;vz[cm]",50,-25,25)

hvxyall = r.TH2D("hvxyall","vxy of all considered interactions;vx[cm];vy[cm]",120,-60,0,120,0,60)
hvxymissing = r.TH2D("hvxymissing","vxy of missing vertices;vx[cm];vy[cm]",120,-60,0,120,0,60)

ndau_pdg = {}

flavour_multiplier = 1e+5
#information to be stored in true dataframe
nprimary_event = {}
nuvx_event = []
nuvy_event = []
nuvz_event = []
nuacceptance_event = []

hfirst = r.TH1I("hfirst","Number of hits in first emulsion after vertex;Nhits",100,0,100)
hlast = r.TH1I("hlast","Number of hits in last emulsion film of brick;Nhits",100,0,1000)
hfivedown = r.TH1I("hfivedown","Number of hits in five films after vertex;Nhits",100,0,200)
htendown = r.TH1I("htendown","Number of hits in ten films after vertex;Nhits",100,0,400)

profnemu = r.TProfile("profnemu","All particles hits;NFilms_aftervertex;Nhits",60,0,60,0,400)
profnemu_electrons = r.TProfile("profnemu_electrons","Electron hits;NFilms_aftervertex;Nhits",60,0,60,0,400)
profnemu_others = r.TProfile("profnemu_others","Other particle hits;NFilms_aftervertex;Nhits",60,0,60,0,400)
# starting main loop
for event in inputchain:
    iflavour = event.GetTreeNumber()
    if compare_withreco:
     if (event.ievent+iflavour*flavour_multiplier not in df["eventID"].values):
      continue
    nprimary = 0
    hnu_p.Fill(r.TMath.Sqrt(event.nu_px **2 + event.nu_py **2 + event.nu_pz**2))

    nu_tx = event.nu_px/event.nu_pz
    nu_ty = event.nu_py/event.nu_pz

    if compare_withreco == False:
      #only true position xy
      hvxyall.Fill(event.nu_vx,event.nu_vy)

    #starting loop over daughters
    for idau,(dau_px, dau_py, dau_pz, dau_pdg, dau_charge) in enumerate(zip(event.dau_px, event.dau_py, event.dau_pz, event.dau_pdg, event.dau_charge)):

      dau_p = r.TMath.Sqrt(dau_px **2 + dau_py **2 + dau_pz**2)
      dau_tx = dau_px/dau_pz
      dau_ty = dau_py/dau_pz
      dau_theta = r.TMath.ATan(r.TMath.Sqrt(dau_tx **2 + dau_ty **2))
      hdau_p.Fill(dau_p)
      hdau_txty.Fill(dau_tx,dau_ty)
      if (dau_pdg in ndau_pdg.keys()): #already present, count + 1
        ndau_pdg[dau_pdg] = ndau_pdg[dau_pdg] + 1
      else:
        ndau_pdg[dau_pdg] = 1
      if ((abs(dau_pdg) == 11 or abs(dau_pdg) == 13) and idau == 0):
        hl_p.Fill(dau_p) #only for lepton
      if (abs(dau_charge) > 0 and dau_p > minmom and dau_theta < maxtheta):
          nprimary = nprimary + 1
      hdaupdg.Fill(dau_pdg)
    hnprimary.Fill(nprimary)
    #checking emulsion film hits
    nu_filmID = event.nu_filmID   

    #only accepting events in bricks
    if (nu_filmID >= 0): 

      #storing info for dataframe later
      nprimary_event[event.ievent+iflavour*flavour_multiplier] = nprimary
      nuvx_event.append(event.nu_vx)
      nuvy_event.append(event.nu_vy)
      nuvz_event.append(event.nu_vz)
      #checking if event is in acceptance
      nuacceptance_event.append(isinacceptance_borders(nu_filmID,event.nu_vx,event.nu_vy))

      nemupoints = event.nemupoints
      nemupoints_electrons = event.nemupoints_electrons
      nemupoints_others = event.nemupoints_others
      #filling number of points
      hfirst.Fill(nemupoints[nu_filmID])
      hlast.Fill(nemupoints[59])
      if ((nu_filmID + 5) < 60):
       hfivedown.Fill(nemupoints[nu_filmID+5])
      if ((nu_filmID + 10) < 60):
       htendown.Fill(nemupoints[nu_filmID+10])
      for ifilm in range(nu_filmID,60):
       profnemu.Fill(ifilm - nu_filmID, nemupoints[ifilm])
       profnemu_electrons.Fill(ifilm - nu_filmID, nemupoints_electrons[ifilm])
       profnemu_others.Fill(ifilm - nu_filmID, nemupoints_others[ifilm])


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
 nutruedf = pd.DataFrame({"eventID":list(nprimary_event.keys()),"ntruedaughters":list(nprimary_event.values()),
                          "nu_vx":nuvx_event,"nu_vy":nuvy_event,"nu_vz":nuvz_event, "inacceptance":nuacceptance_event},dtype = int)
 nutruedf = pd.merge(df,nutruedf)
 totaldf = pd.merge(nutruedf,nurecodf,how='left')
 nurecodf_samebrickID = totaldf.query("NBrick==brickIDvertex") 
 #plotting the results
 rootnumpy_myutils.fillhist1D(hnprimary_reco,nurecodf_samebrickID["ndaughters"])

 totaldfreco = totaldf[totaldf.notna()["ndaughters"]] 

 totaldfreco["deltandaughters"] = totaldfreco["ntruedaughters"] - totaldfreco["ndaughters"]

 rootnumpy_myutils.fillhist2D(hmolt2D,totaldfreco["ntruedaughters"],totaldfreco["ndaughters"])
 rootnumpy_myutils.fillhist1D(hdeltanprimary,totaldfreco["deltandaughters"])

 #all vs missing
 totaldfmissing = totaldf[totaldf.isna()["ndaughters"]] 
 rootnumpy_myutils.fillhist1D(hvzall,totaldf["nu_vz"])
 rootnumpy_myutils.fillhist1D(hvzmissing,totaldfmissing["nu_vz"])

 rootnumpy_myutils.fillhist2D(hvxyall,totaldf["nu_vx"],totaldf["nu_vy"])
 rootnumpy_myutils.fillhist2D(hvxymissing,totaldfmissing["nu_vx"],totaldfmissing["nu_vy"])

# START DRAWING HISTOGRAMS
cp = r.TCanvas()
cp.Divide(1,2)
cp.cd(1)
hnu_p.Draw()
cp.cd(2)
hdau_p.Draw()
hl_p.SetLineColor(r.kRed)
hl_p.Draw("SAMES")
cp.GetPad(2).BuildLegend()

cdautxty = r.TCanvas("cdautxty","daughters angles 2D",800,800)
hdau_txty.Draw("COLZ")


cmolt2D = r.TCanvas("cmolt2D","Migration matrix of molteplicity",800,800)
hmolt2D.Draw("COLZ TEXT")

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
profnemu_electrons.SetLineColor(r.kRed)
profnemu_electrons.Draw("SAMES")
profnemu_others.SetLineColor(r.kMagenta)
profnemu_others.Draw("SAMES")
cemuprof.BuildLegend()

cvzpos = r.TCanvas()
hvzall.Draw()
hvzmissing.SetLineColor(r.kRed)
hvzmissing.Draw("SAMES")

cvxypos = r.TCanvas("cxvypos","Comparison of xy vertex position",800,800)
hvxyall.Draw()
if (compare_withreco):
 hvxymissing.SetMarkerColor(r.kRed)
 hvxymissing.Draw("SAMES && *")