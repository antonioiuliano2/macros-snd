//Computing cross section as Egenie/Efluka. 28 April 2021. NOw saving in the same file interacting neutrino files

void compute_crosssection(){
 TFile *geniefile = TFile::Open("/home/utente/Simulations/sim_snd/testing_SND_workflow/numu_CCDIS_test.root","UPDATE");
 TFile *flukafile = TFile::Open("/home/utente/Simulations/sim_snd/testing_SND_workflow/NeutMuon.root");

 //getting distributions
 TH1D *hfluka_nuE = (TH1D*) flukafile->Get("1014");

 ROOT::RDataFrame df("gst",geniefile);
 auto hgenie_nuE = df.Histo1D({"hgenie_nuE","Energy of muon neutrinos from genie spectrum;E[GeV]",500,0,5000},"pzv");

 TCanvas *c = new TCanvas();
 hfluka_nuE->Draw("histo");

 TCanvas *c1 = new TCanvas();
 hgenie_nuE->DrawClone("histo");

 //computing cross section distributions
 //correctly handling errors in division
 hfluka_nuE->Sumw2();
 hgenie_nuE->Sumw2();

 TH1D *hnuratio = new TH1D("hnuratio","ratio between Genie and Fluka energies, proportional to the cross section",500,0,5000);
 //making division and plotting ratio distribution
 hnuratio->Divide(hgenie_nuE.GetPtr(), hfluka_nuE);
 TCanvas *c2 = new TCanvas();
 hnuratio->Draw("histo");

 //starting hit or miss section
 double maxratio = hnuratio->GetMaximum();

 TTree *fluka_nutree = (TTree*) flukafile->Get("t");
 TTree *output_nutree = fluka_nutree->CloneTree(0);
 output_nutree->SetName("fluka_neutrinos_selected");
 output_nutree->SetTitle("Neutrinos from Fluka simulation after hit and miss selection");

 //setting branch of tree for neutrino energy, the one I need for this session
 double Ekin;
 fluka_nutree->SetBranchAddress("Ekin",&Ekin);
 const int nNeutrinos = fluka_nutree->GetEntries();
 //starting loop over neutrinos
 cout<<"Starting loop over "<<nNeutrinos<<" neutrinos"<<endl;
 for (int ineutrino = 0; ineutrino < nNeutrinos; ineutrino++){
     fluka_nutree->GetEntry(ineutrino);
     //get uniform random number between 0 and maximum ratio
     double randomratio = gRandom->Uniform(0, maxratio);
     //finding ratio for that neutrino energy
     double matchingratio = hnuratio->GetBinContent(hnuratio->FindBin(Ekin));
     //if the found ratio is larger than the random number, I accept the neutrino
     if (matchingratio > randomratio) output_nutree->Fill(); 

 }
 //writing tree, closing file
 geniefile->cd();
 output_nutree->Write();
 geniefile->Close();


}