
'''reading tracks, estimating resolution from a linear fit using only the information from 5 films'''
import ROOT as r
import fedrarootlogon
import progressbar #for bars in loops
import numpy as np

nfilms = 57 #total number of films
minnseg = 40 # if 10 plates
store_every_ntracks = 1
r.gStyle.SetOptFit(111)

prefix = "root:://eospublic.cern.ch/"
inputfile = "/eos/experiment/sndlhc/emulsionData/2022/CERN/emu_reco/RUN1/b000044/trackfiles/rootfiles/57_1/cell_5_5/linked_tracks.root"

tracksfile = r.TFile.Open(prefix+inputfile)
trackstree = tracksfile.Get("tracks")
#defining graphs
gzx = r.TGraph()
gzy = r.TGraph()

fzx = r.TF1("fzx","pol1")
fzy = r.TF1("fzy","pol1")
#list of histogram, one for each film
hrestx = []
hresty = []
hresx = []
hresy = []
#defining resolution histograms
for ifilm in range(nfilms):
 hrestx.append(r.TH1D("hrestx_{}".format(ifilm),"Angular resolution;TX",60,-0.06,0.06))
 hresty.append(r.TH1D("hresty_{}".format(ifilm),"Angular resolution;TY",60,-0.06,0.06))
 hresx.append(r.TH1D("hresx_{}".format(ifilm),"Position resolution;X[#mum]",60,-60,60))
 hresy.append(r.TH1D("hresy_{}".format(ifilm),"Position resolution;Y[#mum]",60,-60,60))

#minnseg = 4 #if 5 plates
czx = r.TCanvas()
czy = r.TCanvas()
#starting loop over tracks
#ntracks = 100000
ntracks = trackstree.GetEntries()
bar = progressbar.ProgressBar(maxval=ntracks, \
    widgets=[progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage()])
bar.start()

for itrack in range(ntracks):
 
 FilledFilm=np.zeros(nfilms,dtype=bool)#tell us if there is a segment or an hole in this plate, default False
 zPlate = np.zeros(nfilms)
 #we can check with np.all(FilledFilm[52:57]) if the last 5 are all filled
 trackstree.GetEntry(itrack)
 #condition over track length
 if (trackstree.nseg < minnseg):
  bar.update(itrack+1)
  continue
 segments = trackstree.s
 #looping over track segments
 ipoint = 0
 for seg in segments:
  FilledFilm[seg.Plate()-1] = True #from 0 to nfilms - 1
  zPlate[seg.Plate()-1] = seg.Z()
  dz = 300. #z of a film

  varx = seg.COV()[0,0]
  vary = seg.COV()[1,1]

  gzx.SetPoint(ipoint,seg.Z(), seg.X())
  gzy.SetPoint(ipoint,seg.Z(), seg.Y())
  #if i want to add errors, still error on z is not so clear
  #gzx.SetPointError(ipoint,300,r.TMath.Sqrt(varx))
  #gzy.SetPointError(ipoint,300,r.TMath.Sqrt(vary))

  ipoint = ipoint + 1

 if(np.all(FilledFilm[nfilms-5:nfilms])): #checking we do not have gaps
  fromz = zPlate[nfilms-5] - dz/2.
  toz = zPlate[nfilms-1]+dz/2.
  gzx.Fit(fzx,"Q","",fromz,toz) #fitting only in this range
  gzy.Fit(fzy,"Q","",fromz,toz) 


 #comparing fitted and found angles/position
 tx_fitted = fzx.GetParameter(1)
 ty_fitted = fzy.GetParameter(1)
 ipoint = 0
 for seg in segments:
  ifilm = seg.Plate() - 1
  #comparing angles
  restx = tx_fitted - seg.TX()
  resty = ty_fitted - seg.TY()

  hrestx[ifilm].Fill(restx)
  hresty[ifilm].Fill(resty)

  #find y value for given z, then comparing positions
  x_fitted = fzx.Eval(seg.Z())
  y_fitted = fzy.Eval(seg.Z())

  resx = x_fitted - seg.X()
  resy = y_fitted - seg.Y()
  #if(fzx.GetChisquare()<50 and fzy.GetChisquare()<50):
  hresx[ifilm].Fill(resx)
  hresy[ifilm].Fill(resy)
  
  #empty graphs
  gzx.RemovePoint(ipoint)
  gzy.RemovePoint(ipoint)

  ipoint = ipoint + 1
 #end of track, updating progress bar and moving to next
 bar.update(itrack+1)
#end of track loop, draw resolution histograms and gaussian fits
bar.finish()
cresang = r.TCanvas()
cresang.Divide(2,1)
cresang.cd(1)
hrestx[56].Draw()
hrestx[56].Fit("gaus","","",-0.015,0.015)
cresang.cd(2)
hresty[56].Draw()
hresty[56].Fit("gaus","","",-0.015,0.015)

crespos = r.TCanvas()
crespos.Divide(2,1)
crespos.cd(1)
hresx[56].Draw()
hresx[56].Fit("gaus","","",-15,15)
crespos.cd(2)
hresy[56].Draw()
hresy[56].Fit("gaus","","",-15,15)
