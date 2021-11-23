//script to study MCTrue neutrino interactions (Antonio, 8 November 2021)
//Ideally, this time should organize it into function, so I do not have to write the code each time from scratch
//anytime someone ask me to generate distribution into short time

//used functions


double GetParticleCharge (int pdgcode, TDatabasePDG *pdg){
  //from PDG, get charge
  double charge = 0.;
  if (pdg->GetParticle(pdgcode)) charge = pdg->GetParticle(pdgcode)->Charge();
  else if (pdgcode > 1e+8) charge = 1.; //test storing heavy nuclei
  return charge;
}

bool CheckNeutrinoVertexPosition(TVector3 nu_vertex){
  //target position (to select events in target - values need to be checked for each simulation)
  const Double32_t targetxmin = -47.5000; //instead of -47.6000, 1mm border
  const Double32_t targetxmax = -8.0000;
  const Double32_t targetymin = 15.5000;
  const Double32_t targetymax = 55.0000; //instead of 55.1, 1mm border
     
  if (nu_vertex.X() > targetxmin && nu_vertex.X() < targetxmax && 
      nu_vertex.Y() > targetymin && nu_vertex.Y() < targetymax) return true;
  else return false;
}

bool NeutrinoVertexLocation(int trackID, const ShipMCTrack& track, vector<int> &primary, vector<int> &primaryvisible);


TDatabasePDG *pdg = TDatabasePDG::Instance();
TH1D *hp = new TH1D("hp","Particle momentum at primary vertex;P[GeV/c]",100,0,1000);
TH1D *hn = new TH1D("hn","Number of primaries at primary vertex;Ntracks",50,0,50);

int FindBrick(Float_t hitX, Float_t hitY, Float_t hitZ); //returning which brick the hit belongs to;
//START MAIN PROGRAM

