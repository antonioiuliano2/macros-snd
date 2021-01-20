'''Reconstrucing neutrino interaction energy (total for nu_e, hadron only for nu_mu) from hits in SciFi and Muon Filter. Original code from A.Buonaura, start working on it from 2 December 2020 '''
from __future__ import print_function
from __future__ import division
# example for accessing smeared hits and fitted tracks
import ROOT as r
import os,sys,getopt
import rootUtils as ut
import shipunit as u
from ShipGeoConfig import ConfigRegistry
from rootpyPickler import Unpickler
from decorators import *
import shipRoot_conf
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from array import array
sys.path.append('../')
from Functions import *

PDG = ROOT.TDatabasePDG.Instance()

#path2dir = '/eos/user/a/annship/Tungsten/numu/CCDIS/'
#directories0 = ['0to5E3/','5E3to10E3/'] 
nentries = 20315
trainingfraction = 1/5
numpyseed = 1 #for numpy random
path2dir='/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/nueCCDIS_28_11_2020/'
EvtID = []
Ehad = []
nhitScifiTOT = []
nhitMuFilterTOT = []
shipRoot_conf.configure()

def GetEhadAndNhits(directories):
 for i,directory in enumerate(directories):
  inputfile = path2dir+directory+'AcceptedEvts.root'
  geofile =  path2dir+directory+'geofile_full.conical.Genie-TGeant4.root'

  f = r.TFile(inputfile,'READ')
  fgeo = r.TFile(geofile,'READ')
  # new geofile, load Shipgeo dictionary written by run_simScript.py
  upkl    = Unpickler(fgeo)
  ShipGeo = upkl.load('ShipGeo')

  t = f.Get('cbmsim')

  nentries = t.GetEntries()

  nScifi = 6
  nMuFilter = 8

  for ev in range(nentries):
   t.GetEntry(ev)
   evID = t.MCEventHeader.GetEventID()
   EvtID.append(evID+i*5000)
   Ehad.append(GetHadronSystemEnergy(t,ev))

   nhitScifi = [0]*nScifi
   nhitMuFilter = [0]*nMuFilter
   for hit in t.ScifiPoint:
    detID = hit.GetDetectorID()
    p, px, py, pz = GetHitP(hit)
    #if hit.GetTrackID()>0 and p>0.01:
    pdg = hit.PdgCode()
    if pdg<100000000:
     charge = PDG.GetParticle(pdg).Charge()/3.
    else:
     charge =1
    if p>0.01 and charge!=0:
     nhitScifi[detID]+=1
   for hit in t.MuFilterPoint:
    detID = hit.GetDetectorID()
    p, px, py, pz = GetHitP(hit)
    #if hit.GetTrackID()>0 and p>0.01:
    pdg = hit.PdgCode()
    if pdg<100000000:
     charge = PDG.GetParticle(pdg).Charge()/3.
    else:
     charge =1
    if p>0.01 and charge!=0:
     nhitMuFilter[detID]+=1
  
   nhitSF=0
   nhitMuF= 0
   for nhit in nhitScifi: nhitSF += nhit
   for nhit in nhitMuFilter: nhitMuF += nhit
   nhitScifiTOT.append(nhitSF)
   nhitMuFilterTOT.append(nhitMuF)
 return Ehad, nhitScifiTOT, nhitMuFilterTOT

def ReadHitsTree(inputfile,entries):
 """ I have put the nhits directly in a tree""" 
 f = r.TFile.Open(inputfile,"READ")
 t = f.Get("sndhits")
 print(" Start loop over {} entries".format(len(entries)))
 #nentries = t.GetEntries()
 for ev in entries:
  t.GetEntry(ev)
  #get number of hits
  nhitScifiTOT.append(t.nscifihits)
  nhitMuFilterTOT.append(t.nmufilterhits)
  #get neutrino energy 
  nuE = r.TMath.Sqrt(t.nu_px * t.nu_px + t.nu_py * t.nu_py + t.nu_pz * t.nu_pz)
  Ehad.append(nuE)
 #end of loop returning output
 return Ehad, nhitScifiTOT, nhitMuFilterTOT

#Ehad, nhitScifiTOT, nhitMuFilterTOT = GetEhadAndNhits(directories0)
entrylist = np.linspace(nentries,0,nentries-1,dtype=int)
np.random.seed(numpyseed)
np.random.shuffle(entrylist) #randomly shuffle it
Ehad, nhitScifiTOT, nhitMuFilterTOT = ReadHitsTree(path2dir+"nuhits_SND_total_nuanu.root",entrylist[:int(nentries *trainingfraction)])

def CreateElementsForLinearRegression(Ehad,nhitScifiTOT,nhitMuFilterTOT):
 y = np.array(Ehad, dtype=np.float64).reshape(len(Ehad),1)
 x0 = np.ones((len(Ehad),1))
 x1 = np.array(nhitScifiTOT, dtype=np.float64).reshape(len(Ehad),1)
 x2 = np.array(nhitMuFilterTOT, dtype=np.float64).reshape(len(Ehad),1)
 X = np.append(x0,x1,axis=1)
 X = np.append(X,x2,axis=1)
 return y,X


y,X = CreateElementsForLinearRegression(Ehad,nhitScifiTOT,nhitMuFilterTOT)
theta = np.ones((3,1))
#theta[0]=0
print(theta)

