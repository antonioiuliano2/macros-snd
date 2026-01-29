import ROOT as r

#/afs/cern.ch/work/a/aiulian/public/revertexing_addtracks/b000021
inputfile = r.TFile.Open("total_primaryelectroninfo.root") 
inputtree = inputfile.Get("treeexample")
#getting one entry for each MCEvent and primary electron segment info
inputtree.BuildIndex("s.eMCEvt","s.eMCTrack")

nevents = 5000 #max number of MCTrue Events

hnseg2D = r.TH2I("hnseg2D","number of segments of electron vs all segments;nseg;nsegtrue",61,0,61,61,0,61)
hpurity = r.TH1D("hpurity","electron tracking purity;purity[%]",20,0,1)
htracks_split = r.TH1I("htrackssplit","how many reco tracks the electron is split into;ntracks",10,0,10)

for ievent in range(nevents):

 check = inputtree.GetEntryWithIndex(ievent,1) #primary electron for each event
 #check if -1 if the event is not found
 if check > 0:
 
  purity = inputtree.nsegtrue/inputtree.nseg
  
  hnseg2D.Fill(inputtree.nseg, inputtree.nsegtrue)
  hpurity.Fill(purity)
  htracks_split.Fill(inputtree.ntracks_split)
 
#plotting histograms 
c1 = r.TCanvas()
hpurity.Draw()
c2 = r.TCanvas()
htracks_split.Draw()
c2D = r.TCanvas() 
hnseg2D.Draw()

