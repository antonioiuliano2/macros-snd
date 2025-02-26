//load hit from sndsw to FEDRA, duplicating muon background and centering each group to a neutrino vertex (18/02/22)

// #include <stdio.h>
// #include <TROOT.h>
// #include "TRandom.h"
//#include "/home/utente/fedra/include/EdbCouplesTree.h"

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
  cenv.SetValue("FairShip2Fedra.maxtheta",1.); //angular max of scanning
  cenv.SetValue("FairShip2Fedra.minmomentum",0.1); //do not pass particles beyond this value, track ID would be -2
  cenv.SetValue("FairShip2Fedra.ngrains",70.); // to set weight
  cenv.SetValue("FairShip2Fedra.angres",0.003);//used for smearing, if dosmearing = true
  cenv.SetValue("FairShip2Fedra.useLocalBrickCoordinates",1); //local brick coordinates
}

using namespace TMath;
TRandom3 *grandom = new TRandom3(); //creating every time a TRandom3 is a bad idea
TFile *file = NULL;
TH1D *heff = NULL ; //efficiency at different angles
EmulsionDet emureader;
//starting script
void fromsndsw2FEDRA_muonduplication(){
  TString geofilename = "geofile_sndlhc_TI18.root";
  gGeoManager->Import(geofilename);
  grandom->SetSeed(0);
  
  TEnv cenv("FairShip2Fedra");
  set_default(cenv);
  //getting options from file
  cenv.ReadFile("FairShip2Fedra.rootrc" ,kEnvLocal);
  
  //const Int_t nevents = cenv.GetValue("FairShip2Fedra.nevents",10000);
  const int nplates = cenv.GetValue("FairShip2Fedra.nplates",60);
  const int nbricks = cenv.GetValue("FairShip2Fedra.nbricks",20); // to set b00000%i number
  float angres = cenv.GetValue("FairShip2Fedra.angres",0.003); //Used cases: 3, 5milliradians. Constant value overwritten if useresfunction=true
  float minmomentum = cenv.GetValue("FairShip2Fedra.minmomentum",0.1);
  float maxtheta = cenv.GetValue("FairShip2Fedra.maxtheta",1.);
  const float ngrains = cenv.GetValue("FairShip2Fedra.ngrains",70.) ; //the same number for all the couples, so they have the same weigth.
  const float emuefficiency = cenv.GetValue("FairShip2Fedra.emuefficiency",0.85); // flat value
  const bool useefficiencymap = cenv.GetValue("FairShip2Fedra.useefficiencymap",0); //use the map instead of the constant value down
  const bool dosmearing = cenv.GetValue("FairShip2Fedra.dosmearing",1); //gaussian smearing or not
  const bool useLocalBrickCoordinates = cenv.GetValue("FairShip2Fedra.useLocalBrickCoordinates",1); //local brick coordinates
  
  cout<<"Starting conversion with efficiency "<<emuefficiency<<" maxtheta "<<maxtheta<<" and min momentum "<<minmomentum<<endl;
  
  if (useefficiencymap){ 
    file = TFile::Open("efficiency_alltracks.root");
    heff = (TH1D*) file->Get("heff");
  }
  
  //const Int_t nevents = reader.GetTree()->GetEntries();
  int ihit = 0;
  int nfilmhit = 0, nbrick = 0;
  float tantheta, momentum;
  int trackID = 0, motherID = 0, pdgcode = 0;
  
  map <int, int> brickindex;
  //building map
  int index = 0;
  for (int wall = 1; wall <= 5; ++wall) {
    for (int brick = 1; brick <= 4; ++brick) {
      if (index>nbricks) break;
      int id = wall * 10 + brick;
      brickindex[id] = index;
      ++index;
      if (index>=nbricks) break;
    }
    if (index>=nbricks) break;
  }
  
  // ***********************CREATING FEDRA TREES**************************
  // gInterpreter->AddIncludePath("/afs/cern.ch/work/a/aiulian/public/fedra/include");
  EdbCouplesTree *ect[nbricks][nplates]; //2D array->which brick and which plate?
  TH1I *hbrickID = new TH1I("hbrickID","ID of brick where neutrino interaction happened;brickID",60,0,60);
  
  //60 plates x 20 bricks -> 1200 files! Max number opened files is 1024 by default, need to set ulimit -n 1500 before launching this macro!
  int ibrick = 0;
    for (const auto& [brickID, idx] : brickindex) {
        for (int i = 1; i <= nplates; i++) {
            ect[ibrick][i-1] = new EdbCouplesTree();
            ect[ibrick][i-1]->InitCouplesTree("couples", Form("b%06i/p%03i/%i.%i.0.0.cp.root", brickID, i, brickID, i), "RECREATE");
        }
        ibrick++;
    }

  const int nfiles = 2;
  const int replaceratio = 1;
  TString inputpaths[nfiles]= {"sndLHC.Ntuple-TGeant4-1E5cm2.root", "inECC_sndLHC.Genie-TGeant4.root"}; //first file muon background, second file neutrino signal
  float evID_multiplier = 2000;
  
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
  int nnuevents = nusignalreader.GetEntries();
  // int nnuevents = 100;
  const int nmuonsxnu = nmuonevents;
  
  int nskippednu = 0;
  cout<<"Start processing neutrino events: "<<nnuevents<< ", for each we have muon events: "<<nmuonevents<<endl;  
  
  int inECCevent;

  //coordinates of center of muon background (in sndsw system). Can we get them automatically somehow?
  // auto  muons2d = TH2F("muons2", "BKG muons starting positions", 40, -18.36, -17.44, 40, 24.20, 25.30); // this doesn't solve the problem
  // for (int imu = 0; imu < nmuonevents; imu++){
  //   muonbkgreader.SetEntry(imu);
  //   muons2d.Fill(muonbkgtracks[0].GetStartX(), muonbkgtracks[0].GetStartY());
  // }
  // const Float_t xcenter_muons = muons2d.GetMean(1); //cm
  // const Float_t ycenter_muons = muons2d.GetMean(2);
  const Float_t xcenter_muons = -17.9; //cm
  const Float_t ycenter_muons = 24.75;

  bool maxreached = false;


  // TStopwatch stopWatch;
  // stopWatch.Start();
  //looping over neutrino events
  for (int inu = 0; inu < nnuevents; inu++){
    
    nusignalreader.SetEntry(inu);
    inECCevent = inu;
    
    int nbrickvertex = FindBrick(nusignaltracks[0].GetStartX(), nusignaltracks[0].GetStartY(), nusignaltracks[0].GetStartZ());
    // cout<<"processing event "<< inu << " in brick " <<nbrickvertex << " time " <<  stopWatch.RealTime() <<endl;
    if(nbrickvertex !=11){
      cout<<"WARNING: NEUTRINO INTERACTION N. "<<inu<<" DID NOT HAPPEN IN ANY BRICK! SKIPPED!"<<endl;
      nskippednu++;
      continue;
    }
    // stopWatch.Continue();
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
      int detID = emupoint.GetDetectorID();
      float eloss = emupoint.GetEnergyLoss();
      pdgcode = emupoint.PdgCode();
      trackID = emupoint.GetTrackID();
      if (trackID >= 0) motherID = nusignaltracks[trackID].GetMotherId();
      else motherID = -2; //hope I do not see them
      
      int NWall = 0, NRow = 0, NColumn = 0, NPlate = 0;
      emureader.DecodeBrickID(detID, NWall, NRow, NColumn, NPlate); //getting info about brick
      int nbrick = (detID - NPlate)*1e-3;
      if (brickindex.find(nbrick) == brickindex.end()) continue; //skip events not happening in the selected bricks
      if (nbrick!=nbrickvertex) continue;
            
      double charge,mass;         
      if ((TDatabasePDG::Instance()->GetParticle(pdgcode))!=NULL){ 
        charge = TDatabasePDG::Instance()->GetParticle(pdgcode)->Charge();
        mass = TDatabasePDG::Instance()->GetParticle(pdgcode)->Mass();
      }
      else{ 
        charge = 0.;
        mass = 0.;
      }
      double kinenergy = TMath::Sqrt(pow(mass,2)+pow(momentum,2)) - mass;

      float xem, yem, tx, ty;
      //first, convert positions
      if (useLocalBrickCoordinates){ //each brick has its own reference system
        double globalpos[3] = {emupoint.GetX(),emupoint.GetY(),emupoint.GetZ()};
        double localpos[3] = {0,0,0};

        emureader.GetLocalPosition(detID, globalpos, localpos);

        xem = localpos[0];
        yem = localpos[1];

        //second, convert angles
        double globalang[3] = {emupoint.GetPx(),emupoint.GetPy(),emupoint.GetPz()};
        double localang[3] = {0,0,0};

        emureader.GetLocalAngles(detID, globalang, localang);
        //angles in TX, TY format
        tx = localang[0]/localang[2];
        ty = localang[1]/localang[2];
      }
      else{ //global reference system as sndsw, but in FEDRA units
        xem = emupoint.GetX();
        yem = emupoint.GetY();

        xem = xem* 1E+4 + 473000;
        yem = yem* 1E+4 - 158000;         
      
        tx = emupoint.GetPx()/emupoint.GetPz();
        ty = emupoint.GetPy()/emupoint.GetPz();  
      }
      float tantheta = pow(pow(tx,2) + pow(ty,2),0.5);      
       
      // *************EXCLUDE HITS FROM BEING SAVED*******************
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
        int whichbrick = brickindex[nbrick]; //finding index of the array for the brick of our hit;
        ect[whichbrick][NPlate-1]->eS->Set(ihit,xem,yem,tx,ty,1,1);
        ect[whichbrick][NPlate-1]->eS->SetMC(inECCevent, trackID); //objects used to store MC true information
        ect[whichbrick][NPlate-1]->eS->SetAid(motherID, 0); //forcing areaID member to store mother MC track information
        ect[whichbrick][NPlate-1]->eS->SetP(momentum);
        ect[whichbrick][NPlate-1]->eS->SetVid(pdgcode,0); //forcing viewID[0] member to store pdgcode information
        ect[whichbrick][NPlate-1]->eS->SetW(ngrains); //need a high weight to do tracking
        ect[whichbrick][NPlate-1]->eS->SetDZem(eloss); //energy loss
        ect[whichbrick][NPlate-1]->Fill();
        ihit++; //hit entry, increasing as the tree is filled        
      }
    }//end of loop on emulsion points
    
    //accessing muon background events (we are still in the neutrino loop, so we have 1000 muon events for every neutrino)
    
    for (int imuon = 0; imuon < nmuonsxnu; imuon++){
      if (imuon%10000==0) cout<<"processing event "<< inu <<" muon "<<imuon<<endl;
      int muonbkgevent = imuon;
      if(muonbkgevent  >= nmuonevents) {cout << "Number of available bkg muons reached, aborting" << endl; maxreached = true; break;}
      muonbkgreader.SetEntry(muonbkgevent);
      //looping over Muon Background hits, we shift them towards the neutrinos
      
      // cout<<"processing event "<< inu <<" muon "<<imuon << " time " <<  stopWatch.RealTime()<< endl;
      for (const EmulsionDetPoint& emupoint:muonbkgemulsionhits){   
        bool savehit = true; //by default I save all hits
        //no you don't want to do this//     if (j % 2 == 0) continue;
        int detID = emupoint.GetDetectorID();
        float eloss = emupoint.GetEnergyLoss();
        momentum = TMath::Sqrt(pow(emupoint.GetPx(),2) + pow(emupoint.GetPy(),2) + pow(emupoint.GetPz(),2));
        pdgcode = emupoint.PdgCode();
        trackID = emupoint.GetTrackID();
        if (trackID >= 0) motherID = muonbkgtracks[trackID].GetMotherId();
        else motherID = -2; //hope I do not see them
        
        int NWall = 0, NRow = 0, NColumn = 0, NPlate = 0;
        emureader.DecodeBrickID(detID, NWall, NRow, NColumn, NPlate); //getting info about brick
        int nbrick = (detID - NPlate)*1e-3;
        if (brickindex.find(nbrick) == brickindex.end()) continue; //skip events not happening in the selected bricks
        if (nbrick!=nbrickvertex) continue;
        double charge,mass;         
        if ((TDatabasePDG::Instance()->GetParticle(pdgcode))!=NULL){ 
          charge = TDatabasePDG::Instance()->GetParticle(pdgcode)->Charge();
          mass = TDatabasePDG::Instance()->GetParticle(pdgcode)->Mass();
        }
        else{ 
          charge = 0.;
          mass = 0.;
        }
        double kinenergy = TMath::Sqrt(pow(mass,2)+pow(momentum,2)) - mass;

        float xem, yem, tx, ty;
        //first, convert positions
        if (useLocalBrickCoordinates){ //each brick has its own reference system
          double globalpos[3] = {emupoint.GetX() + xoffset_numuons,emupoint.GetY() + yoffset_numuons,emupoint.GetZ()};
          double localpos[3] = {0,0,0};

          emureader.GetLocalPosition(detID, globalpos, localpos);

          xem = localpos[0];
          yem = localpos[1];

          //second, convert angles
          double globalang[3] = {emupoint.GetPx(),emupoint.GetPy(),emupoint.GetPz()};
          double localang[3] = {0,0,0};

          emureader.GetLocalAngles(detID, globalang, localang);
          //angles in TX, TY format
          tx = localang[0]/localang[2];
          ty = localang[1]/localang[2];
        }
        else{ //global reference system as sndsw, but in FEDRA units
          xem = emupoint.GetX();
          yem = emupoint.GetY();

          xem = xem + xoffset_numuons;
	        yem = yem + yoffset_numuons;

          xem = xem* 1E+4 + 473000;
          yem = yem* 1E+4 - 158000;         
        
          tx = emupoint.GetPx()/emupoint.GetPz();
          ty = emupoint.GetPy()/emupoint.GetPz();  
        }
        float tantheta = pow(pow(tx,2) + pow(ty,2),0.5);      

        //if(nbrick != brickIDs[whichbrick]) savehit = false; //saving only one brick at the time
        // *************EXCLUDE HITS FROM BEING SAVED*******************
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
          int whichbrick = brickindex[nbrick]; //finding index of the array for the brick of our hit;
          ect[whichbrick][NPlate-1]->eS->Set(ihit,xem,yem,tx,ty,1,0);
          ect[whichbrick][NPlate-1]->eS->SetMC(inECCevent, trackID); //objects used to store MC true information
          ect[whichbrick][NPlate-1]->eS->SetAid(motherID, 0); //forcing areaID member to store mother MC track information
          ect[whichbrick][NPlate-1]->eS->SetP(momentum);
          ect[whichbrick][NPlate-1]->eS->SetVid(pdgcode,0); //forcing viewID[0] member to store pdgcode information
          ect[whichbrick][NPlate-1]->eS->SetW(ngrains); //need a high weight to do tracking
          ect[whichbrick][NPlate-1]->eS->SetDZem(eloss); //need a high weight to do tracking
          ect[whichbrick][NPlate-1]->Fill();
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