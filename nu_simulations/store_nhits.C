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
  const Double32_t targetxmin = 8.; 
  const Double32_t targetxmax = 49.;
  const Double32_t targetymin = 15.5;
  const Double32_t targetymax = 56.5;
     
  if (nu_vertex.X() > targetxmin && nu_vertex.X() < targetxmax && 
      nu_vertex.Y() > targetymin && nu_vertex.Y() < targetymax) return true;
  else return false;
}

int whichwall(double vz){
  const double zmin0 = -25.4750;
  const double zmax0 = -17.6850;
  const double zmax1 = -6.8950;
  const double zmax2 = 3.8950;
  const double zmax3 = 14.6850;
  const double zmax4 = 25.4750;

  if (vz < zmin0) return -1;
  else if (vz < zmax0) return 0;
  else if (vz < zmax1) return 1;
  else if (vz < zmax2) return 2;
  else if (vz < zmax3) return 3;
  else if (vz < zmax4) return 4;
  else return -1;
}

int whichscifi(double scifihitz){
  const double zmin1 = -17.0933;
  const double zmax1 = -14.0933;
  const double zmax2 = -3.6977;
  const double zmax3 = 6.6978;
  const double zmax4 = 17.0933;
  const double zmax5 = 27.4888;

  if (scifihitz < zmin1) return -1;
  else if (scifihitz < zmax1) return 0;
  else if (scifihitz < zmax2) return 1;
  else if (scifihitz < zmax3) return 2;
  else if (scifihitz < zmax4) return 3;
  else if (scifihitz < zmax5) return 4;
  else return -1;
}

