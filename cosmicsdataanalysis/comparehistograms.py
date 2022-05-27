'''comparing histograms from different files together'''
import ROOT as r

#in this case, angles from Cosmics and package

#first, cosmics file
file1 = r.TFile.Open("/home/scanner/sndlhc/SNDCosmics/angles_firstfive_B1_nseglarger3.root")

c1 = file1.Get("c1_n4")

hcostheta1 = c1.GetPrimitive("hcostheta")

#second, package data file

file2 =  r.TFile.Open("/home/scanner/sndlhc/TEST_POSA/b000003/angles_nseglarger2_3platess.root")
c2 = file2.Get("c1_n4")
hcostheta2 = c2.GetPrimitive("hcostheta")

#normalizing and drawing them together

ctheta = r.TCanvas()
hcostheta1.Scale(1./hcostheta1.Integral())
hcostheta2.Scale(1./hcostheta2.Integral())
hcostheta1.Draw()
hcostheta1.SetTitle("wall commissioning brick 1")
hcostheta2.SetLineColor(r.kRed)
hcostheta2.Draw("SAME")
hcostheta2.SetTitle("test package")
ctheta.BuildLegend()
