//tool to load hit from emulsion to FEDRA (13 aprile 2018) //updated to work with the new TTreeReader structure (6 March)
//to use it, go in a directory and create the folders b000001/p001 to b000001/p029
//then launch it from the directory mother of b000001

#include <stdio.h>
#include <TROOT.h>
#include "TRandom.h"

void fromsndsw2FEDRA(TString filename, int nbrick);
void smearing (Float_t &TX, Float_t &TY, const float angres);
bool efficiency(const float emuefficiency);
bool efficiency(const float tantheta, TH1D * emuefficiency);

int FindBrick(Float_t hitX, Float_t hitY, Float_t hitZ); //returning which brick the hit belongs to;

//start script
void fromsndsw2FEDRA(){
 int nbrick = 9; //from 0 to 19
 fromsndsw2FEDRA("sndLHC.Genie-TGeant4.root",nbrick);
}

void set_default(TEnv &cenv){ //setting default parameters, if not presents from file
 cenv.SetValue("FairShip2Fedra.nbricks",20);//5 walls in 2x2 configuration
 cenv.SetValue("FairShip2Fedra.nplates",60);
 //cenv.SetValue("FairShip2Fedra.nevents",10000); // number of events to be passed to FEDRA
 cenv.SetValue("FairShip2Fedra.useefficiencymap",0);
 cenv.SetValue("FairShip2Fedra.emuefficiency",0.85); //only if useefficiency map is set to false
 cenv.SetValue("FairShip2Fedra.dosmearing",1);
 cenv.SetValue("FairShip2Fedra.maxtheta",1); //angular max of scanning
 cenv.SetValue("FairShip2Fedra.minmomentum",0.1); //do not pass particles beyond this value, track ID would be -2
 cenv.SetValue("FairShip2Fedra.ngrains",70); // to set weight
 cenv.SetValue("FairShip2Fedra.angres",0.003);//used for smearing, if dosmearing = true

}

