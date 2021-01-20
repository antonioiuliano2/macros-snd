//check distributions of produced GENIE events
void plotGenieEvents(TString nuname, TString process){
  TString prepath("/afs/cern.ch/work/a/aiuliano/public/Generate_GENIEinput_AnnaritaSND/GenieEvents_SNDAcceptance/");
  TString filename = nuname+TString("_")+process+TString("_FairShip.root");
  TString subpath = process+TString("/")+nuname+TString("/");
  
  TString filepath = prepath + subpath + filename;

  //input dataframe 
  auto df = ROOT::RDataFrame("gst",filepath.Data()); 
  //plotting pzv

  auto hnup = df.Histo1D({"hnup","Momentum of neutrino;P[GeV]",250,0,5000},"pzv");

  TCanvas *c = new TCanvas();
  hnup->DrawClone();
}
