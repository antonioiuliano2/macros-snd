//script to count EmulsionDetPoint for primary electrontrack
using namespace ROOT;

int CountNSegPrimaryElectron(ROOT::RVec<int> EmulsionDetPoint_fTrackID,ROOT::RVec<int> EmulsionDetPoint_fDetectorID){
 int nseg_primarye = 0;
 for (int i = 0; i < EmulsionDetPoint_fDetectorID.size(); i++){
  int trackID = EmulsionDetPoint_fTrackID[i];
  if (trackID == 1) nseg_primarye ++;
 }
 return nseg_primarye;
}
void studyMCelectrontrack(){
  const int nfiles = 100;
  std::vector<string> inputfiles;
  for (int i = 0; i < nfiles; i++){
    inputfiles.push_back(Form("/eos/experiment/sndlhc/users/dancc/NUSIM/nue_inBrick21/%i/sndLHC.Genie-TGeant4.root",i+1)); //from 1 to 100, 100 events each
  }

  RDataFrame df("cbmsim", inputfiles); // Interface to TTree and TChain
  
  auto df1 = df.Define("nseg_primarye",CountNSegPrimaryElectron,{"EmulsionDetPoint.fTrackID","EmulsionDetPoint.fDetectorID"});
  
  auto hnseg = df1.Histo1D<int>({"hnseg","Number of films traversed by primary electron in brick 21",61,0,61},"nseg_primarye");

  TCanvas *c = new TCanvas();

  hnseg->DrawClone();

  df1.Snapshot("nsegtrue", "electroninfo.root", {"nseg_primarye"});
}