//#include "/home/utente/fedra/include/EdbCouplesTree.h"
using namespace TMath;
TRandom3 *grandom = new TRandom3(); //creating every time a TRandom3 is a bad idea
TFile *file = NULL;
TH1D *heff = NULL ; //efficiency at different angles
//starting script
void fromsndsw2FEDRA(TString filename, int nbrick){
 grandom->SetSeed(0);

 TEnv cenv("FairShip2Fedra");
 set_default(cenv);
 cenv.ReadFile("FairShip2Fedra.rootrc" ,kEnvLocal);
 //getting options from file
 //const Int_t nevents = cenv.GetValue("FairShip2Fedra.nevents",10000);
 const int nplates = cenv.GetValue("FairShip2Fedra.nplates",60);
 const int nbricks = cenv.GetValue("FairShip2Fedra.nbricks",20); // to set b00000%i number

 float angres = cenv.GetValue("FairShip2Fedra.angres",0.003); //Used cases: 3, 5milliradians. Constant value overwritten if useresfunction=true
 float minmomentum = cenv.GetValue("FairShip2Fedra.minmomentum",0.1);
 float maxtheta = cenv.GetValue("FairShip2Fedra.maxtheta",1);

 const float ngrains = cenv.GetValue("FairShip2Fedra.ngrains",70) ; //the same number for all the couples, so they have the same weigth.
 const float emuefficiency = cenv.GetValue("FairShip2Fedra.emuefficiency",0.85); // flat value
 
 const bool useefficiencymap = cenv.GetValue("FairShip2Fedra.useefficiencymap",0); //use the map instead of the constant value down
 const bool dosmearing = cenv.GetValue("FairShip2Fedra.dosmearing",1); //gaussian smearing or not

 cout<<"Starting conversion with efficiency "<<emuefficiency<<" maxtheta "<<maxtheta<<" and min momentum "<<minmomentum<<endl;
 if (useefficiencymap){ 
  file = TFile::Open("efficiency_alltracks.root");
  heff = (TH1D*) file->Get("heff");
 }
 
 Float_t tx = 0, ty=0, xem= 0, yem = 0;
 //const Int_t nevents = reader.GetTree()->GetEntries();
 int ihit = 0, ievent = 0;
 int nfilmhit = 0, nbrickhit = 0;
 float tantheta, momentum;
 int trackID = 0, motherID = 0, pdgcode = 0;
 // ***********************CREATING FEDRA TREES**************************
 gInterpreter->AddIncludePath("/afs/cern.ch/work/a/aiuliano/public/fedra/include");
 EdbCouplesTree *ect[nbricks][nplates]; //2D array->which brick and which plate?
 int brickIDs[20] = {11,12,13,14,21,22,23,24,31,32,33,34,41,42,43,44,51,52,53,54};

 TH1I *hbrickID = new TH1I("hbrickID","ID of brick where neutrino interaction happened;brickID",50,0,50);

 cout<<"Now converting brick number "<<brickIDs[nbrick]<<endl;
 for (int i = 1; i <= nplates; i++){
   ect[nbrick][i-1] = new EdbCouplesTree();
   ect[nbrick][i-1]->InitCouplesTree("couples",Form("b0000%02i/p0%02i/%i.%i.0.0.cp.root",brickIDs[nbrick],i,brickIDs[nbrick],i),"RECREATE"); //i learned padding with %0i
 }
 Int_t Flag = 1;

 //simulation of SND neutrino data
 const int nflavours = 4; //nue,anue,numu,anumu
 int nuyield[nflavours] = {730,290,235, 120}; //over all RUN3 (2022-2024) data taking, 150 inverse femtobarns
 float flavour_multiplier = 1e+5;// #0 numu, 100 anumu, 200 nue, 300 anue
 //int nuyield[nflavours] = {0,1020,235,120};
 //float replaceratio = 25./150.; //assuming we replace every 25 inverse femtobarns
 float replaceratio = 1.;
 TString inputpaths[nflavours] = {"/eos/user/a/aiulian/sim_snd/numu_sim_activeemu_7_September_2021/","/eos/user/a/aiulian/sim_snd/anumu_sim_activeemu_8_September_2021/", "/eos/user/a/aiulian/sim_snd/nue_sim_activeemu_10_September_2021/1/", "/eos/user/a/aiulian/sim_snd/anue_sim_activeemu_13_September_2021/1/"};

 // ************************STARTING LOOP ON SIMULATION******************  
for (int iflavour = 0; iflavour < nflavours; iflavour++){

 // **********************OPENING INPUT FILE***************************
 TFile * inputfile = TFile::Open((inputpaths[iflavour]+filename).Data());

 if (!inputfile) return;
 cout<<"opening file "<<(inputpaths[iflavour]+filename).Data()<<endl;
 //getting tree and arrays
 TTreeReader reader("cbmsim",inputfile);
 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");
 TTreeReaderArray<EmulsionDetPoint> emulsionhits(reader,"EmulsionDetPoint");

 int nevents = nuyield[iflavour] * replaceratio;
 cout<<"Start processing nevents: "<<nevents<<endl;  

 //temporary fix for inECC file misproduction: using list of event numbers
 fstream eventlistfile((inputpaths[iflavour]+"inECCevents.txt").Data(), fstream::in);
 int inECCevent;

 // while (reader.Next()){
 for (int i = 0; i < nevents; i++){
  if (i%1000==0) cout<<"processing event "<<i<<" out of "<<nevents<<endl;
  eventlistfile>>inECCevent;
  //reader.Next();
  reader.SetEntry(inECCevent);//reading next event in ECC
  int nbrickvertex = FindBrick(tracks[0].GetStartX(), tracks[0].GetStartY(), tracks[0].GetStartZ());
  //cout<<"TEST "<<inECCevent<<" "<<tracks[0].GetStartX()<<" "<<tracks[1].GetStartY()<<" "<<tracks[2].GetStartZ()<<" "<<nbrickvertex<<endl;
  hbrickID->Fill(nbrickvertex+1);

   for (const EmulsionDetPoint& emupoint:emulsionhits){   
     bool savehit = true; //by default I save all hits
//no you don't want to do this//     if (j % 2 == 0) continue;
     momentum = TMath::Sqrt(pow(emupoint.GetPx(),2) + pow(emupoint.GetPy(),2) + pow(emupoint.GetPz(),2));
     pdgcode = emupoint.PdgCode();
     trackID = emupoint.GetTrackID();
     bool emubasetrackformat = true;    
     if (trackID >= 0) motherID = tracks[trackID].GetMotherId();
     else motherID = -2; //hope I do not see them
     
     xem = emupoint.GetX();
     yem = emupoint.GetY();

     xem = xem* 1E+4 + 473000;
     yem = yem* 1E+4 - 158000;         
     
     tx = emupoint.GetPx()/emupoint.GetPz();
     ty = emupoint.GetPy()/emupoint.GetPz();  
     tantheta = pow(pow(tx,2) + pow(ty,2),0.5);

     double charge,mass;        

     if ((TDatabasePDG::Instance()->GetParticle(pdgcode))!=NULL){ 
      charge = TDatabasePDG::Instance()->GetParticle(pdgcode)->Charge();
      mass = TDatabasePDG::Instance()->GetParticle(pdgcode)->Mass();
      }
     else{ 
      charge = 0.;
      mass = 0.;
      }
     nbrickhit = FindBrick(emupoint.GetX(), emupoint.GetY(), emupoint.GetZ());
     if(nbrickhit+1 != brickIDs[nbrick]) savehit = false; //saving only one brick at the time
     nfilmhit = emupoint.GetDetectorID(); //getting number of the film, currently it
     double kinenergy = TMath::Sqrt(pow(mass,2)+pow(momentum,2)) - mass;
     // *************EXCLUDE HITS FROM BEING SAVED*******************
     if (nfilmhit > 1000) savehit = false;
     if (tantheta > TMath::Tan(maxtheta)) savehit = false; //we scan from theta 0 to a maximum of 1 rad
     if(charge == 0.) savehit = false; //we do not track neutral particles
     if(momentum < minmomentum) savehit = false; //particles with too low kin energy 
     //saving the hits for a plate in the corresponding couples (only one layer saved, the other has ID + 10000)             
 
     if (useefficiencymap){ //efficiency map with angle
     if(!efficiency(tantheta, heff)) savehit = false; //inserting some holes due to emulsion inefficiency           
     }
     //constant value of efficiency
     else if(!efficiency(emuefficiency)) savehit = false;
     if (dosmearing){ 
      smearing(tx,ty,angres);
     }
     // **************SAVING HIT IN FEDRA BASE-TRACKS****************     
     if (savehit){       
      ect[nbrick][nfilmhit]->eS->Set(ihit,xem,yem,tx,ty,1,Flag);
      ect[nbrick][nfilmhit]->eS->SetMC(inECCevent+flavour_multiplier*iflavour, trackID); //objects used to store MC true information
      ect[nbrick][nfilmhit]->eS->SetAid(motherID, 0); //forcing areaID member to store mother MC track information
      ect[nbrick][nfilmhit]->eS->SetP(momentum);
      ect[nbrick][nfilmhit]->eS->SetVid(pdgcode,0); //forcing viewID[0] member to store pdgcode information
      ect[nbrick][nfilmhit]->eS->SetW(ngrains); //need a high weight to do tracking
      ect[nbrick][nfilmhit]->Fill();
      ihit++; //hit entry, increasing as the tree is filled        
      }
     }//end of loop on emulsion points
    ievent++;
   } //end of loop on tree
  eventlistfile.close();
  } //end loop over flavours
  for (int iplate = 0; iplate < nplates; iplate++){
   //ect[nbrick][iplate]->Write();  
   ect[nbrick][iplate]->Close();  
  }
 cout<<"end of script, saving rootrc wih used parameters"<<endl;
 cenv.WriteFile("FairShip2Fedra.save.rootrc");
 //closing temporary fix file
 TCanvas *c = new TCanvas();
 hbrickID->Draw();

}

void smearing (Float_t &TX, Float_t &TY, const float angres){
 float deltaTX = grandom->Gaus(0,angres); //angular resolution, adding a gaussian offset to TX and TY
 float deltaTY = grandom->Gaus(0,angres);
 //cout<<TX<<endl;
 TX = TX + deltaTX;
 TY = TY + deltaTY;
}

bool efficiency(const float emuefficiency){ //for now, just a constant, to be replaced with an efficiency map (probably with the angle)

 float prob = grandom->Uniform(0,1);
 if (prob < emuefficiency) return true; //efficiency larger than probability, we take the event
 else return false;
}

bool efficiency(const float tantheta, TH1D * emuefficiency){ //for now, just a constant, to be replaced with an efficiency map (probably with the angle)

 float prob = grandom->Uniform(0,1);
 int ibin = emuefficiency->FindBin(tantheta); 
 const float efficiency = emuefficiency->GetBinContent(ibin);
 if (prob < efficiency) return true; //efficiency larger than probability, we take the event
 else return false;
}
/*
script ispirato a void ReadGEMdata da parte di Annarita
*/

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