void store_nhits(){
 // what do we need to do?
 bool doscifiloop = true;
 bool domuonloop = false;
 bool writetree = false;
 //input files
 TString filepath("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anueCCDIS_28_11_2020/");
 TChain treechain("cbmsim"); //adding together simulations of neutrinos and antineutrinos
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anueCCDIS_28_11_2020/ship.conical.Genie-TGeant4.root"); 
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/nueCCDIS_28_11_2020/ship.conical.Genie-TGeant4.root");
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/numuCCDIS_10_11_2020/ship.conical.Genie-TGeant4.root");
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anumuCCDIS_10_11_2020/ship.conical.Genie-TGeant4.root");
 treechain.Add((filepath+TString("ship.conical.Genie-TGeant4.root")).Data());
 //paramaters for scifi (geometry needs to be automatically received in future)
 const double xscifimin = 8.0000;
 const double xscifimax = 49.0000;
 const double binsxwidth = 0.0250;
 const int nbinsx = (xscifimax - xscifimin)/binsxwidth;
 const double yscifimin = 15.5000;
 const double yscifimax = 56.5000;
 const double binsywidth = 0.0250; //250 micron
 const int nbinsy = (yscifimax - yscifimin)/binsywidth;

 const int nscifi = 5;
 const int nmax1_nneutrinos = 100;
 int n_neutrinos[nscifi];
 TH2D *hxyscifich[nscifi];
 TH2D *hxyscifich_event[nscifi]; //cleaned in each event
 TH1I *hmaxscifi = new TH1I("hmaxscifi","Number of maximum hits in a channel",50,0,50);
 for (int iscifi=0; iscifi<nscifi;iscifi++){
   n_neutrinos[iscifi] = 0;
   hxyscifich[iscifi] = new TH2D(TString::Format("hxyscifich[%i]",iscifi),"2D distribution scifi channels;x[cm];y[cm]", nbinsx, xscifimin, xscifimax, nbinsy, yscifimin, yscifimax);
   hxyscifich_event[iscifi] = new TH2D(TString::Format("hxyscifich[%i]_event",iscifi),"2D distribution scifi channels per event;x[cm];y[cm]", nbinsx, xscifimin, xscifimax, nbinsy, yscifimin, yscifimax);
 }
 double scifihitz;
 int scifistation;

 TDatabasePDG *pdg = TDatabasePDG::Instance();
 
 //setting branches to be read
 TTreeReader reader(&treechain);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");

 TTreeReaderArray<EmulsionDetPoint> emudetpoints(reader,"EmulsionDetPoint");

 TTreeReaderArray<ScifiPoint> scifipoints(reader,"ScifiPoint");
 TTreeReaderArray<MuFilterPoint> mufilterpoints(reader, "MuFilterPoint");

 const int nentries =10000.;
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
 
 TH1D *hvz_first = new TH1D("hvz_first","VZ of neutrino interaction first wall",13,-26,-13);
 TGraph *gvxy_first = new TGraph();

 TH2D *hscifiyz = new TH2D("hscifiyz","Position of SciFi hits in the zy plane;z[cm];y[cm]",60,-30,30,80,0,80);
 TH2D *hmuyz = new TH2D("hmuyz","Position of Muon Filter hits in the zy plane;z[cm];y[cm]",200,40,240,80,0,80);

 TProfile2D *E_nhits = new TProfile2D("E_nhits", "Energy vs number of Scifi and Mufilter hits;# SciFi Hits; # MuFilter Hits; E[GeV]", 100, 0, 20000, 100, 0, 20000, 0., 10000.);

 TFile *outputfile;
 TTree *outputtree;
 Int_t MCEventID;
 Double_t nu_vx, nu_vy, nu_vz;
 Double_t nu_px, nu_py, nu_pz;
 if (writetree){
  outputfile = new TFile((filepath+TString("nuhits_SND.root")).Data(),"RECREATE");
  outputtree = new TTree("sndhits","Hits from neutrino interactions in SND at LHC");
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
 }
  //***********************************START OF MAIN LOOP*************************//
 for(int ientry = 0;ientry<nentries;ientry++){
  
   if (n_neutrinos[0] >= nmax1_nneutrinos) break; //Antonia asked only for 100 neutrinos in wall1 
   //resetting counters
   nscifihits = 0;
   nmufilterhits = 0;

   //clearing scifi histograms
   for (int iscifi = 0; iscifi < nscifi; iscifi++){
     hxyscifich_event[iscifi]->Reset();
   }
   
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
   
   int nu_wall = whichwall(nu_vz);
   if (nu_wall == 0){ //check first one
     hvz_first->Fill(nu_vz);
     gvxy_first->SetPoint(n_neutrinos[0], nu_vx, nu_vy);
   }
   if (nu_wall >= 0) n_neutrinos[nu_wall]++;

   //*********************************START OF TRACKS LOOP************************//
   /*for (const ShipMCTrack& track: tracks){ 
   }*/ //**************************++*****END OF TRACKS LOOP**************************//
   if(doscifiloop){
   //*********************************START OF SCIFI HITS LOOP************************//
    for (const ScifiPoint& scifipoint:scifipoints){
     pdgcode = scifipoint.PdgCode();
     scifihitz = scifipoint.GetZ();
     charge = GetParticleCharge(pdgcode, pdg);
     if (TMath::Abs(charge) > 0.){
       nscifihits++;
       scifistation = whichscifi(scifihitz);
       if(scifistation >= 0){ 
         if (nu_wall == scifistation) hxyscifich[scifistation]->Fill(scifipoint.GetX(), scifipoint.GetY());
         hxyscifich_event[scifistation]->Fill(scifipoint.GetX(), scifipoint.GetY());
       }
      hscifiyz->Fill(scifipoint.GetZ(), scifipoint.GetY());
     }
    }  //*******************************END OF SCIFI HITS LOOP************************//
   }
   if (domuonloop){
    for (const MuFilterPoint& mufilterpoint:mufilterpoints){
     pdgcode = mufilterpoint.PdgCode();
     detID = mufilterpoint.GetDetectorID();
     charge = GetParticleCharge(pdgcode, pdg);
     if (TMath::Abs(charge) > 0. && detID < 10000.){ //excluding second plane with detID > 10000.
       nmufilterhits++;
       hmuyz->Fill(mufilterpoint.GetZ(), mufilterpoint.GetY());
     }
    } //*******************************END OF MUON FILTER HITS LOOP************************//
   }
   if (ientry % 1000 == 0) cout<<"NSciFi hits "<<nscifihits<<"NMufilter hits "<<nmufilterhits <<" neutrino E "<<neutrinoE<<endl;
   E_nhits->Fill(nscifihits, nmufilterhits, neutrinoE);
   if (writetree) outputtree->Fill();

   if (doscifiloop){ //which is the most full scifi channel?
    int maxscifi = 0;
    for (int iscifi = 0; iscifi< nscifi; iscifi++){
      if (hxyscifich_event[iscifi]->GetMaximum() > maxscifi) maxscifi = hxyscifich_event[iscifi]->GetMaximum();
    }
    hmaxscifi->Fill(maxscifi);
   }

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

 if(writetree){
  outputfile->cd();
  outputtree->Write();
  E_nhits->Write();
  outputfile->Close();
 }
 TCanvas *cscifichannels = new TCanvas();
 cscifichannels->Divide(3,2);
 for (int iscifi = 0; iscifi < nscifi; iscifi++){
  cscifichannels->cd(iscifi+1);
  cout<<"Neutrinos in wall "<<iscifi+1<<" are "<<n_neutrinos[iscifi]<<endl;
  //hxyscifich[iscifi]->Scale(1./n_neutrinos[iscifi]);
  hxyscifich[iscifi]->Draw("COLZ");
  gvxy_first->SetMarkerStyle(kFullCircle);
  gvxy_first->SetMarkerColor(kRed);
  gvxy_first->Draw("SAME");
 }
 TCanvas *cmaxscifi = new TCanvas();
 hmaxscifi->Draw();

 TCanvas *cvz = new TCanvas();
 hvz_first->Draw();
} //end of main program
