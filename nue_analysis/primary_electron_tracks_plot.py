import ROOT as r
import fedrarootlogon

#/afs/cern.ch/work/a/aiulian/public/nueecc_study
inputfile = r.TFile.Open("total_primaryelectroninfo.root") 
inputtree = inputfile.Get("treeexample")
#getting one entry for each MCEvent and primary electron segment info
inputtree.BuildIndex("s.eMCEvt","s.eMCTrack")

vertexpath = "/eos/experiment/sndlhc/MonteCarlo/FEDRA/nuecc/nuecc_muon1.3E5/b000021/"
#MCelectroninfofile = r.TFile.Open("electroninfo.root")
#nsegMCtree = MCelectroninfofile.Get("nsegtrue") #why did I call everything nsegtrue?

nevents = 4000 #max number of MCTrue Events (4000)

hnseg2D = r.TH2I("hnseg2D","number of segments in the track vs belonging to the primary electron;nseg;nseg_e",61,0,61,61,0,61)
hnseg2D_vsMCTrue = r.TH2I("hnseg2D_vsMCTrue","number of segments of electron MCTrue vs in upstream FEDRA track;nseg_e_MCtrue;nseg_e_tracked;",61,0,61,61,0,61)

hpurity = r.TH1D("hpurity","electron tracking purity;purity[%]",20,0,1)

min_nsegtrue = 3 #require at least 3 segments from the true electron to accept the event

htracks_split = r.TH1I("htrackssplit","how many reco tracks the electron is split into;ntracks",10,0,10)

hn = r.TH1I("hn","multiplicity of vertices with electron",30,0,30)

ntracked_primarye = 0
for ievent in range(nevents):

 check = inputtree.GetEntryWithIndex(ievent,1) #primary electron for each event
 
 #check if -1 if the event is not found
 if check > 0:
  #getting same event in processed tree from MC simulation
  #nsegMCtree.GetEntry(ievent)
  #nsegMCtrue = nsegMCtree.nseg_primarye
  multiplicity = 0
  try: #it may crash if refit is not present, I consider it as 0
  
   vertexdf = r.RDataFrame("vtx",vertexpath+"b000021.0.0.{}.vtx.root".format(ievent+1))
   vertexdf_flag = vertexdf.Filter("flag==0")
  
   vertexdf_evertex = vertexdf_flag.Filter("ROOT::VecOps::Any(TrackID=={})".format(inputtree.trid)) #check if the interesting track is in any vertex with flag 0 
   result_mult = vertexdf_evertex.Take[int]("n")
   if (result_mult.size()>0):
    multiplicity = result_mult.GetValue()[0]
  except:
   multiplicity = -1 #vtx refit was not done
  hn.Fill(multiplicity)
 
  purity = inputtree.nsegtrue/inputtree.nseg 
  
  if inputtree.nsegtrue >= min_nsegtrue:
    ntracked_primarye = ntracked_primarye + 1
  
  hnseg2D.Fill(inputtree.nseg, inputtree.nsegtrue)
  #hnseg2D_vsMCTrue.Fill(nsegMCtrue, inputtree.nsegtrue)
  hpurity.Fill(purity)
  htracks_split.Fill(inputtree.ntracks_split)
 
#plotting histograms 
c1 = r.TCanvas()
hpurity.Draw()
c2 = r.TCanvas()
htracks_split.Draw()
c2D = r.TCanvas() 
hnseg2D.Draw("COLZ")
#c2D_MC = r.TCanvas()
#hnseg2D_vsMCTrue.Draw("COLZ")
cn = r.TCanvas()
hn.Draw()

cn.Print("mult_vertices_primaryelectron.root")

print("Events with at least {} segments: {} over {}, rate {}".format(
  min_nsegtrue,ntracked_primarye,nevents,ntracked_primarye/nevents))