void MakeTree(){

 //reading input file or files

 TChain treechain("cbmsim");
 /*int nfirstfile = 1;
 int nlastfile = 10;
 for (int ifile = nfirstfile; ifile<=nlastfile;ifile++){
  treechain.Add(Form("%i/sndLHC.Genie-TGeant4.root",ifile));
 }*/
 treechain.Add("inECC_sndLHC.Genie-TGeant4.root");


 //accessing branches
 TTreeReader reader(&treechain);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");
 TTreeReaderArray<EmulsionDetPoint> emudetpoints(reader,"EmulsionDetPoint");

 //variables to be used in the main code
 int ievent; //event number in tree
 double weight; //statistical weight, related to density and length of tranversed material
 //primary neutrino info
 int nu_pdg;
 int nu_brickID, nu_filmID;
 double nu_vx, nu_vy, nu_vz;
 double nu_px, nu_py, nu_pz; 

 const double minmom = 0.1;
 const int nemulsionfilms = 60; //nummber of emulsion films in a brick
 int nemupoints[nemulsionfilms];

 const Int_t NDauMax=100;
 //daughters info
 double dau_px[NDauMax], dau_py[NDauMax], dau_pz[NDauMax];
 double dau_charge[NDauMax];
 int dau_pdg[NDauMax];

 int ndaughters;

 const int nentries = reader.GetEntries(true);
 cout<<"Number of events "<<nentries<<endl;
 //normalization
 double totalweight;
 double localizedweight;

 //IDs of tracks of interest (Tracks at neutrino vertex)
 vector<int> primary;
 vector<int> primaryvisible;
 //outputfile with extracted info
 TFile *outputfile = new TFile("nutracks_SND.root","RECREATE");
 TTree *outputtree = new TTree("sndhits","Tracks from neutrino interactions in SND at LHC");
 //eventID
 outputtree->Branch("ievent",&ievent,"ievent/I");
 outputtree->Branch("nu_pdg",&nu_pdg,"nu_pdg/I");
 outputtree->Branch("nu_filmID",&nu_filmID,"nu_filmID/I");
 //neutrino momenta
 outputtree->Branch("nu_px",&nu_px, "nu_px/D");
 outputtree->Branch("nu_py",&nu_py, "nu_py/D");
 outputtree->Branch("nu_pz",&nu_pz, "nu_pz/D");
 //neutrino vertex position
 outputtree->Branch("nu_vx",&nu_vx, "nu_vx/D");
 outputtree->Branch("nu_vy",&nu_vy, "nu_vy/D");
 outputtree->Branch("nu_vz",&nu_vz, "nu_vz/D");
 //info about daughters
 outputtree->Branch("ndaughters",&ndaughters,"ndaughters/I");
 outputtree->Branch("dau_px",&dau_px, "dau_px[ndaughters]/D");
 outputtree->Branch("dau_py",&dau_py, "dau_py[ndaughters]/D");
 outputtree->Branch("dau_pz",&dau_pz, "dau_pz[ndaughters]/D");
 outputtree->Branch("dau_pdg",&dau_pdg, "dau_pdg[ndaughters]/I");
 outputtree->Branch("dau_charge",&dau_charge, "dau_charge[ndaughters]/D");
 //event weigth
 outputtree->Branch("weight",&weight,"weight/D");
 //emulsion hits number
 outputtree->Branch("nemupoints",&nemupoints, "nemupoints[60]/I");



 //***********************************START OF MAIN LOOP*************************//

 for(int ientry = 0;ientry<nentries;ientry++){
   //clearing list of tracks of interest
   primaryvisible.clear();
   primary.clear();

   ndaughters = 0;

   if (ientry % 1000== 0) cout<<"arrived at entry "<<ientry<<endl;
   reader.SetEntry(ientry);

   ievent = ientry;

   //FIRST CONDITION: check if neutrino interaction is within SND target
   nu_vx = tracks[0].GetStartX();
   nu_vy = tracks[0].GetStartY();
   nu_vz = tracks[0].GetStartZ();
   TVector3 Vn = TVector3(nu_vx, nu_vy, nu_vz);

   nu_px = tracks[0].GetPx();
   nu_py = tracks[0].GetPy();
   nu_pz = tracks[0].GetPz();

   nu_pdg = tracks[0].GetPdgCode();

   bool isintarget = CheckNeutrinoVertexPosition(Vn);

   if (!isintarget) continue; //only process events within nu target

   nu_brickID = FindBrick(nu_vx, nu_vy, nu_vz);
   nu_filmID = -1; //number of film after brick ID
      
   weight = tracks[0].GetWeight();
   totalweight += weight;

    //*********************************START TRACK LOOP****************************//
     for (const ShipMCTrack& track: tracks){     
         //look for charged particles from primary vertex

         if (track.GetMotherId()==0){ //daughter of neutrino, increasing counter at the end
          dau_px[ndaughters] = track.GetPx();
          dau_py[ndaughters] = track.GetPy();
          dau_pz[ndaughters] = track.GetPz();

          dau_pdg[ndaughters] = track.GetPdgCode();
          dau_charge[ndaughters] = GetParticleCharge(dau_pdg[ndaughters], pdg);

          ndaughters++;

         }


     } //end track loop
     for (int ifilm = 0; ifilm < nemulsionfilms; ifilm++){
      nemupoints[ifilm] = 0;
     }
     //start loop over emulsion points
     for (const EmulsionDetPoint& emudetpoint: emudetpoints){
       int emu_detID = emudetpoint.GetDetectorID();
       
       double emu_x = emudetpoint.GetX();
       double emu_y = emudetpoint.GetY();
       double emu_z = emudetpoint.GetZ();

       int emu_brickID = FindBrick(emu_x, emu_y, emu_z);

       double emu_momentum = TMath::Sqrt(pow(emudetpoint.GetPx(),2) + pow(emudetpoint.GetPy(),2)+pow(emudetpoint.GetPz(),2));

       int emu_pdgcode = emudetpoint.PdgCode();
       double emu_charge = GetParticleCharge(emu_pdgcode,pdg);       

       if ((emu_z - nu_vz) > 0 && (emu_z - nu_vz < 0.2)) nu_filmID = emu_detID; //we found the film immediately after the interaction

       if (emu_brickID == nu_brickID){//looking only at hits from particles from the same brick
        if (emu_momentum > minmom && TMath::Abs(emu_charge) > 0.){ // visibility selection
         nemupoints[emu_detID]++; //both detID and array members start from 0        
        } //end visibility condition
      }//end brick ID condition


       
       

     } //end loop over emulsion films

     outputtree->Fill();
 }//end event loop

 outputfile->cd();
 outputtree->Write();
 outputfile->Close();
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
} //possible numbers: 10, 11, 12, 13, 20,21,22,23, 30,31,32,33, 40,41,42,43, 50,51,52,53,54//
