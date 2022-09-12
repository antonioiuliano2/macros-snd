import ROOT as r
import fedrarootlogon

run0path = "/home/scanner/sndlhc/RUN0/b000031/"

couplesfile = r.TFile.Open(run0path+"p019/31.19.0.0.cp.root")
couples = couplesfile.Get("couples")

for cp in couples:
 seg = cp.s

 print(seg.X(), seg.Y(), seg.TX(), seg.TY())
