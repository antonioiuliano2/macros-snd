void compareframealign(){
    TFile *mic2_file = TFile::Open(
        "root:://eospublic.cern.ch//eos/experiment/sndlhc/emulsionData/2022/Napoli/SND/mic2/RUN1_W2_B1/P10/tracks.raw.root");
    TTree *mic2_framealign = (TTree*) mic2_file->Get("FrameAlign");
    
    TFile *mic3_file = TFile::Open(
        "root:://eospublic.cern.ch//eos/experiment/sndlhc/emulsionData/2022/Napoli/SND/mic3/RUN1_W2_B2/P10/tracks.raw.root");
    TTree *mic3_framealign = (TTree*) mic3_file->Get("FrameAlign");
    
    TFile *mic5_file = TFile::Open(
        "root:://eospublic.cern.ch//eos/experiment/sndlhc/emulsionData/2022/Napoli/SND/mic5/RUN1_W3_B4/P58/tracks.raw.root"); 
    TTree *mic5_framealign = (TTree*) mic5_file->Get("FrameAlign");
    //drawing dx graphs
    TCut frameselection("area==2&&view==0"); 
    
    mic2_framealign->Draw("dx:z1>>gdx_mic2(300,100.,1400.,100,-1.,1.)",frameselection);
    TH2F *gdx_mic2 = (TH2F*) gDirectory->Get("gdx_mic2");
    gdx_mic2->SetTitle("mic2 Naples;z1[#mum];dy[#mum]");
    gdx_mic2->SetMarkerStyle(8);

    mic3_framealign->Draw("dx:z1>>gdx_mic3(300,100.,1400.,100,-1.,1.)",frameselection);
    TH2F *gdx_mic3 = (TH2F*) gDirectory->Get("gdx_mic3");
    gdx_mic3->SetTitle("mic3 Naples;z1[#mum];dy[#mum]");
    gdx_mic3->SetMarkerStyle(8);
    gdx_mic3->SetMarkerColor(kRed);

    mic5_framealign->Draw("dx:z1>>gdx_mic5(300,100.,1400.,100,-1.,1.)",frameselection);
    TH2F *gdx_mic5 = (TH2F*) gDirectory->Get("gdx_mic5");
    gdx_mic5->SetTitle("mic5 Naples;z1[#mum];dy[#mum]");
    gdx_mic5->SetMarkerStyle(8);
    gdx_mic5->SetMarkerColor(kYellow);
    //                   now dy ///////////////
    mic2_framealign->Draw("dy:z1>>gdy_mic2(300,100.,1400.,100,-1.,1.)",frameselection);
    TH2F *gdy_mic2 = (TH2F*) gDirectory->Get("gdy_mic2");
    gdy_mic2->SetTitle("mic2 Naples;z1[#mum];dy[#mum]");
    gdy_mic2->SetMarkerStyle(8);

    mic3_framealign->Draw("dx:z1>>gdy_mic3(300,100.,1400.,100,-1.,1.)",frameselection);
    TH2F *gdy_mic3 = (TH2F*) gDirectory->Get("gdy_mic3");
    gdy_mic3->SetTitle("mic3 Naples;z1[#mum];dy[#mum]");
    gdy_mic3->SetMarkerStyle(8);
    gdy_mic3->SetMarkerColor(kRed);

    mic5_framealign->Draw("dx:z1>>gdy_mic5(300,100.,1400.,100,-1.,1.)",frameselection);
    TH2F *gdy_mic5 = (TH2F*) gDirectory->Get("gdy_mic5");
    gdy_mic5->SetTitle("mic5 Naples;z1[#mum];dy[#mum]");
    gdy_mic5->SetMarkerStyle(8);
    gdy_mic5->SetMarkerColor(kYellow);

   // TMultiGraph *mg = new TMultiGraph();
   // mg->Add(gdx_mic2);
   // mg->Add(gdx_mic3);
   // mg->Add(gdx_mic5);

   // mg->Draw("A");
    TCanvas *c = new TCanvas();
    //c->Divide(2,2);
    //c->cd(1);
    gdx_mic2->Draw();
    //c->cd(2);
    gdx_mic3->Draw("SAME");
    //c->cd(3);
    gdx_mic5->Draw("SAME");
    c->BuildLegend();

    TCanvas *cdy = new TCanvas();
    //c->Divide(2,2);
    //c->cd(1);
    gdy_mic2->Draw();
    //c->cd(2);
    gdy_mic3->Draw("SAME");
    //c->cd(3);
    gdy_mic5->Draw("SAME");
    cdy->BuildLegend();    

}