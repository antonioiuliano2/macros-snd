//#include "Inclusion.h" (not actually needed in root 6)
//example code to read pythia8 nu_tau_sim. Originally received by Simona Ilieva, July 2021 
void BinLogXY(TH2*h) 
{

   TAxis *axis = h->GetXaxis(); 
   int bins = axis->GetNbins();

   Axis_t from = axis->GetXmin();
   Axis_t to = axis->GetXmax();
   Axis_t width = (to - from) / bins;
   Axis_t *new_bins = new Axis_t[bins + 1];

   for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);
   } 
   axis->Set(bins, new_bins); 
   delete new_bins; 
   
   axis = h->GetYaxis(); 
   bins = axis->GetNbins();

   from = axis->GetXmin();
   to = axis->GetXmax();
   width = (to - from) / bins;
   Axis_t *new_bins_y = new Axis_t[bins + 1];

   for (int i = 0; i <= bins; i++) {
     new_bins_y[i] = TMath::Power(10, from + i * width);
   } 
   axis->Set(bins, new_bins_y); 
   delete new_bins_y; 
}

void read_pythia_ntuple(){
    // Macro reading output of SND@LHC pythia8 nu_tau sim
    // Particles are saved as a TParticle class
    // Nu_tau history is saved in Ancstr branch
    // The nu_tau itself is the 0th entry in each branch of TParticles
    TFile *f = new TFile("pythia8_hard_PDFpset13.root"); //home/utente/Simulations/sim_snd/pythia8_hard_PDFpfset13.root
    TTree *t1 = (TTree*)f->Get("NuTauTree");
    TParticle *part;
    TClonesArray *ancstr = new TClonesArray("TParticle"); //The branch is a collection of TParticles
    t1->SetBranchAddress("Ancstr",&ancstr);
    int evtId{};
    t1->SetBranchAddress("EvtId",&evtId);
    
    TFile *fout = new TFile("Nutau_PP_pythia8_pdf13.root", "recreate");
    TH1D *hE = new TH1D("hE","; E [GeV]", 100, 0, 1000);
    TH1D *hE_c = new TH1D("hE_c","; E [GeV]", 100, 0, 1000);
    TH1D *hE_b = new TH1D("hE_b","; E [GeV]", 100, 0, 1000);
    TH1D *hn = new TH1D("hn","; |#eta|",100, 0,10);
    TH2D *hnE = new TH2D("hnE","; |#eta|; E [GeV]", 500, log10(10e-1), log10(10), 500, log10(10e-5), log10(1000));
    BinLogXY(hnE);
    TH2D *hXY = new TH2D("hXY","; X [cm]; Y [cm]", 100, -20, 20, 100, -20, 20);
    TH1D *hZ = new TH1D("hZ", " ; Z [mm]", 100, 0, 700);
    
    bool isD = false, isB = false;
    double eta{};
    double Tofb=10e-12;

    const double xsecangle = 150e-6;
    
    //read all entries and fill the histograms
    int nentries = t1->GetEntries();
    cout<<"N trees: "<<nentries<<endl;
    for (int i=0;i<nentries;i++) {
      ancstr->Clear();
      isD=false; isB=false;
      t1->GetEntry(i);
      int nparts = ancstr->GetEntriesFast(); 
      part=(TParticle*)ancstr->At(2); //why do we retrieve this particle here?
      //cout<<"nparts "<<nparts<<" evtId: "<<evtId<<endl;
      //start loop over particles, nothing currently done
      for(int j=0; j<nparts;j++){
        part=(TParticle*)ancstr->At(j);
        TVector3 *partmomentum = new TVector3(part->Px(),part->Py(),part->Pz());
       // cout<<"Before rotation"<<endl;
       // cout<<i<<" "<<j<<" "<<part->GetPdgCode()<<" "<<partmomentum->Px()<<" "<<partmomentum->Py()<<" "<<partmomentum->Pz()<<endl;
        double origpz = partmomentum->Pz();
        partmomentum->RotateX(xsecangle);
       // cout<<"After rotation"<<endl;
       // cout<<i<<" "<<j<<" "<<part->GetPdgCode()<<" "<<partmomentum->Px()<<" "<<partmomentum->Py()<<" "<<partmomentum->Pz()<<endl;
       // cout<<"Delta z: "<<partmomentum->Pz() - origpz<<endl;
        //cout<<j<<" id "<<part->GetPdgCode()<<endl;
      }	
      part=(TParticle*)ancstr->At(1); //checking particle ID
      if(fabs(part->GetPdgCode())>=400 && fabs(part->GetPdgCode())<=499) isD = true;
      if(fabs(part->GetPdgCode())>=500 && fabs(part->GetPdgCode())<=599) isB = true;
      if(fabs(part->GetPdgCode())==15){ //if it is tau, we go to the mother
         part=(TParticle*)ancstr->At(2);
         if(fabs(part->GetPdgCode())>=400 && fabs(part->GetPdgCode())<=499) isD = true;
         if(fabs(part->GetPdgCode())>=500 && fabs(part->GetPdgCode())<=599) isB = true;
      }
      if(isD || isB){
        part=(TParticle*)ancstr->At(0);
        if(isD&&isB)cout<<"both b&c pid "<<part->GetPdgCode()<<endl;
        if(isD) hE_c->Fill(part->Energy());
        if(isB) hE_b->Fill(part->Energy());
        hE->Fill(part->Energy());
        eta = -log(tan(part->Theta()/2.));
        hn->Fill(fabs(part->Eta()));
        hnE->Fill(fabs(part->Eta()), part->Energy());
        hXY->Fill(part->Vx()*10, part->Vy()*10);
        hZ->Fill(part->Vz());
      }
    }
    hn->Write();
    hXY->Write();
    hZ->Write();
    TCanvas *c1 =new TCanvas();
    hnE->Draw("coltz");
    c1->SetLogz();
    c1->SetLogy();
    c1->Write();
    
    TCanvas *c2 =new TCanvas();
    hE->Scale(1./1.972E+03/Tofb);
    hE_b->Scale(1./0.388039/Tofb);
    hE_c->Scale(1./4.48114/Tofb);
    hE->Write(); hE_c->Write(); hE_b->Write();
    hE->SetLineColor(kRed);
    hE_c->SetLineColor(kGreen);
    hE_b->SetLineColor(kMagenta);
    hE_c->Draw();
    hE_b->Draw("same");
    hE->Draw("same");
    //c2->SetLogz();
    c2->SetLogy();
    c2->Write();
    fout->Close();
}
