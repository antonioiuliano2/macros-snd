#include "TFile.h"
#include "TTree.h"

void doNormHistos(){

	TFile *inFile = TFile::Open("hvtx.allbricks.root", "READ");
	
	TH1F *hntrks = (TH1F*) inFile->Get("h_ntrks");
	TH1F *hmaxaperture = (TH1F*) inFile->Get("h_maxaperture");
	TH1F *hIPdau = (TH1F*) inFile->Get("h_IP_dau");
	
	Double_t factor = 1.;
	TH1F *h_ntrks_n = (TH1F*)hntrks->Clone("h_ntrks_n");
	h_ntrks_n->Scale(factor/hntrks->GetEntries());
	
	TH1F *h_maxaperture_n = (TH1F*)hmaxaperture->Clone("h_maxaperture_n");
	h_maxaperture_n->Scale(factor/hmaxaperture->GetEntries());
	
	TH1F *h_IP_dau_n = (TH1F*)hIPdau->Clone("h_IP_dau_n");
	h_IP_dau_n->Scale(factor/hIPdau->GetEntries());
	
	TFile *outFile = new TFile("hvtx.allbricks_norm.root", "RECREATE");
	h_ntrks_n->Write();
	h_maxaperture_n->Write();
	h_IP_dau_n->Write();
	outFile->Close();
	inFile->Close();	
}
