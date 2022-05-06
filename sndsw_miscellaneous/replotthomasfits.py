'''replotting Thomas fits to show also fit results. (06 May 2022)'''
import ROOT as r

inputfile = r.TFile("v3-scifiRes-run23.root")

thomascanvas = inputfile.Get("scifiRes")
#we want to plot the two plots from the third row one above the other
resh30pad = thomascanvas.GetPad(5)
h30 = resh30pad.GetPrimitive("resH_Scifi30")

resv31pad = thomascanvas.GetPad(6)
h31 = resv31pad.GetPrimitive("resV_Scifi31")

r.gStyle.SetOptFit(111)
#creating my canvas
newcanvas = r.TCanvas()
newcanvas.Divide(1,2)
newcanvas.cd(1)
h30.GetXaxis().SetRangeUser(-500,500)
h30.Draw()
newcanvas.cd(2)
h31.GetXaxis().SetRangeUser(-500,500)
h31.Draw()