def computeCost(X,y,theta):
 m = len(y) #number of events
 predictions=X.dot(theta)
 square_err=(predictions - y)*(predictions-y)
 cost = 1/(2*m) * np.sum(square_err)
 #print(cost)
 return cost

cost = computeCost(X,y,theta)

def gradientDescent(X,y,theta,alpha,num_iters):
    """
    Take in numpy array X, y and theta and update theta by taking   num_iters gradient steps
    with learning rate of alpha
    
    return theta and the list of the cost of theta during each  iteration
    """
    
    m=len(y)
    J_history=[]
    theta_history =[0]*num_iters
    
    for i in range(num_iters):
        theta_history[i] =[0]*3
        predictions = X.dot(theta)
        #print(' pred: ', predictions)
        error = np.dot(X.transpose(),(predictions -y))
        #print('error : ', error)
        descent=alpha * 1/m * error
        #print(' descent: ',descent)
        theta-=descent
        #print('theta: ',theta)
        J_history.append(computeCost(X,y,theta))
        theta_history[i] = [theta.item(0),theta.item(1),theta.item(2)]
    
    return theta, J_history, theta_history


#def ComputeErrorsTheta(X,y,theta):


theta,J_history,theta_history = gradientDescent(X,y,theta,0.0000002,200)

plt.figure(1)
plt.plot(J_history)
plt.xlabel("Iteration")
plt.ylabel("$J(\Theta)$")
plt.title("Cost function using Gradient Descent")
plt.show(block=False)

theta0=[]
theta1=[]
theta2=[]
for i in range(len(theta_history)):
 theta0.append(theta_history[i][0])
 theta1.append(theta_history[i][1])
 theta2.append(theta_history[i][2])

plt.figure(2)
plt.plot(theta0)
plt.xlabel("Iteration")
plt.ylabel("$\Theta_{0}$")
plt.title("")
plt.show(block=False)

plt.figure(3)
plt.plot(theta1)
plt.xlabel("Iteration")
plt.ylabel("$\Theta_{1}$")
plt.title("")
plt.show(block=False)

plt.figure(4)
plt.plot(theta2)
plt.xlabel("Iteration")
plt.ylabel("$\Theta_{2}$")
plt.title("")
plt.show(block=False)
print("h(x) ="+str(round(theta[0,0],2))+" + "+str(round(theta[1,0],2))+"x1 + "+str(round(theta[2,0],2))+"x2")

#Run over the remaining directories:
#directories1 = ['10E3to15E3/','15E3to20E3/','25E3to30E3/','35E3to40E3/','40E3to45E3/','45E3to50E3/']
#Ehad, nhitScifiTOT, nhitMuFilterTOT = GetEhadAndNhits(directories1)
Ehad, nhitScifiTOT, nhitMuFilterTOT = ReadHitsTree(path2dir+"nuhits_SND_total_nuanu.root",entrylist[int(nentries *trainingfraction):])
y_test,X_test = CreateElementsForLinearRegression(Ehad,nhitScifiTOT,nhitMuFilterTOT)


def predict(x,theta):
    """
    Takes in numpy array of x and theta and return the predicted value of y based on theta
    """
    
    predictions= x.dot(theta)
  
    return predictions
def PlotFit(X,y,theta):
 fig, axes = plt.subplots(1,2,sharey=True,constrained_layout=True,figsize=(30,15))
 pred = predict(X,theta)
 for i in range(0,2):
  axes[i].set_title("Best fit line")
  #axes[i].set_xlabel(str(e))
  axes[i].set_ylabel('Ehad')
  X_plt = X[:,[0,i+1]]
  axes[i].scatter(X[:,i+1], y,color='g')
  X_plt=np.insert(X_plt,i+1,0,axis=1)
  axes[i].plot(X[:,i+1],predict(X_plt,theta),'-' ,color='k')
 plt.show(block=False)

PlotFit(X_test,y_test,theta)

Ehad_rec = predict(X_test,theta)

res = (y_test-Ehad_rec)/y_test

#now, we can sort entries back again (they were shuffled in entrylist) and add a TTree with Erec and Etrue
Ehad_rec,y_test,entrylist = sort_arrays_by_last(Ehad_rec, y_test, entrylist)

energydata = {"Erec": Ehad_rec, "Etrue": y_test, "HitsTreeEntry": entrylist}

energydf = r.RDF.MakeNumpyDataFrame(energydata)
energydf.Snapshot("energytree","reshisto_shuffleseed{}.root".format(numpyseed))

#opening newly created file, adding histograms
outputfile = r.TFile(path2dir+("reshisto_shuffleseed{}.root".format(numpyseed)),"UPDATE")
h_res = r.TH1F('h_res','Resolution; #Delta E/E;',20,-1,1)
h_resvsE = r.TH2F('h_resvsE','Resolution vs Energy;E[GeV];#Delta E', 50,0,5000,20, -1 , 1)

for i in range(len(res)):
 #fill histograms
 h_res.Fill(res[i])
 h_resvsE.Fill(y_test[i],res[i])

#Plot and fit
h_res.SetLineColor(r.kBlack)
f1 = r.TF1('f1','pol0(0)+gaus(1)',-0.8,0.8)
c = r.TCanvas()
h_res.Draw()
f1.SetParameter(0,100)
f1.SetParameter(2,0)
f1.SetParameter(3,0.3)
h_res.Fit(f1)
f1.Draw("sames")

#write objects and close file
h_res.Write()
h_resvsE.Write()

outputfile.Close()
