#include "TFile.h"
#include "TTree.h"

void Histo2(TCanvas *c, TString cname, TH1 *h1, TH1 *h2, TString histoname);

void doMerge(TString inFilepath){
	
	TFile *inFile = TFile::Open(inFilepath, "READ");
	
	/** Neutrino part **/
	
	TH1F *nu_hntrks = (TH1F*) inFile->Get("nu_hntrks");
	TH1F *nu_hmaxaperture = (TH1F*) inFile->Get("nu_hmaxaperture");
	TH1F *nu_hIPdau = (TH1F*) inFile->Get("nu_hIPdau");
	TH1F *nu_hprob = (TH1F*) inFile->Get("nu_hprob");
	TH1F *nu_hmeanIPdau = (TH1F*) inFile->Get("nu_hmeanIPdau");
	TH1F *nu_hmaxIPdau = (TH1F*) inFile->Get("nu_hmaxIPdau");
	TH1F *nu_hfillfact = (TH1F*) inFile->Get("nu_hfillfact");
	TH1F *nu_hmeanfillfact = (TH1F*) inFile->Get("nu_hmeanfillfact");
	/** Muon bkg part **/
	TH1F *mu_hntrks = (TH1F*) inFile->Get("mu_hntrks");
	TH1F *mu_hmaxaperture = (TH1F*) inFile->Get("mu_hmaxaperture");
	TH1F *mu_hIPdau = (TH1F*) inFile->Get("mu_hIPdau");
	TH1F *mu_hprob = (TH1F*) inFile->Get("mu_hprob");
	TH1F *mu_hmeanIPdau = (TH1F*) inFile->Get("mu_hmeanIPdau");
	TH1F *mu_hmaxIPdau = (TH1F*) inFile->Get("mu_hmaxIPdau");
	TH1F *mu_hfillfact = (TH1F*) inFile->Get("mu_hfillfact");
	TH1F *mu_hmeanfillfact = (TH1F*) inFile->Get("mu_hmeanfillfact");

	// new coloring
	mu_hntrks->SetLineColor(kBlue);
	mu_hntrks->SetFillColor(kBlue);
	mu_hntrks->SetFillStyle(3004);
	mu_hmaxaperture->SetLineColor(kBlue);
	mu_hmaxaperture->SetFillColor(kBlue);
	mu_hmaxaperture->SetFillStyle(3004);
	mu_hIPdau->SetLineColor(kBlue);
	mu_hIPdau->SetFillColor(kBlue);
	mu_hIPdau->SetFillStyle(3004);
	mu_hprob->SetLineColor(kBlue);
	mu_hprob->SetFillColor(kBlue);
	mu_hprob->SetFillStyle(3004);
	mu_hmeanIPdau->SetLineColor(kBlue);
	mu_hmeanIPdau->SetFillColor(kBlue);
	mu_hmeanIPdau->SetFillStyle(3004);
	mu_hmaxIPdau->SetLineColor(kBlue);
	mu_hmaxIPdau->SetFillColor(kBlue);
	mu_hmaxIPdau->SetFillStyle(3004);
	mu_hfillfact->SetLineColor(kBlue);
	mu_hfillfact->SetFillColor(kBlue);
	mu_hfillfact->SetFillStyle(3004);
	mu_hmeanfillfact->SetLineColor(kBlue);
	mu_hmeanfillfact->SetFillColor(kBlue);
	mu_hmeanfillfact->SetFillStyle(3004);


	// changing axis range
	nu_hntrks->GetYaxis()->SetRangeUser(0, 1);
	mu_hntrks->GetYaxis()->SetRangeUser(0, 1);
	nu_hmaxaperture->GetYaxis()->SetRangeUser(0, 0.3);
	mu_hmaxaperture->GetYaxis()->SetRangeUser(0, 0.3);
	nu_hIPdau->GetYaxis()->SetRangeUser(0, 1);
	mu_hIPdau->GetYaxis()->SetRangeUser(0, 1);

    TCanvas *c1;
    Histo2(c1, "c1", nu_hntrks, mu_hntrks, "associated tracks");
    TCanvas *c2;
    Histo2(c2, "c2", nu_hmaxaperture, mu_hmaxaperture, "Max Aperture");
    TCanvas *c3;
    Histo2(c3, "c3", nu_hprob, mu_hprob, "Probability");
    TCanvas *c4;
    Histo2(c4, "c4", nu_hmeanIPdau, mu_hmeanIPdau, "Mean IP of daughters");
    TCanvas *c5;
    Histo2(c5, "c5", nu_hmaxIPdau, mu_hmaxIPdau, "Mean IP of daughters");
    TCanvas *c6;
    Histo2(c6, "c6", nu_hfillfact, mu_hfillfact, "Fill factor");
    TCanvas *c7;
    Histo2(c7, "c7", nu_hmeanfillfact, mu_hmeanfillfact, "Mean Fill factor");

}
void Histo2(TCanvas *c,TString cname, TH1 *h1, TH1 *h2, TString histoname){
   //gStyle->SetOptStat("emr");
   c = new TCanvas(cname, cname, 1190, 757);
   c->SetGrid();
   h1->SetLineColor(kRed); h2->SetLineColor(kBlue);
   //h1->GetXaxis()->SetTitle((h_axis[0]).c_str()); h1->GetYaxis()->SetTitle((h_axis[1]).c_str());
   //h2->GetXaxis()->SetTitle((h_axis[0]).c_str()); h2->GetYaxis()->SetTitle((h_axis[1]).c_str());
   h1->SetLineWidth(2); h2->SetLineWidth(2);
   h1->SetFillColor(kRed); h2->SetFillColor(kBlue);
   h1->SetFillStyle(3005); h2->SetFillStyle(3004);
   TLegend *legenda= new TLegend(0.7, 0.9, 0.9, 0.7);    
   legenda->AddEntry(h1,"#nu_{#mu} vtx "+histoname,"f"); 
   legenda->AddEntry(h2,"#mu bkg vtx "+histoname,"f");
   if(h1->GetMaximum() > h2->GetMaximum()){
        h1->Draw("HIST");
        h2->Draw("HIST SAMES");
   }
   else{
        h2->Draw("HIST");
        h1->Draw("HIST SAMES");
   }
   legenda->Draw();
}