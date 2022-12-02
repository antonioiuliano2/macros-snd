import ROOT as r
import numpy as np
import fedrarootlogon

path = '/home/scanner/sndlhc/RUN0/b000431/first_quadrant/b000431/trackfiles/31_7'
trackFile = path+'/b000431.0.0.0.trk.root'

f = r.TFile.Open(trackFile)
tree = f.tracks
c = r.TCanvas('c', 'angular distribution', 800, 600)

h = r.TH1D('h', 'h; tan#theta_{xz}', 50, 0, 0.025)
'''
for track in tree:
 if track.nseg>17:
  h.Fill(track.t.TX())
'''
tree.Draw("t.eTX>>h", "nseg>17")
h.Draw()

g1 = r.TF1("g1", "gaus", 0.009,0.015)
g1.SetLineColor(r.kRed)
g2 = r.TF1("g2", "gaus", 0.015, 0.02)
g2.SetLineColor(r.kGreen)
h.Fit(g1, 'R')
h.Fit(g2, 'R+')

g3 = r.TF1("double_gaus", "gaus(0) + gaus(3)", 0.009, 0.02)
g3.SetParNames("Constant 1", "Mean 1", "Sigma 1",
               "Constant 2", "Mean 2", "Sigma 2")
g3.SetLineColor(r.kBlue)
  
r.gStyle.SetOptFit(1)

par1 = np.zeros(3)
par2 = np.zeros(3)
g1.GetParameters(par1)
g2.GetParameters(par2)
g3.SetParameters(np.concatenate((par1, par2)))
  
h.Fit(g3, "R+")

