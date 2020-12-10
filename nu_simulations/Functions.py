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

def CheckGeoAcceptance(ShipGeo,x0,y0,z0):
 xmin = ShipGeo.EmulsionDet.ShiftX 
 xmax = ShipGeo.EmulsionDet.ShiftX+ShipGeo.Scifi.xdim
 ymin = ShipGeo.EmulsionDet.ShiftY
 ymax = ShipGeo.EmulsionDet.ShiftY + ShipGeo.Scifi.ydim
 zmin = -ShipGeo.EmulsionDet.zdim/2
 zmax = ShipGeo.EmulsionDet.zdim/2
 #print(xmin, xmax, ymin, ymax)
 acceptance = False
 if xmin <= x0 <= xmax:
  if ymin <= y0 <= ymax:
   if zmin <= z0 <= zmax:
    acceptance = True
 return acceptance

def SelectAcceptanceEvts(ShipGeo,t):
 AcceptanceEvts = []
 for ev in range(t.GetEntries()):
  t.GetEntry(ev)
  for mctrk in t.MCTrack:
   if mctrk.GetMotherId()==-1:
    x0 = mctrk.GetStartX()
    y0 = mctrk.GetStartY()
    z0 = mctrk.GetStartZ()
    acceptance = CheckGeoAcceptance(ShipGeo,x0,y0,z0)
    if acceptance:
     AcceptanceEvts.append(ev)
 return AcceptanceEvts

def GetWeight(t,ev):
 t.GetEntry(ev)
 wfound = False
 weight =-99999
 for mctrk in t.MCTrack:
  if wfound==True:
   break
  if mctrk.GetMotherId()==-1:
   weight = mctrk.GetWeight()
   wfound=True
 return weight


def PrintEventsInAcceptance(ShipGeo,t):
 AcceptanceEvts = SelectAcceptanceEvts(ShipGeo,t)
 print('Events entering the detector: ')
 print(AcceptanceEvts)
 return

def GetNeutrinoVertex(t, ev):
 t.GetEntry(ev)
 nufound=False
 for trk in t.MCTrack:
  if trk.GetMotherId()==-1:
   nufound=True
   x0 = trk.GetStartX()
   y0 = trk.GetStartY()
   z0 = trk.GetStartZ()
  if nufound==True:
   break
 return x0,y0,z0
  
def GetNeutrino4momentum(t, ev):
 t.GetEntry(ev)
 nufound=False
 for trk in t.MCTrack:
  if trk.GetMotherId()==-1:
   nufound=True
   px = trk.GetPx()
   py = trk.GetPy()
   pz = trk.GetPz()
   p = trk.GetP()
  if nufound==True:
   break
 return p,px,py,pz

def GetNPrimaryTracks(t,ev):
 t.GetEntry(ev)
 ntrks=0
 for trk in t.MCTrack:
  if trk.GetMotherId()==0:
   ntrks+=1
 return ntrks

def GetHitP(hit):
 px = hit.GetPx()
 py = hit.GetPy()
 pz = hit.GetPz()
 p = r.TMath.Sqrt(px*px+py*py+pz*pz)
 return p, px, py, pz

def GetHitDirection(hit):
 px = hit.GetPx()
 py = hit.GetPy()
 pz = hit.GetPz()
 return px/pz,py/pz

def GetHitPosition(hit):
 x = hit.GetX()
 y = hit.GetY()
 z = hit.GetZ()
 return x,y,z

def GetNeutrinoE(t,ev):
 t.GetEntry(ev)
 for trk in t.MCTrack:
  if trk.GetMotherId()==-1:
   p = trk.GetP()
 return p

def GetMuonE(t,ev):
 t.GetEntry(ev)
 p=0
 for trk in t.MCTrack:
  if trk.GetMotherId()==0 and abs(trk.GetPdgCode())==13:
   p = trk.GetP()
   return p

def GetMuon4momentum(t,ev):
 t.GetEntry(ev)
 p=0
 muFound=False
 for trk in t.MCTrack:
  if trk.GetMotherId()==0 and abs(trk.GetPdgCode())==13:
   p = trk.GetP()
   px = trk.GetPx()
   py = trk.GetPy()
   pz = trk.GetPz()
   muFound=True
   break
 if muFound==True:
  return p, px, py, pz
 else:
  return -99,-99,-99,-99


def GetHadronSystemEnergy(t,ev):
 t.GetEntry(ev)
 pnu = GetNeutrinoE(t,ev)
 pmu = GetMuonE(t,ev)
 if pmu:
  return pnu-pmu
 else:
  return pnu

def GetPositionMuFilterPlane(ShipGeo,npl):
 zmax = ShipGeo.MuFilter.Zcenter + ShipGeo.MuFilter.Z/2
 #print(zmax)
 z =  zmax - ShipGeo.MuFilter.TDetZ/2 -(7-npl)*(ShipGeo.MuFilter.FeZ+ShipGeo.MuFilter.TDetZ)
 return z

def Propagate(x0,y0,z0,z1,tx,ty):
 Dz = z1-z0
 x1 = x0+tx*Dz
 y1 = y0+ty*Dz
 return x1,y1

def GetTauDecayVertex(t, ev):
 t.GetEntry(ev)
 taufound=False
 tauid=-99
 for i,trk in enumerate(t.MCTrack):
  if trk.GetMotherId()==0 and abs(trk.GetPdgCode())==15:
   tauid = i
   taufound=True
   break
 if taufound==True:
  #print(tauid) 
  for trk in t.MCTrack:
   if trk.GetMotherId()==tauid:
    x1 = trk.GetStartX()
    y1 = trk.GetStartY()
    z1 = trk.GetStartZ()
    break
 else:
  x1=-99
  y1=-99
  z1=-99
 return x1,y1,z1

def GetTauMomentum(t, ev):
 t.GetEntry(ev)
 for i,trk in enumerate(t.MCTrack):
  if trk.GetMotherId()==0 and abs(trk.GetPdgCode())==15:
   return trk.GetP(), trk.GetPx(), trk.GetPy(), trk.GetPz()

def GetPrimaryTracks(t,ev):
 trk1 =[]
 t.GetEntry(ev)
 for trk in t.MCTrack:
  if trk.GetMotherId()==0:
   if abs(trk.GetPdgCode())==16:
    break
   else:
    trk1.append(trk)
 return trk1

def GetTauDaughters(t,ev):
 trk2 =[]
 t.GetEntry(ev)
 for trk in t.MCTrack:
  if trk.GetMotherId()==1:
   trk2.append(trk)
 return trk2

def sort_arrays_by_last(a, b, c):
 '''sorting three numpy arrays a,b,c according to c values'''
 sortindexes = np.argsort(c)
 c = c[sortindexes]
 b = b[sortindexes]
 a = a[sortindexes]
 return a,b,c
