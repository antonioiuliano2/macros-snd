void reproduceROC(){
 TFile *TMVAoutfile = TFile::Open("TMVA_out.root");
 TTree *TestTree = (TTree*) TMVAoutfile->Get("dataset/TestTree");
 
 Int_t classID;
 Float_t BDT;

 TestTree->SetBranchAddress("classID",&classID);
 TestTree->SetBranchAddress("BDT",&BDT);
 int nentries = TestTree->GetEntries();

 vector<Float_t> mva_signal;
 vector<Float_t> mva_background;

 for (int ientry = 0; ientry < nentries; ientry++){
     TestTree->GetEntry(ientry);
     if (classID > 0) mva_background.push_back(BDT);
     else mva_signal.push_back(BDT);
 } 
 cout<<"Ended loop over "<<nentries<<endl;
 //end loop, getting roc curve
 const Double_t rejectionbkg = 0.98;

 TMVA::ROCCurve *myroc = new TMVA::ROCCurve(mva_signal,mva_background);
 Double_t testvar = myroc->GetEffSForEffB(1-rejectionbkg,100);
 cout<<"TEST: Eff S : "<<testvar<<" for Rej B "<<rejectionbkg<<endl;
 TCanvas  *c1 = new TCanvas();
 TGraph * graph = myroc->GetROCCurve(100);
 testvar = 0.956;
 cout<<"Rej B "<<graph->Eval(testvar)<< " EffS "<<testvar<<endl;
 graph->SetTitle("ROC Curve; Signal efficiency; Bkg Rejection");
 graph->Draw("AP*");
}