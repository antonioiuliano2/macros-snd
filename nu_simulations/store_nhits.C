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
  const Double32_t targetxmin = -47.5000; //instead of -47.6000, 1mm border
  const Double32_t targetxmax = -8.0000;
  const Double32_t targetymin = 15.5000;
  const Double32_t targetymax = 55.0000; //instead of 55.1, 1mm border
     
  if (nu_vertex.X() > targetxmin && nu_vertex.X() < targetxmax && 
      nu_vertex.Y() > targetymin && nu_vertex.Y() < targetymax) return true;
  else return false;
}

bool HitWithinNMoliere(double vx, double vy, double hitx, double hity){
  const double W_RM = 0.9327; //Molière Radius in cm (https://pdg.lbl.gov/2020/AtomicNuclearProperties/HTML/tungsten_W.html)
  const int Nmax_RM = 3;
  double R_dist = TMath::Sqrt(pow(hitx - vx,2) + pow(hity - vy, 2));
  if (R_dist < Nmax_RM * W_RM) return true;
  else return false;
}

bool HitWithinNMoliere1D(double vx, double hitx){
  const double W_RM = 0.9327; //Molière Radius in cm (https://pdg.lbl.gov/2020/AtomicNuclearProperties/HTML/tungsten_W.html)
  const int Nmax_RM = 3;
  double dist = hitx - vx;
  if (TMath::Abs(dist) < Nmax_RM * W_RM) return true;
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

  return -1; //even if redudant, needed to avoid warnings
}

int whichscifi(double scifihitz){
  const double zmin0 = -17.6850;
  const double zmax0 = -14.6850;
  const double zmax1 = -3.8950;
  const double zmax2 =  6.8950;
  const double zmax3 = 17.6850;
  const double zmax4 = 28.4750;

  if (scifihitz < zmin0) return -1;
  else if (scifihitz < zmax0) return 0;
  else if (scifihitz < zmax1) return 1;
  else if (scifihitz < zmax2) return 2;
  else if (scifihitz < zmax3) return 3;
  else if (scifihitz < zmax4) return 4;
  else return -1;

  return -1; //even if redudant, needed to avoid warnings
}

