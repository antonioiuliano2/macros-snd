//Converting monitoring times from TDateTime local to UNIX global
void convertmonitortree(const char* inputfilename){
    TString filepath("/home/utente/DAQSNDBuild/");
    TString treefilename = TString(filepath+TString("/backupfiles_lxplustime/")+TString(inputfilename));
    TFile *smsfile = TFile::Open(treefilename);
    TTree *smstree = (TTree*) smsfile->Get("smstree");

    //setting variables and branches
    TDatime *monitortime;
    const int ntrhsensors = 5;
    const int nsmksensors = 3;
    double temp[ntrhsensors], relhum[ntrhsensors];
    bool smk[nsmksensors], coolingon;

    smstree->SetBranchAddress("monitortime",&monitortime);
    smstree->SetBranchAddress("smk",&smk);
    smstree->SetBranchAddress("temp",&temp);
    smstree->SetBranchAddress("relhum",&relhum);
    smstree->SetBranchAddress("coolingon",&coolingon);

    const int nentries = smstree->GetEntries();
    //new ttree, same branches, except monitortime
    ULong64_t convertedtime; //from TDatime Convert()
    TString conv_treefilename = TString(filepath+TString(inputfilename));
    TFile *conv_smsfile = TFile::Open(conv_treefilename,"NEW");
    TTree *conv_smstree = new TTree("smstree","TTree with Survey Monitoring System data");
    conv_smstree->Branch("monitortime",&convertedtime, "monitortime/l"); //l is for long unsigned int branches
    conv_smstree->Branch("temp",&temp,"temp[5]/D");
    conv_smstree->Branch("relhum",&relhum,"relhum[5]/D");
    conv_smstree->Branch("smk",&smk,"smk[3]/O");
    conv_smstree->Branch("coolingon",&coolingon,"coolingon/O"); 

    //start loop
    for (int ientry = 0; ientry < nentries; ientry++){
        smstree->GetEntry(ientry);
        convertedtime = monitortime->Convert();
        conv_smstree->Fill();
    }
    //end of loop, saving converted file
    conv_smsfile->cd();
    conv_smsfile->Write();
}