//compare original neutrino distributions from FLUKA with distributions from sndsw
void makenutree(){
 TString path("/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/numuCCDIS_SND_9_March_2021/");
 TFile *simfile = TFile::Open((path+TString("ship.conical.Genie-TGeant4.root")).Data());
 TTreeReader reader("cbmsim",simfile);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");

 TH2D *hxy_sndsw = new TH2D("hxy_sndsw","Neutrinos xy distribution from sndsw;x[cm];y[cm]",100,-100,100,100,0,100);
 //looping over events
 const int nevents = reader.GetEntries();
 cout<<"Start looping over events "<<nevents<<endl;

 Int_t pdgcode;
 Double_t nupx, nupy, nupz;
 Double_t nux, nuy, nuz;
 Double_t weight;

 TFile *outputfile = new TFile((path+TString("onlyneutrinos.root")).Data(),"RECREATE");
 TTree *outputtree = new TTree("neutrinos","Distributions of neutrinos in sndsw");

 outputtree->Branch("pdgcode",&pdgcode,"pdgcode/I");

 outputtree->Branch("nux",&nux,"nux/D");
 outputtree->Branch("nuy",&nuy,"nuy/D");
 outputtree->Branch("nuz",&nuz,"nuz/D");

 outputtree->Branch("nupx",&nupx,"nupx/D");
 outputtree->Branch("nupy",&nupy,"nupy/D");
 outputtree->Branch("nupz",&nupz,"nupz/D");

 outputtree->Branch("w",&weight,"w/D");
 
 for (int ievent = 0; ievent < nevents; ievent++){
     if (ievent%10000==0) cout<<"Arrived at event "<<ievent<<endl;
     reader.SetEntry(ievent);
     auto numutrack = tracks[0];
     //getting variables

     weight = numutrack.GetWeight();
     pdgcode = numutrack.GetPdgCode();

     nux = numutrack.GetStartX();
     nuy = numutrack.GetStartY();
     nuz = numutrack.GetStartZ();

     nupx = numutrack.GetPx();
     nupy = numutrack.GetPy();
     nupz = numutrack.GetPz();
     //filling tree
     outputtree->Fill();
 }
 outputfile->cd();
 outputtree->Write();
 outputfile->Close();
}

void comparison_Fluka_sndsw(){

 //***********************fluka simulation**********************
 ROOT::RDataFrame FLUKAsim("t", "/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/Generate_GENIEinput_AnnaritaSND/NeutrinoFiles/NeutMuon.root");
 auto FLUKAsim_full = FLUKAsim.Define("pt","Ekin.EKin * x_cos + Ekin.EKin * y_cos");

 auto hxy_fluka = FLUKAsim_full.Histo2D({"hxy_fluka","Neutrinos xy distribution from FLUKA;x[cm];y[cm]",200,-100,100,200,-100,100},"x","y","weight");
 auto hcosxy_fluka = FLUKAsim_full.Histo2D({"hcosxy_fluka","Neutrino angles from FLUKA;x_cos;y_cos",400,-0.2,0.2,400,-0.2,0.2},"x_cos","y_cos","weight");
 auto hppt_fluka = FLUKAsim_full.Define("log10ptot","log10(Ekin.EKin)").Define("log10pt","log10(pt+0.01)")
                                .Histo2D({"hppt_fluka","Neutrinos ppt distribution from FLUKA;log10(ptot);log10(pt+0.01)",200,0,4,200,-4.,1.},"log10ptot","log10pt","weight");
 //***********************sndsw simulation**********************
 ROOT::RDataFrame SNDSWsim("neutrinos","/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/numuCCDIS_SND_9_March_2021/onlyneutrinos.root");
 //computing cosine x and cosine y
 auto SNDSWsim_full = SNDSWsim.Define("nup","sqrt(nupx*nupx + nupy*nupy + nupz*nupz)")
                      .Define("nupt","sqrt(nupx*nupx + nupy*nupy)")
                      .Define("nux_cos","nupx/nup")
                      .Define("nuy_cos","nupy/nup");

 auto hxy_sndsw = SNDSWsim_full.Histo2D({"hxy_sndsw","Neutrinos xy distribution from SNDSW;x[cm];y[cm]",200,-100,100,200,-100,100},"nux","nuy");
 auto hcosxy_sndsw = SNDSWsim_full.Histo2D({"hcosxy_sndsw","Neutrino angles from SNDSW;x_cos;y_cos",400,-0.2,0.2,400,-0.2,0.2},"nux_cos","nuy_cos");
 auto hppt_sndsw = SNDSWsim_full.Define("log10ptot","log10(nup)").Define("log10pt","log10(nupt+0.01)")
                                .Histo2D({"hppt_sndsw","Neutrinos ppt distribution from SNDSW;log10(ptot);log10(pt+0.01)",200,0,4,200,-4,1},"log10ptot","log10pt");
 //***********************plotting histograms*******************
 TCanvas *c = new TCanvas();
 hxy_fluka->DrawClone();
 hxy_sndsw->SetLineColor(kRed);
 hxy_sndsw->SetMarkerColor(kRed);
 hxy_sndsw->DrawClone("SAMES");
 c->BuildLegend();

 TCanvas *c_cosxy = new TCanvas();
 hcosxy_fluka->DrawClone();
 hcosxy_sndsw->SetLineColor(kRed);
 hcosxy_sndsw->SetMarkerColor(kRed);
 hcosxy_sndsw->DrawClone("SAMES");
 c_cosxy->BuildLegend();

 TCanvas *c_ppt = new TCanvas();
 c_ppt->Divide(2,1);
 c_ppt->cd(1);
 hppt_fluka->DrawClone("COLZ");
 c_ppt->cd(2);
 hppt_sndsw->DrawClone("COLZ");

 
}