void store_nhits(){
 // what do we need to do?
 bool doscifiloop = true;
 bool domuonloop = false;
 bool writetree = false;
 bool writescifihistograms = true;
 bool require3moliere = true; //scifi hits are only stored if within 3 molière radii (Request by De Lellis)
 const int nmax1_nneutrinos = 100; //maximum number of SciFi hits in first wall
 //input files
 TString filepath("./");
 TChain treechain("cbmsim"); //adding together simulations of neutrinos and antineutrinos
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anueCCDIS_28_11_2020/ship.conical.Genie-TGeant4.root"); 
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/nueCCDIS_28_11_2020/ship.conical.Genie-TGeant4.root");
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/numuCCDIS_10_11_2020/ship.conical.Genie-TGeant4.root");
 //treechain.Add("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/anumuCCDIS_10_11_2020/ship.conical.Genie-TGeant4.root");
 treechain.Add((filepath+TString("ship.conical.Genie-TGeant4.root")).Data());
 //const int nentries =10000.;
 const int nentries =treechain.GetEntries();
 //paramaters for scifi (geometry needs to be automatically received in future)
 const double xscifimin = -47.5000;
 const double xscifimax = -8.0000;
 const double binsxwidth = 0.0250;
 const int nbinsx = (xscifimax - xscifimin)/binsxwidth;
 const double yscifimin = 15.5000;
 const double yscifimax = 55.0000;
 const double binsywidth = 0.0250; //250 micron
 const int nbinsy = (yscifimax - yscifimin)/binsywidth;

 const int nscifi = 5;
 int n_neutrinos[nscifi];
 TH2D *hxyscifich[nscifi];
 TH1D *hxscifich_event[nscifi], *hyscifich_event[nscifi]; //cleaned in each event
 TH1I *hmaxscifix = new TH1I("hmaxscifix","Number of maximum hits in a channel along x",200,0,200);
 TH1I *hmaxscifiy = new TH1I("hmaxscifiy","Number of maximum hits in a channel along y",200,0,200);
 TH1D *hnxscifich[nscifi], *hnyscifich[nscifi];
 for (int iscifi=0; iscifi<nscifi;iscifi++){
   n_neutrinos[iscifi] = 0;
   hxyscifich[iscifi] = new TH2D(TString::Format("hxyscifich%i",iscifi),"2D distribution scifi channels;x[cm];y[cm]", nbinsx, xscifimin, xscifimax, nbinsy, yscifimin, yscifimax);
   hnxscifich[iscifi] = new TH1D(TString::Format("hnxscifich%i",iscifi),"Number of hits per X channels",50,0, 50);
   hnyscifich[iscifi] = new TH1D(TString::Format("hnyscifich%i",iscifi),"Number of hits per Y channels",50,0, 50);
   hxscifich_event[iscifi] = new TH1D(TString::Format("hxscifich%i_event",iscifi),"X Distribution scifi channels per event;x[cm]", nbinsx, xscifimin, xscifimax);
   hyscifich_event[iscifi] = new TH1D(TString::Format("hyscifich%i_event",iscifi),"Y Distribution scifi channels per event;y[cm]", nbinsy, yscifimin, yscifimax);
 }
 double scifihitx, scifihity, scifihitz;
 int scifistation;

 TDatabasePDG *pdg = TDatabasePDG::Instance();
 
 //setting branches to be read
 TTreeReader reader(&treechain);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");

 TTreeReaderArray<EmulsionDetPoint> emudetpoints(reader,"EmulsionDetPoint");

 TTreeReaderArray<ScifiPoint> scifipoints(reader,"ScifiPoint");
 TTreeReaderArray<MuFilterPoint> mufilterpoints(reader, "MuFilterPoint");

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

 TFile *histofile;
 TFile *outputfile;
 TTree *outputtree;
 Int_t MCEventID;
 Double_t nu_vx, nu_vy, nu_vz;
 Double_t nu_px, nu_py, nu_pz;
 if (writescifihistograms){
  histofile = new TFile((filepath+TString("scifihistos_100events_3moliere.root")).Data(),"RECREATE");
 }
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

   //clearing scifi histograms per event
   for (int iscifi = 0; iscifi < nscifi; iscifi++){
     hxscifich_event[iscifi]->Reset();
     hyscifich_event[iscifi]->Reset();
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
     scifihitx = scifipoint.GetX();
     scifihity = scifipoint.GetY();
     scifihitz = scifipoint.GetZ();
     bool withinevent = true;
     //bool withinevent = HitWithinNMoliere(nu_vx, nu_vy, scifihitx, scifihity); //is the hit within three molère radii?
     if (!require3moliere) withinevent = true; //if not asked, accept everything

     charge = GetParticleCharge(pdgcode, pdg);
     
     if (TMath::Abs(charge) > 0. && withinevent){
       nscifihits++;
       scifistation = whichscifi(scifihitz);
       if(scifistation >= 0){ 
         if (nu_wall == scifistation){ 
           hxyscifich[scifistation]->Fill(scifihitx, scifihity);
           hxscifich_event[scifistation]->Fill(scifihitx);
           hyscifich_event[scifistation]->Fill(scifihity);
         }
       }
      hscifiyz->Fill(scifihitz, scifihity);
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
    int maxscifix = 0, maxscifiy = 0;
    for (int iscifi = 0; iscifi< nscifi; iscifi++){

      if (nu_wall != iscifi) continue; //filling only scifi at wall immediately after nu interaction!
      
      if (hxscifich_event[iscifi]->GetMaximum() > maxscifix) maxscifix = hxscifich_event[iscifi]->GetMaximum();
      if (hyscifich_event[iscifi]->GetMaximum() > maxscifiy) maxscifiy = hxscifich_event[iscifi]->GetMaximum();
      
      //loop over bins, how many per bin (tracks per channel), fraction over nchannels?
      for (int ibin = 1; ibin <= nbinsx; ibin++){ 
        int nx = hxscifich_event[iscifi]->GetBinContent(ibin);
        if (HitWithinNMoliere1D(nu_vx, hxscifich_event[iscifi]->GetBinCenter(ibin)) || !require3moliere) 
         hnxscifich[iscifi]->Fill(nx);
      }
      for (int ibin = 1; ibin <= nbinsy; ibin++){
        int ny = hyscifich_event[iscifi]->GetBinContent(ibin);
        if (HitWithinNMoliere1D(nu_vy, hyscifich_event[iscifi]->GetBinCenter(ibin)) || !require3moliere) 
         hnyscifich[iscifi]->Fill(ny);
      }

    }
    if(nu_wall == 0){
     hmaxscifix->Fill(maxscifix);
     hmaxscifiy->Fill(maxscifiy);
    }
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
 gvxy_first->SetMarkerStyle(kFullCircle);
 gvxy_first->SetMarkerColor(kRed);
 gvxy_first->SetTitle("xy distribution of neutrino vertices in first wall");
 gvxy_first->SetName("gvxy_first");
 //drawing plots in scifi hits (overlapping hits in first wall)
 TCanvas *cscifichannels = new TCanvas();
 cscifichannels->Divide(3,2);
 for (int iscifi = 0; iscifi < nscifi; iscifi++){
  cscifichannels->cd(iscifi+1);
  cout<<"Neutrinos in wall "<<iscifi+1<<" are "<<n_neutrinos[iscifi]<<endl;
  //hxyscifich[iscifi]->Scale(1./n_neutrinos[iscifi]);
  hxyscifich[iscifi]->Draw("COLZ");
  if (writescifihistograms) hxyscifich[iscifi]->Write();
  if (iscifi==0){
   gvxy_first->Draw("SAME");
  }
 }
 TCanvas *cmaxscifi = new TCanvas();
 cmaxscifi->Divide(1,2);
 cmaxscifi->cd(1);
 hmaxscifix->Draw();
 cmaxscifi->cd(2);
 hmaxscifiy->Draw();

 TCanvas *cvz = new TCanvas();
 hvz_first->Draw();

 if (writescifihistograms){ 
   gvxy_first->Write();
   hmaxscifix->Write();
   hmaxscifiy->Write();
   //scaling histograms over number of neutrinos in first wall and saving them
   for (int iscifi = 0; iscifi < nscifi; iscifi++){
    hnxscifich[iscifi]->Scale(1./(n_neutrinos[0]));
    hnyscifich[iscifi]->Scale(1./(n_neutrinos[0]));
    hnxscifich[iscifi]->Write();
    hnyscifich[iscifi]->Write(); 
   }
 }
 TCanvas *cnbins = new TCanvas();
 cnbins->Divide(1,2);
 cnbins->cd(1);
 hnxscifich[0]->Draw("histo");
 cnbins->cd(2);
 hnyscifich[0]->Draw("histo");
} //end of main program
