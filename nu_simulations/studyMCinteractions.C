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
//START MAIN PROGRAM

void MakeTree(){

 //reading input file

 TChain treechain("cbmsim");
 int nfirstfile = 1;
 int nlastfile = 10;
 for (int ifile = nfirstfile; ifile<=nlastfile;ifile++){
  treechain.Add(Form("%i/sndLHC.Genie-TGeant4.root",ifile));
 }


 //accessing branches
 TTreeReader reader(&treechain);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");

 //variables to be used in the main code
 int ievent; //event number in tree
 double weight; //statistical weight, related to density and length of tranversed material
 //primary neutrino info
 int nu_pdg;
 double nu_vx, nu_vy, nu_vz;
 double nu_px, nu_py, nu_pz; 

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
     outputtree->Fill();
 }//end event loop

 outputfile->cd();
 outputtree->Write();
 outputfile->Close();
}
