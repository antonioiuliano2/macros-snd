//tool to load hit from emulsion to FEDRA (13 aprile 2018) //updated to work with the new TTreeReader structure (6 March)
//to use it, go in a directory and create the folders b000001/p001 to b000001/p029
//then launch it from the directory mother of b000001

#include <stdio.h>
#include <TROOT.h>
#include "TRandom.h"

void fromsndsw2FEDRA(TString filename, int nbrick);

int FindBrick(Float_t hitX, Float_t hitY, Float_t hitZ); //returning which brick the hit belongs to;

//#include "/home/utente/fedra/include/EdbCouplesTree.h"
using namespace TMath;
TFile *file = NULL;
//starting script
void printnuevents_brick(TString filename){
 
 //const Int_t nevents = reader.GetTree()->GetEntries();
 int ievent = 0;
 int brickIDs[20] = {11,12,13,14,21,22,23,24,31,32,33,34,41,42,43,44,51,52,53,54};

 TH1I *hbrickID = new TH1I("hbrickID","ID of brick where neutrino interaction happened;brickID",50,0,50); 

 //simulation of SND neutrino data
 const int nflavours = 4; //nue,anue,numu,anumu
 int nuyield[nflavours] = {730,290,235, 120}; //over all RUN3 (2022-2024) data taking, 150 inverse femtobarns
 float flavour_multiplier = 1e+5;// #0 numu, 100 anumu, 200 nue, 300 anue
 //int nuyield[nflavours] = {0,1020,235,120};
 //float replaceratio = 25./150.; //assuming we replace every 25 inverse femtobarns
 float replaceratio = 1.;
 TString inputpaths[nflavours] = {"/eos/user/a/aiulian/sim_snd/numu_sim_activeemu_7_September_2021/","/eos/user/a/aiulian/sim_snd/anumu_sim_activeemu_8_September_2021/", "/eos/user/a/aiulian/sim_snd/nue_sim_activeemu_10_September_2021/1/", "/eos/user/a/aiulian/sim_snd/anue_sim_activeemu_13_September_2021/1/"};
cout<<"Event,NBrick"<<endl;
 // ************************STARTING LOOP ON SIMULATION******************  
for (int iflavour = 0; iflavour < nflavours; iflavour++){

 // **********************OPENING INPUT FILE***************************
 TFile * inputfile = TFile::Open((inputpaths[iflavour]+filename).Data());

 if (!inputfile) return;
 //getting tree and arrays
 TTreeReader reader("cbmsim",inputfile);
 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");
 TTreeReaderArray<EmulsionDetPoint> emulsionhits(reader,"EmulsionDetPoint");

 int nevents = nuyield[iflavour] * replaceratio;  

 //temporary fix for inECC file misproduction: using list of event numbers
 fstream eventlistfile((inputpaths[iflavour]+"inECCevents.txt").Data(), fstream::in);
 int inECCevent;

 // while (reader.Next()){
 for (int i = 0; i < nevents; i++){  
  eventlistfile>>inECCevent;
  //reader.Next();
  reader.SetEntry(inECCevent);//reading next event in ECC
  int nbrickvertex = FindBrick(tracks[0].GetStartX(), tracks[0].GetStartY(), tracks[0].GetStartZ());
  //cout<<"TEST "<<inECCevent<<" "<<tracks[0].GetStartX()<<" "<<tracks[1].GetStartY()<<" "<<tracks[2].GetStartZ()<<" "<<nbrickvertex<<endl;
  hbrickID->Fill(nbrickvertex+1);
  ievent = inECCevent+flavour_multiplier*iflavour;
  cout<<ievent<<","<<nbrickvertex+1<<endl;
  }//end loop over events
 } //end loop over files
 //closing temporary fix file
 TCanvas *c = new TCanvas();
 hbrickID->Draw();

}
int FindBrick(Float_t hitX, Float_t hitY, Float_t hitZ){
  float xborder = -27.5; //arbitrary, but accurate enough to separate the bricks
  float yborder = 35.1;
  int nx, ny, nz;
  if (hitX > xborder) nx = 0; //left 1, right 0, x positive towards right
  else nx = 1;
  if (hitY < yborder) ny = 0; //down 0, up 1, y positive towards up
  else ny = 1; 

  //beam exiting
  float z0_start = -25.4750; float z0_end = -17.6850;
  float z1_start = -15.8750; float z1_end = -8.0850;
  float z2_start = -6.2750;  float z2_end = 1.5150;
  float z3_start = 3.3250;   float z3_end = 11.1150;
  float z4_start = 12.9250;  float z4_end = 20.7150;

  if (hitZ > z0_start && hitZ < z0_end) nz = 1;
  else if(hitZ > z1_start && hitZ < z1_end) nz = 2;
  else if(hitZ > z2_start && hitZ < z2_end) nz = 3;
  else if(hitZ > z3_start && hitZ < z3_end) nz = 4;
  else if(hitZ > z4_start && hitZ < z4_end) nz=5;
  else nz = -10; //not in a brick

  int nbrick = nx + ny*2 + 10 * nz;
  return nbrick;
} //possible numbers: 10, 11, 12, 13, 20,21,22,23, 30,31,32,33, 40,41,42,43, 50,51,52,53,54
