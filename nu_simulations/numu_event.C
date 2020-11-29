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
 treechain.Add("/home/utente/Simulations/sim_snd/numuCCDIS_10_11_2020/ship.conical.Genie-TGeant4.root"); 
 treechain.Add("/home/utente/Simulations/sim_snd/anumuCCDIS_10_11_2020/ship.conical.Genie-TGeant4.root");
 
 //setting branches to be read
 TTreeReader reader(&treechain);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");
 TTreeReaderArray<EmulsionDetPoint> emudetpoints(reader,"EmulsionDetPoint");

 TTreeReaderArray<MuFilterPoint> mufilterpoints(reader, "MuFilterPoint");
 
 const int nentries = 1000;

 cout<<"Number of events "<<nentries<<endl;
 //defining histograms

 TH2D *hvxy = new TH2D("hvxy","Position of neutrino interaction vertices in the transverse plane;x[cm];y[cm]", 200,-100,100,200,-100,100);
 TH2D *hvyz = new TH2D("hvyz","Position of neutrino interaction vertices in the zy plane;z[cm];y[cm]",200,-100,100,200,-100,100);
 
 TH2D *hmuyz = new TH2D("hmuyz","Position of Muon Filter hits in the zy plane;z[cm];y[cm]",200,40,240,80,0,80);
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
   /*for (const ShipMCTrack& track: tracks){ 
   }*/ //**************************++*****END OF TRACKS LOOP**************************//
   //*********************************START OF MUON FILTER HITS LOOP************************//
   for (const MuFilterPoint& mufilterpoint:mufilterpoints){
     hmuyz->Fill(mufilterpoint.GetZ(), mufilterpoint.GetY()); 
   } //*******************************END OF MUON FILTER HITS LOOP************************//
 } //*********************************END OF MAIN LOOP************************************//
 //drawing histograms
 TCanvas *cvn = new TCanvas();
 cvn->Divide(1,2);
 cvn->cd(1);
 hvxy->Draw("COLZ");
 cvn->cd(2);
 hvyz->Draw("COLZ");

 TCanvas *cmu = new TCanvas();
 hmuyz->Draw("COLZ");
} //end of main program
