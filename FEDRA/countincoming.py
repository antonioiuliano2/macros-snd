import ROOT as r
import fedrarootlogon
#input functions for RDataframe must be in C++
r.gInterpreter.Declare("""
int countincoming(ROOT::RVec<int> incoming_array){
 int nin = 0;

 for (auto &incoming_track:incoming_array){ //for incoming_track in incoming_array (in python)
   if (incoming_track == 0) nin = nin + 1;
 }

 return nin;
}
 
""")

df = r.RDataFrame("vtx","vertextree.root")

#adding nin and nout
df0 = df.Define("nin","countincoming(incoming)")
df1 = df0.Define("nout","n - nin")

hnin_cutnout = df1.Filter("nout>1").Histo1D("nin")

#making histograms
hn = df1.Histo1D(("hn","Number of tracks total",10,0,10),"n")
hnin = df1.Histo1D(("hnin","Number of tracks in",10,0,10), "nin")
hnout = df1.Histo1D(("hnout","Number of tracks out",10,0,10),"nout")

#drawing plots
c = r.TCanvas()
hn.DrawClone()
hnin.SetLineColor(r.kRed)
hnin.DrawClone("same")
hnout.SetLineColor(r.kMagenta)
hnout.DrawClone("same")
c.BuildLegend()

c1 = r.TCanvas()
hnin_cutnout.Draw()
