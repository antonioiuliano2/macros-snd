void updatemonitorgraphs(const char* inputfilename, TFile *graphfile){
    //accessing dcs monitoring file (example file name: dcs_output_20220421.root)
    TString treefilename = TString("/eos/experiment/sndlhc/emulsionData/dcs_monitoring/"+TString(inputfilename));
    TFile *smsfile = TFile::Open(treefilename);
    TTree *smstree = (TTree*) smsfile->Get("smstree");
    //setting variables and branches
    TDatime *monitortime;
    const int ntrhsensors = 5;
    const int nsmksensors = 3;
    EColor colors[ntrhsensors] = {kRed,kOrange,kBlue,kMagenta,kBlack};
    double temp[ntrhsensors], relhum[ntrhsensors];
    bool smk[nsmksensors], coolingon;

    smstree->SetBranchAddress("monitortime",&monitortime);
    smstree->SetBranchAddress("smk",&smk);
    smstree->SetBranchAddress("temp",&temp);
    smstree->SetBranchAddress("relhum",&relhum);
    smstree->SetBranchAddress("coolingon",&coolingon);

    const int nentries = smstree->GetEntries();

    //preparing graphs
    TGraph *tempgraphs[ntrhsensors];
    TGraph *humiditygraphs[ntrhsensors];
    for (int isensor = 0; isensor < ntrhsensors;isensor++){
        tempgraphs[isensor] = new TGraph();
        humiditygraphs[isensor] = new TGraph();

        tempgraphs[isensor]->SetTitle(Form("Temperature for sensor %i;Time[H:M];temp[C]",isensor+1));
        humiditygraphs[isensor]->SetTitle(Form("Humidity for sensor %i;Time[H:M];rh",isensor+1));
    }

    //start loop
    UInt_t convertedtime; //from TDatime Convert()
    for (int ientry = 0; ientry < nentries; ientry++){
        smstree->GetEntry(ientry);
        convertedtime = monitortime->Convert();
        //loop over sensors
        for (int isensor = 0; isensor < ntrhsensors;isensor++){
            tempgraphs[isensor]->AddPoint(convertedtime,temp[isensor]);
            humiditygraphs[isensor]->AddPoint(convertedtime,relhum[isensor]);
        }
    }
    //end of loop, plotting graphs
    TCanvas *ctrh = new TCanvas();
     ctrh->Divide(1,2);
    for (int isensor = 0; isensor < ntrhsensors;isensor++){
       ctrh->cd(1);
       tempgraphs[isensor]->GetXaxis()->SetTimeDisplay(1);
       tempgraphs[isensor]->GetXaxis()->SetTimeFormat("%d/%m-%H:%M");
       if (isensor > 0) tempgraphs[isensor]->Draw("SAME");
       else tempgraphs[isensor]->Draw();
       tempgraphs[isensor]->SetLineColor(colors[isensor]);
       tempgraphs[isensor]->GetYaxis()->SetRangeUser(10,20);

       ctrh->cd(2);
       humiditygraphs[isensor]->GetXaxis()->SetTimeDisplay(1);
       humiditygraphs[isensor]->GetXaxis()->SetTimeFormat("%d/%m-%H:%M");
       if (isensor > 0) humiditygraphs[isensor]->Draw("SAME");
       else humiditygraphs[isensor]->Draw();
       humiditygraphs[isensor]->SetLineColor(colors[isensor]);
       humiditygraphs[isensor]->GetYaxis()->SetRangeUser(30,60);
    }
    ctrh->GetPad(1)->BuildLegend();
    ctrh->GetPad(2)->BuildLegend();
    //updating title, just for all plots
    tempgraphs[0]->SetTitle("Temperature;Time[H:M];temp[C]");
    humiditygraphs[0]->SetTitle("Relative Humidity;Time[H:M];rh");

    cout<<"Total number of points"<<endl;
    cout<<smstree->GetEntries()<<endl;
    cout<<"Number of points without cooling "<<endl;
    cout<<smstree->GetEntries("!coolingon")<<endl;
    cout<<"Number of point with at least one smoking triggered "<<endl;
    cout<<smstree->GetEntries("smk[0]||smk[1]||smk[2]")<<endl;

    graphfile->cd();
    ctrh->SetName("ctrh");
    ctrh->Write("ctrh",TObject::kOverwrite);

}

void drawmonitorgraphs(const char *inputfilename){
    TString graphfilename = TString("/eos/experiment/sndlhc/www/online/graphs_"+TString(inputfilename));

    TFile *graphfile = new TFile(graphfilename.Data(),"UPDATE"); //does not delete existing data
    TUnixSystem *mysystem = new TUnixSystem();
    while(1){
         //cout<<"Updated graphs"<<endl;
         updatemonitorgraphs(inputfilename,graphfile); //does not stop on its own, only when killed
         graphfile->SaveSelf();
         mysystem->Sleep(1800*1000.);
        }
}