import ROOT as r
import fedrarootlogon

r.gStyle.SetOptStat(0)

cpu_procfile = r.TFile.Open("/home/scanner/sndlhc/RUN1/firsttest_offlinescans/b000024/trackfiles/109_102/b000024.0.0.0.trk.root")
Naples_procfile = r.TFile.Open("/home/scanner/sndlhc/RUN1/b000124/trackfiles/9_2/b000124.0.0.0.trk.root")
faster_procfile = r.TFile.Open("/home/scanner/sndlhc/RUN1/b000124/trackfiles/rootfiles/609_602/b000124.0.0.0.trk.root") #tighter selection in length
Bologna_procfile = r.TFile.Open("/home/scanner/sndlhc/RUN1/b000124/trackfiles/rootfiles/709_702/b000124.0.0.0.trk.root")

files = [cpu_procfile, Naples_procfile, faster_procfile, Bologna_procfile]


titles = ["cpu","Napoli","Test Faster","Bologna"]

nseghistograms = []
nplhistograms = []

cnseg = r.TCanvas()

for ifile,inputfile in enumerate(files):
 trackstree = inputfile.Get("tracks")
 #drawing nseg
 trackstree.Draw("nseg>>hnseg{}".format(ifile))
 trackstree.Draw("npl>>hnpl{}".format(ifile))

 hnseg = r.gDirectory.FindObject("hnseg{}".format(ifile))
 hnseg.SetTitle(titles[ifile])
 nseghistograms.append(hnseg)

 hnpl = r.gDirectory.FindObject("hnpl{}".format(ifile))
 hnpl.SetTitle(titles[ifile])
 nplhistograms.append(hnpl)


#drawing histograms together
nseghistograms[0].Draw()
nseghistograms[1].SetLineColor(r.kRed)
nseghistograms[1].Draw("SAMES")
#nseghistograms[2].SetLineColor(r.kMagenta)
#nseghistograms[2].Draw("SAMES")
nseghistograms[3].SetLineColor(r.kGreen)
nseghistograms[3].Draw("SAME")
cnseg.BuildLegend()
nseghistograms[0].SetTitle("volume track comparison;nseg")

cnpl = r.TCanvas()
nplhistograms[0].Draw()
nplhistograms[1].SetLineColor(r.kRed)
nplhistograms[1].Draw("SAMES")
#nplhistograms[2].SetLineColor(r.kMagenta)
#nplhistograms[2].Draw("SAMES")
nplhistograms[3].SetLineColor(r.kGreen)
nplhistograms[3].Draw("SAME")
cnpl.BuildLegend()
nplhistograms[0].SetTitle("volume track comparison;npl")
