// reading nu_mu CCDIS simulations for SHIPatLHC (A.Iuliano, 9 November 2020)

TVector3 NeutrinoVertexCoordinates(const ShipMCTrack &track){
     //building a tvector3 with neutrino vertex coordinates (track is a neutrino interaction daughter)
     double vx = track.GetStartX();
     double vy = track.GetStartY();
     double vz = track.GetStartZ();
    
     return TVector3(vx,vy,vz);
}

void numu_event(){
 //input files
 TChain treechain("cbmsim"); //adding together simulations of neutrinos and antineutrinos
 treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/numu_CCDIS_9_11_20/ship.conical.Genie-TGeant4.root"); 
 treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anumu_CCDIS_9_11_20/ship.conical.Genie-TGeant4.root");
 
 //setting branches to be read
 TTreeReader reader(&treechain);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");
 TTreeReaderArray<EmulsionDetPoint> emudetpoints(reader,"EmulsionDetPoint");
 TTreeReaderArray<SciFiPoint> scifipoints(reader,"SciFiPoint");
 TTreeReaderArray<MuFilterPoint> mufilterpoints(reader, "MuFilterPoint");
 
 const int nentries = reader.GetEntries()<<endl;

 cout<<"Number of events "<<nentries<<endl;
 //defining histograms

 TH2D *hvxy = new TH2D("hvxy","Position of neutrino interaction vertices in the transverse plane;x[cm];y[cm]", 200,-100,100,200,-100,100);
 TH2D *hvyz = new TH2D("hvyz","Position of neutrino interaction vertices in the zy plane;z[cm];y[cm]",200,-100,100,200,-100,100);
 
 //***********************************START OF MAIN LOOP*************************//
 for(int ientry = 0;ientry<nentries;ientry++){
   if (ientry % 10000 == 0) cout<<"arrived at entry "<<ientry<<endl;
   reader.SetEntry(ientry);
   //getting primary vertex position
   TVector3 Vn = NeutrinoVertexCoordinates(tracks[0]);
   Double_t weight = tracks[0].GetWeight();
   hvxy->Fill(Vn[0],Vn[1],weight);
   hvyz->Fill(Vn[2],Vn[1],weight);
   
   //*********************************START OF TRACKS LOOP************************//
   for (const ShipMCTrack& track: tracks){ 
   } //**************************++*****END OF TRACKS LOOP**************************//
   //*********************************START OF MUON FILTER HITS LOOP************************//
   for (const MuFilterPoint& mufilterpoint:mufilterpoints){ 
   } //*******************************END OF MUON FILTER HITS LOOP************************//
 } //*********************************END OF MAIN LOOP************************************//
} //end of main program
