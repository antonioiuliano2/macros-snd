// reading nu_mu CCDIS simulations for SHIPatLHC (A.Iuliano, 9 November 2020)

double GetParticleCharge (int pdgcode, TDatabasePDG *pdg){
  //from PDG, get charge
  double charge = 0.;
  if (pdg->GetParticle(pdgcode)) charge = pdg->GetParticle(pdgcode)->Charge();
  else if (pdgcode > 1e+8) charge = 1.; //test storing heavy nuclei
  return charge;
}

bool CheckNeutrinoVertexPosition(TVector3 nu_vertex){
  //target position (to select events in target - values need to be checked for each simulation)
  const Double32_t targetxmin = 5.; 
  const Double32_t targetxmax = 50.;
  const Double32_t targetymin = 15.;
  const Double32_t targetymax = 59.;
     
  if (nu_vertex.X() > targetxmin && nu_vertex.X() < targetxmax && 
      nu_vertex.Y() > targetymin && nu_vertex.Y() < targetymax) return true;
  else return false;
}

void store_nhits(){
 //input files
 TString filepath("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anueCCDIS_28_11_2020/");
 TChain treechain("cbmsim"); //adding together simulations of neutrinos and antineutrinos
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anueCCDIS_28_11_2020/ship.conical.Genie-TGeant4.root"); 
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/nueCCDIS_28_11_2020/ship.conical.Genie-TGeant4.root");
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/numuCCDIS_10_11_2020/ship.conical.Genie-TGeant4.root");
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anumuCCDIS_10_11_2020/ship.conical.Genie-TGeant4.root");
 treechain.Add((filepath+TString("ship.conical.Genie-TGeant4.root")).Data());
 
 TDatabasePDG *pdg = TDatabasePDG::Instance();
 
 //setting branches to be read
 TTreeReader reader(&treechain);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");

 TTreeReaderArray<EmulsionDetPoint> emudetpoints(reader,"EmulsionDetPoint");

 TTreeReaderArray<ScifiPoint> scifipoints(reader,"ScifiPoint");
 TTreeReaderArray<MuFilterPoint> mufilterpoints(reader, "MuFilterPoint");

 const int nentries =100000.;
 //const int nentries =treechain.GetEntries();
 //treechain.LoadTree(-1); //loading first tree of the chain, preparing for loop (otherwise root prints warning due to the GetEntries() before this line)

 cout<<"Number of events "<<nentries<<endl;

 //variables for energy resolution estimation
 Int_t nscifihits, nmufilterhits;
 Double_t neutrinoE, weight;

 //variables for hits

 int pdgcode, detID, trackID;
 double charge;
 
 //defining histograms

 TH2D *hvxy = new TH2D("hvxy","Position of neutrino interaction vertices in the transverse plane;x[cm];y[cm]", 200,-100,100,200,-100,100);
 TH2D *hvyz = new TH2D("hvyz","Position of neutrino interaction vertices in the zy plane;z[cm];y[cm]",200,-100,100,200,-100,100);
 
 TH2D *hscifiyz = new TH2D("hscifiyz","Position of SciFi hits in the zy plane;z[cm];y[cm]",60,-30,30,80,0,80);
 TH2D *hmuyz = new TH2D("hmuyz","Position of Muon Filter hits in the zy plane;z[cm];y[cm]",200,40,240,80,0,80);

 TProfile2D *E_nhits = new TProfile2D("E_nhits", "Energy vs number of Scifi and Mufilter hits;# SciFi Hits; # MuFilter Hits; E[GeV]", 100, 0, 20000, 100, 0, 20000, 0., 10000.);

 TFile *outputfile = new TFile((filepath+TString("nuhits_SND.root")).Data(),"RECREATE");
 TTree *outputtree = new TTree("sndhits","Hits from neutrino interactions in SND at LHC");

 Int_t MCEventID;
 Double_t nu_vx, nu_vy, nu_vz;
 Double_t nu_px, nu_py, nu_pz;
 //eventID
 outputtree->Branch("MCEventID",&MCEventID,"MCEventID/I");
 //neutrino momenta
 outputtree->Branch("nu_px",&nu_px, "nu_px/D");
 outputtree->Branch("nu_py",&nu_py, "nu_py/D");
 outputtree->Branch("nu_pz",&nu_pz, "nu_pz/D");
 //neutrino vertex position
 outputtree->Branch("nu_vx",&nu_vx, "nu_vx/D");
 outputtree->Branch("nu_vy",&nu_vy, "nu_vy/D");
 outputtree->Branch("nu_vz",&nu_vz, "nu_vz/D");
 //event weigth
 outputtree->Branch("weight",&weight,"weight/D");
 
 outputtree->Branch("nmufilterhits",&nmufilterhits, "nmufilterhits/I");
 outputtree->Branch("nscifihits",&nscifihits, "nscifihits/I");
 //***********************************START OF MAIN LOOP*************************//
 for(int ientry = 0;ientry<nentries;ientry++){
  
   //resetting counters
   nscifihits = 0;
   nmufilterhits = 0;
   
   if (ientry % 10000== 0) cout<<"arrived at entry "<<ientry<<endl;
   reader.SetEntry(ientry);
   MCEventID = ientry;
   //getting neutrino energy;
   neutrinoE = tracks[0].GetEnergy();
   //getting primary vertex position
   nu_vx = tracks[0].GetStartX();
   nu_vy = tracks[0].GetStartY();
   nu_vz = tracks[0].GetStartZ();
   TVector3 Vn = TVector3(nu_vx, nu_vy, nu_vz);
   //getting neutrino trimomentum
   nu_px = tracks[0].GetPx();
   nu_py = tracks[0].GetPy();
   nu_pz = tracks[0].GetPz();
   
   weight = tracks[0].GetWeight();
   hvxy->Fill(Vn[0],Vn[1],weight);
   hvyz->Fill(Vn[2],Vn[1],weight);

   bool isintarget = CheckNeutrinoVertexPosition(Vn);

   if (!isintarget) continue; //geometrical acceptance
   
   //*********************************START OF TRACKS LOOP************************//
   /*for (const ShipMCTrack& track: tracks){ 
   }*/ //**************************++*****END OF TRACKS LOOP**************************//
   //*********************************START OF SCIFI HITS LOOP************************//
   for (const ScifiPoint& scifipoint:scifipoints){
     pdgcode = scifipoint.PdgCode();
     charge = GetParticleCharge(pdgcode, pdg);
     if (TMath::Abs(charge) > 0.){
       nscifihits++;
       hscifiyz->Fill(scifipoint.GetZ(), scifipoint.GetY());
     }
   }  //*******************************END OF SCIFI HITS LOOP************************//

   for (const MuFilterPoint& mufilterpoint:mufilterpoints){
     pdgcode = mufilterpoint.PdgCode();
     detID = mufilterpoint.GetDetectorID();
     charge = GetParticleCharge(pdgcode, pdg);
     if (TMath::Abs(charge) > 0. && detID < 10000.){ //excluding second plane with detID > 10000.
       nmufilterhits++;
       hmuyz->Fill(mufilterpoint.GetZ(), mufilterpoint.GetY());
     }
   } //*******************************END OF MUON FILTER HITS LOOP************************//
   if (ientry % 1000 == 0) cout<<"NSciFi hits "<<nscifihits<<"NMufilter hits "<<nmufilterhits <<" neutrino E "<<neutrinoE<<endl;
   E_nhits->Fill(nscifihits, nmufilterhits, neutrinoE);
   outputtree->Fill();
 } //*********************************END OF MAIN LOOP************************************//
 //drawing histograms
 TCanvas *cvn = new TCanvas();
 cvn->Divide(1,2);
 cvn->cd(1);
 hvxy->Draw("COLZ");
 cvn->cd(2);
 hvyz->Draw("COLZ");

 TCanvas *cscifi = new TCanvas();
 hscifiyz->Draw("COLZ");

 TCanvas *cmu = new TCanvas();
 hmuyz->Draw("COLZ");

 TCanvas *cenergy = new TCanvas();
 E_nhits->Draw("LEGO");

 outputfile->cd();
 outputtree->Write();
 E_nhits->Write();
 outputfile->Close();
 
} //end of main program
