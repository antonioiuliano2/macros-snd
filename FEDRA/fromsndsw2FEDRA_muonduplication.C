//load hit from sndsw to FEDRA, duplicating muon background and centering each group to a neutrino vertex (18/02/22)

#include <stdio.h>
#include <TROOT.h>
#include "TRandom.h"

void fromsndsw2FEDRA_muonduplication();
void smearing (Float_t &TX, Float_t &TY, const float angres);
bool efficiency(const float emuefficiency);
bool efficiency(const float tantheta, TH1D * emuefficiency);

int FindBrick(Float_t hitX, Float_t hitY, Float_t hitZ); //returning which brick the hit belongs to;
int FindBrick (Int_t &detID);

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
void fromsndsw2FEDRA_muonduplication(){
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
  int ihit = 0;
  int nfilmhit = 0, nbrickhit = 0;
  float tantheta, momentum;
  int trackID = 0, motherID = 0, pdgcode = 0;
  // ***********************CREATING FEDRA TREES**************************
  gInterpreter->AddIncludePath("/afs/cern.ch/work/a/aiulian/public/fedra/include");
  EdbCouplesTree *ect[nbricks][nplates]; //2D array->which brick and which plate?
  int brickIDs[20] = {11,12,13,14,21,22,23,24,31,32,33,34,41,42,43,44,51,52,53,54};
  
  map <int, int> brickindex;
  //building map 
  brickindex[11] = 0;
  brickindex[12] = 1;
  brickindex[13] = 2;
  brickindex[14] = 3;
  brickindex[21] = 4;
  brickindex[22] = 5;
  brickindex[23] = 6;
  brickindex[24] = 7;
  brickindex[31] = 8;
  brickindex[32] = 9;
  brickindex[33] = 10;
  brickindex[34] = 11;
  brickindex[41] = 12;
  brickindex[42] = 13;
  brickindex[43] = 14;
  brickindex[44] = 15;
  brickindex[51] = 16;
  brickindex[52] = 17;
  brickindex[53] = 18;
  brickindex[54] = 19;
  //builtmap
  TH1I *hbrickID = new TH1I("hbrickID","ID of brick where neutrino interaction happened;brickID",60,0,60);
  
  //60 plates x 20 bricks -> 1200 files! Max number opened files is 1024 by default, need to set ulimit -n 1500 before launching this macro!
  for (int ibrick = 0; ibrick < nbricks; ibrick++){
    for (int iplate = 1; iplate <= nplates; iplate++){
      ect[ibrick][iplate-1] = new EdbCouplesTree();
      ect[ibrick][iplate-1]->InitCouplesTree("couples",Form("b0000%02i/p0%02i/%i.%i.0.0.cp.root",brickIDs[ibrick],iplate,brickIDs[ibrick],iplate),"RECREATE"); //i learned padding with %0i
    }
  }
  Int_t Flag = 1;
  
  const int nfiles = 2;
  const int replaceratio = 1;
  TString inputpaths[nfiles]= {"sndLHC.Ntuple-TGeant4-1E4cm2.root", "inECC_sndLHC.Genie-TGeant4.root"}; //first file muon background, second file neutrino signal
  float evID_multiplier = 1e+3;
  
  //now input files must be open together (for each neutrino, I read all muons), I need two separate reader classes
  //input file: muon background
  TFile * muonbkgfile = TFile::Open((inputpaths[0]).Data());
  TTreeReader muonbkgreader("cbmsim",muonbkgfile);
  TTreeReaderArray<ShipMCTrack> muonbkgtracks(muonbkgreader,"MCTrack");
  TTreeReaderArray<EmulsionDetPoint> muonbkgemulsionhits(muonbkgreader,"EmulsionDetPoint");
  
  //input file: neutrino signal
  TFile * nusignalfile = TFile::Open((inputpaths[1]).Data());
  TTreeReader nusignalreader("cbmsim",nusignalfile);
  TTreeReaderArray<ShipMCTrack> nusignaltracks(nusignalreader,"MCTrack");
  TTreeReaderArray<EmulsionDetPoint> nusignalemulsionhits(nusignalreader,"EmulsionDetPoint");
  
  //number of events in neutrino and background simulations
  //int nevents = nuyield[ifile] * replaceratio;
  int nmuonevents = muonbkgreader.GetEntries();
  int nnuevents = 100;
  const int nmuonsxnu = 100;
  //int nnuevents = nusignalreader.GetEntries();
  
  int nskippednu = 0;
  cout<<"Start processing neutrino events: "<<nnuevents<< " for each, we have muon events: "<<nmuonevents<<endl;  
  
  
  int inECCevent;

  //coordinates of center of muon background (in sndsw system). Can we get them automatically somehow?
  auto  muons2d = TH2F("muons2", "BKG muons starting positions", 40, -18.36, -17.44, 40, 24.20, 25.30); // this doesn't solve the problem
  for (int imu = 0; imu < nmuonevents; imu++){
    muonbkgreader.SetEntry(imu);
    muons2d.Fill(muonbkgtracks[0].GetStartX(), muonbkgtracks[0].GetStartY());
  }
  //const Float_t xcenter_muons = -17.9; //cm
  //const Float_t ycenter_muons = 24.75;
  const Float_t xcenter_muons = muons2d.GetMean(1); //cm
  const Float_t ycenter_muons = muons2d.GetMean(2);

  bool maxreached = false;

  //looping over neutrino events
  for (int inu = 0; inu < nnuevents; inu++){
    
    nusignalreader.SetEntry(inu);
    inECCevent = inu;
    
    int nbrickvertex = FindBrick(nusignaltracks[0].GetStartX(), nusignaltracks[0].GetStartY(), nusignaltracks[0].GetStartZ());
    if(nbrickvertex < 0){
      cout<<"WARNING: NEUTRINO INTERACTION N."<<inu<<"DID NOT HAPPEN IN ANY BRICK! SKIPPED!"<<endl;
      nskippednu++;
      continue;
    }
    int wallnumber_vertex = nbrickvertex/1E1;
    int transversenumber_vertex = (nbrickvertex - wallnumber_vertex*1E1)/1E0;
    
    hbrickID->Fill(nbrickvertex);
    
    //computing offset from neutrino position and center of muons
    
    Float_t xoffset_numuons, yoffset_numuons;
    
    xoffset_numuons = nusignaltracks[0].GetStartX() - xcenter_muons;
    yoffset_numuons = nusignaltracks[0].GetStartY() - ycenter_muons;
    
    //looping over NEUTRINO SIGNAL hits, we take them as they are, no offset
    for (const EmulsionDetPoint& emupoint:nusignalemulsionhits){   
      bool savehit = true; //by default I save all hits
      //no you don't want to do this//     if (j % 2 == 0) continue;
      momentum = TMath::Sqrt(pow(emupoint.GetPx(),2) + pow(emupoint.GetPy(),2) + pow(emupoint.GetPz(),2));
      pdgcode = emupoint.PdgCode();
      trackID = emupoint.GetTrackID();
      bool emubasetrackformat = true;    
      if (trackID >= 0) motherID = nusignaltracks[trackID].GetMotherId();
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
      nfilmhit = emupoint.GetDetectorID(); //getting number of the film, currently it
      //nbrickhit = FindBrick(emupoint.GetX(), emupoint.GetY(), emupoint.GetZ());
      nbrickhit = FindBrick(nfilmhit); 
      
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
	int whichbrick = brickindex[nbrickhit]; //finding index of the array for the brick of our hit;
	ect[whichbrick][nfilmhit]->eS->Set(ihit,xem,yem,tx,ty,1,Flag);
	ect[whichbrick][nfilmhit]->eS->SetMC(inECCevent, trackID); //objects used to store MC true information
	ect[whichbrick][nfilmhit]->eS->SetAid(motherID, 0); //forcing areaID member to store mother MC track information
	ect[whichbrick][nfilmhit]->eS->SetP(momentum);
	ect[whichbrick][nfilmhit]->eS->SetVid(pdgcode,0); //forcing viewID[0] member to store pdgcode information
	ect[whichbrick][nfilmhit]->eS->SetW(ngrains); //need a high weight to do tracking
	ect[whichbrick][nfilmhit]->Fill();
	ihit++; //hit entry, increasing as the tree is filled        
      }
    }//end of loop on emulsion points
    
    
    //accessing muon background events (we are still in the neutrino loop, so we have 1000 muon events for every neutrino)
    
    for (int imuon = 0; imuon < nmuonsxnu; imuon++){
     int muonbkgevent = imuon+inu*nmuonsxnu;
      if(muonbkgevent  >= nmuonevents) {cout << "Number of available bkg muons reached, aborting" << endl; maxreached = true; break;}
      muonbkgreader.SetEntry(muonbkgevent);
      //looping over Muon Background hits, we shift them towards the neutrinos
      
      for (const EmulsionDetPoint& emupoint:muonbkgemulsionhits){   
	bool savehit = true; //by default I save all hits
	//no you don't want to do this//     if (j % 2 == 0) continue;
	momentum = TMath::Sqrt(pow(emupoint.GetPx(),2) + pow(emupoint.GetPy(),2) + pow(emupoint.GetPz(),2));
	pdgcode = emupoint.PdgCode();
	trackID = emupoint.GetTrackID();
	bool emubasetrackformat = true;    
	if (trackID >= 0) motherID = muonbkgtracks[trackID].GetMotherId();
	else motherID = -2; //hope I do not see them
	
	xem = emupoint.GetX();
	yem = emupoint.GetY();
	
	//adding offset, placing them where the neutrino is
	xem = xem + xoffset_numuons;
	yem = yem + yoffset_numuons;
	
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
	nfilmhit = emupoint.GetDetectorID(); //getting number of the film, currently it
	//nbrickhit = FindBrick(emupoint.GetX(), emupoint.GetY(), emupoint.GetZ());
	nbrickhit = FindBrick(nfilmhit); 
	//NOTE: HITS OUTSIDE OF BRICK REGION 1 ARE EXCLUDED (AVOID BRICK CONTAMINATIONS)
	int wallnumber = nbrickhit/1E1;
	int transversenumber = (nbrickhit - wallnumber*1E1)/1E0;
	if (transversenumber > 1) savehit = false;
	//placing it in neutrino transverse brick (wall number can still be different)
	transversenumber = transversenumber_vertex;
	nbrickhit = wallnumber * 1E1 + transversenumber *1E0;
	
	//if(nbrickhit != brickIDs[whichbrick]) savehit = false; //saving only one brick at the time
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
	  int whichbrick = brickindex[nbrickhit]; //finding index of the array for the brick of our hit;
	  ect[whichbrick][nfilmhit]->eS->Set(ihit,xem,yem,tx,ty,1,Flag);
	  ect[whichbrick][nfilmhit]->eS->SetMC(inECCevent+muonbkgevent*evID_multiplier, trackID); //objects used to store MC true information
	  ect[whichbrick][nfilmhit]->eS->SetAid(motherID, 0); //forcing areaID member to store mother MC track information
	  ect[whichbrick][nfilmhit]->eS->SetP(momentum);
	  ect[whichbrick][nfilmhit]->eS->SetVid(pdgcode,0); //forcing viewID[0] member to store pdgcode information
	  ect[whichbrick][nfilmhit]->eS->SetW(ngrains); //need a high weight to do tracking
	  ect[whichbrick][nfilmhit]->Fill();
	  ihit++; //hit entry, increasing as the tree is filled        
	}
      }//end of loop on emulsion points
      

    }//end of loop on muon background events
  if(maxreached == true) break;
  } //end of loop on neutrino signal events
  //CLOSING FILES
  for (int ibrick = 0; ibrick < nbricks; ibrick++){
    for (int iplate = 0; iplate < nplates; iplate++){
      //ect[nbrick][iplate]->Write();  
      ect[ibrick][iplate]->Close();  
    }
  }
  cout<<"end of script, saving rootrc wih used parameters"<<endl;
  cenv.WriteFile("FairShip2Fedra.save.rootrc");
  cout<<"We have skipped "<<nskippednu<< "neutrinos over "<<nnuevents<<endl;
  //drawing brickID histo
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
  float z0_start = 290.7368; float z0_end = 298.8862;
  float z1_start = 303.7358; float z1_end = 311.8852;
  float z2_start = 316.7348;  float z2_end = 324.8842;
  float z3_start = 329.7338;   float z3_end = 337.8832;
  float z4_start = 342.7328;  float z4_end = 350.8822;
  
  if (hitZ > z0_start && hitZ < z0_end) nz = 1;
  else if(hitZ > z1_start && hitZ < z1_end) nz = 2;
  else if(hitZ > z2_start && hitZ < z2_end) nz = 3;
  else if(hitZ > z3_start && hitZ < z3_end) nz = 4;
  else if(hitZ > z4_start && hitZ < z4_end) nz=5;
  else nz = -10; //not in a brick
  
  int nbrick = nx + ny*2 + 10 * nz;
  return nbrick+1;
} //possible numbers: 11, 12, 13, 14, 21,22,23,24, 31,32,33,34, 41,42,43,44, 50,51,52,53,54//

int FindBrick (Int_t &detID){
  
  int nbrick = detID/1E3;
  int NPlate = detID - nbrick*1E3;
  
  detID = NPlate -1 ; //for conversion (from 0 to 59)
  return nbrick;
}
