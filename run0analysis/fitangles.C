//fitting two gaussian peaks for comparison with electronic detectors (Antonio, 24 November 2022)
void fitangles(){
 //getting file and histograms
 TFile *inputfile = TFile::Open("plots_localtracking.root");
 TH1D *htx = (TH1D*) inputfile->Get("htx");
 
 const int myrebin = 5;
 htx->Rebin(myrebin);

 TCanvas *ctx = new TCanvas();
 htx->Draw();
 htx->GetXaxis()->SetRangeUser(0,0.04);

 //fitting to functions
 htx->Fit("gaus","","",0,0.01);

 htx->Fit("gaus","+","",0.01,0.015);

}
