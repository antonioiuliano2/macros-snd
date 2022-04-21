void drawmonitorgraphs(){
    //accessing dcs monitoring file
    TFile *smsfile = TFile::Open("dcs_output_20220421.root");
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
       tempgraphs[isensor]->GetXaxis()->SetTimeFormat("%H:%M");
       tempgraphs[isensor]->SetLineColor(colors[isensor]);
       if (isensor > 0) tempgraphs[isensor]->Draw("SAME");
       else tempgraphs[isensor]->Draw();
       tempgraphs[isensor]->GetYaxis()->SetRangeUser(10,20);

       ctrh->cd(2);
       humiditygraphs[isensor]->GetXaxis()->SetTimeDisplay(1);
       humiditygraphs[isensor]->GetXaxis()->SetTimeFormat("%H:%M");
       humiditygraphs[isensor]->SetLineColor(colors[isensor]);
       if (isensor > 0) humiditygraphs[isensor]->Draw("SAME");
       else humiditygraphs[isensor]->Draw();
       humiditygraphs[isensor]->GetYaxis()->SetRangeUser(30,60);
    }
    ctrh->GetPad(1)->BuildLegend();
    ctrh->GetPad(2)->BuildLegend();
    //updating title, just for all plots
    tempgraphs[0]->SetTitle("Temperature;Time[H:M];temp[C]");
    humiditygraphs[0]->SetTitle("Relative Humidity;Time[H:M];rh");
}