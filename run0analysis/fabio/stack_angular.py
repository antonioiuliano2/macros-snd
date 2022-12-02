import ROOT as r
import numpy as np
import fedrarootlogon

path1 = '/home/scanner/sndlhc/RUN0/b000431/first_quadrant/b000431/trackfiles/31_7'
path2 = '/home/scanner/sndlhc/RUN0/b000431/second_quadrant/b000431/trackfiles/31_7'
path3 = '/home/scanner/sndlhc/RUN0/b000431/third_quadrant/b000431/trackfiles/31_7'
path4 = '/home/scanner/sndlhc/RUN0/b000431/fourth_quadrant/b000431/trackfiles/31_7'
trackFile1 = path1+'/b000431.0.0.0.trk.root'
trackFile2 = path2+'/b000431.0.0.0.trk.root'
trackFile3 = path3+'/b000431.0.0.0.trk.root'
trackFile4 = path4+'/b000431.0.0.0.trk.root'

r.gStyle.SetOptStat(0)
cx = r.TCanvas('cx', 'stack angular x distribution', 800, 600)
f1 = r.TFile.Open(trackFile1)
f2 = r.TFile.Open(trackFile2)
f3 = r.TFile.Open(trackFile3)
f4 = r.TFile.Open(trackFile4)
tree1 = f1.tracks
tree2 = f2.tracks
tree3 = f3.tracks
tree4 = f4.tracks
#h1x = r.TH1D('h1x', '1st_x; tan#theta_{xz}', 100, 0, 0.025)
h1x = r.TH1D('h1x', '1st_x; tan#theta_{xz}', 800, -0.1, 0.1)
h2x = r.TH1D('h2x', '2nd_x; tan#theta_{xz}', 800, -0.1, 0.1)
h3x = r.TH1D('h3x', '3rd_x; tan#theta_{xz}', 800, -0.1, 0.1)
h4x = r.TH1D('h4x', '4th_x; tan#theta_{xz}', 800, -0.1, 0.1)
h1x.SetLineColor(1)
h2x.SetLineColor(2)
h3x.SetLineColor(3)
h4x.SetLineColor(4)
tree1.Draw("t.eTX>>h1x", "nseg>17", "same")
tree2.Draw("t.eTX>>h2x", "nseg>17", "same")
tree3.Draw("t.eTX>>h3x", "nseg>17", "same")
tree4.Draw("t.eTX>>h4x", "nseg>17", "same")

cy = r.TCanvas('cy', 'stack angular y distribution', 800, 600)
h1y = r.TH1D('h1y', '1st_y; tan#theta_{yz}', 800, -0.1, 0.1)
h2y = r.TH1D('h2y', '2nd_y; tan#theta_{yz}', 800, -0.1, 0.1)
h3y = r.TH1D('h3y', '3rd_y; tan#theta_{yz}', 800, -0.1, 0.1)
h4y = r.TH1D('h4y', '4th_y; tan#theta_{yz}', 800, -0.1, 0.1)
h1y.SetLineColor(1)
h2y.SetLineColor(2)
h3y.SetLineColor(3)
h4y.SetLineColor(4)
tree1.Draw("t.eTY>>h1y", "nseg>17", "same")
tree2.Draw("t.eTY>>h2y", "nseg>17", "same")
tree3.Draw("t.eTY>>h3y", "nseg>17", "same")
tree4.Draw("t.eTY>>h4y", "nseg>17", "same")

r.gStyle.SetOptStat(1)
cx2 = r.TCanvas('cx2', 'sum angular x distribution', 800, 600)
hx = r.TH1D('hx', 'all_quadrants; tan#theta_{xz}', 800, -0.1, 0.1)
histoList = r.TList()
histoList.Add(h1x)
histoList.Add(h2x)
histoList.Add(h3x)
histoList.Add(h4x)
hx.Merge(histoList)
hx.Draw()

cx.BuildLegend()
cy.BuildLegend()
h1x.SetTitle('angular x distribution')
h1y.SetTitle('angular y distribution')
cx.Update()
cy.Update()
