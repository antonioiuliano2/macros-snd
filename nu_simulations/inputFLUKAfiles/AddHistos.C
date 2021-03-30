#include "TROOT.h"
using namespace std;
using namespace TMath;

void AddHistos(string fname)
{
	TH1F *h_1014 = new TH1F("1014","nu_mu",500,0,5000);
	TH2F *h_1214 = new TH2F("1214","nu_mu log10(ptot) vs log10(pt+0.01)",200,0,4,200,-4.,1.);
	TH1F *h_1012 = new TH1F("1012","nu_e",500,0,5000);
	TH2F *h_1212 = new TH2F("1212","nu_e log10(ptot) vs log10(pt+0.01)",200,0,4,200,-4.,1.);
	TH1F *h_1016 = new TH1F("1016","#nu_{#tau}",500,0,5000);
	TH2F *h_1216 = new TH2F("1216","#nu_{#tau} log10(ptot) vs log10(pt+0.01)",200,0,4,200,-4.,1.);
	TH1F *h_2014 = new TH1F("2014","#bar{#nu}_{#mu}",500,0,5000);
	TH2F *h_2214 = new TH2F("2214","#bar{#nu}_{#mu} log10(ptot) vs log10(pt+0.01)",200,0,4,200,-4.,1.);
	TH1F *h_2012 = new TH1F("2012","#bar{#nu}_{e}",500,0,5000);
	TH2F *h_2212 = new TH2F("2212","#bar{#nu}_{e} log10(ptot) vs log10(pt+0.01)",200,0,4,200,-4.,1.);
	TH1F *h_2016 = new TH1F("2016","#bar{#nu}_{#tau}",500,0,5000);
	TH2F *h_2216 = new TH2F("2216","#bar{#nu}_{#tau} log10(ptot) vs log10(pt+0.01)",200,0,4,200,-4.,1.);


	Double_t Ekin=0., px=0., py=0., pz=0., pT=0., x_cos=0., y_cos=0.,weight=0.;
	//Ekin=modulo p
	TFile *f = new TFile(fname.c_str(),"UPDATE");
	TTree *t = (TTree*)f->Get("t");
	t->SetBranchAddress("Ekin",&Ekin);
	t->SetBranchAddress("weight",&weight);
	t->SetBranchAddress("x_cos",&x_cos);
	t->SetBranchAddress("y_cos",&y_cos);
	//t->Branch("px",&px,"px/D");
	//t->Branch("py",&py,"py/D");
	//t->Branch("pz",&pz,"pz/D");
	//t->Branch("pT",&pT,"pT/D");
	
	TString fn = TString(fname);

	for(Int_t i=0;i<t->GetEntries();i++)
	{
		t->GetEntry(i);
		px = Ekin*x_cos;
		py = Ekin*y_cos;
		pT = Sqrt(px*px+py*py);
		pz = Sqrt(Ekin*Ekin-pT*pT);
		if(fn.Contains("NeutMuon.root") && !fn.Contains("Anti"))
		{
			h_1014->Fill(Ekin,weight);
			h_1214->Fill(Log10(Ekin),Log10(pT+0.01),weight);
		}	
		if(fn.Contains("NeutElec.root")&& !fn.Contains("Anti"))
		{
			h_1012->Fill(Ekin,weight);
			h_1212->Fill(Log10(Ekin),Log10(pT+0.01),weight);
		}	
		if(fn.Contains("NeutTau_filter.root")&& !fn.Contains("Anti"))
		{
			h_1016->Fill(Ekin,weight);
			h_1216->Fill(Log10(Ekin),Log10(pT+0.01),weight);
		}	
		if(fn.Contains("AntiNeutMuon.root"))
		{
			h_2014->Fill(Ekin,weight);
			h_2214->Fill(Log10(Ekin),Log10(pT+0.01),weight);
		}	
		if(fn.Contains("AntiNeutElec.root"))
		{
			h_2012->Fill(Ekin,weight);
			h_2212->Fill(Log10(Ekin),Log10(pT+0.01),weight);
		}	
		if(fn.Contains("AntiNeutTau_filter.root"))
		{
			h_2016->Fill(Ekin,weight);
			h_2216->Fill(Log10(Ekin),Log10(pT+0.01),weight);
		}	
	}
	if(fn.Contains("NeutMuon.root") && !fn.Contains("Anti"))
	{
		cout<<" In here "<<endl;
		h_1214->SetDirectory(f);
		h_1214->Write();

		h_1014->SetDirectory(f);
		h_1014->Write();
	}
	if(fn.Contains("NeutElec.root")&& !fn.Contains("Anti"))
	{
		h_1212->SetDirectory(f);
		h_1212->Write();

		h_1012->SetDirectory(f);
		h_1012->Write();
	}
	if(fn.Contains("NeutTau_filter.root")&& !fn.Contains("Anti"))
	{
		h_1216->SetDirectory(f);
		h_1216->Write();

		h_1016->SetDirectory(f);
		h_1016->Write();
	}
	if(fn.Contains("AntiNeutMuon.root"))
	{
		cout<<" In here "<<endl;
		h_2214->SetDirectory(f);
		h_2214->Write();

		h_2014->SetDirectory(f);
		h_2014->Write();
	}
	if(fn.Contains("AntiNeutElec.root"))
	{
		h_2212->SetDirectory(f);
		h_2212->Write();

		h_2012->SetDirectory(f);
		h_2012->Write();
	}
	if(fn.Contains("AntiNeutTau_filter.root"))
	{
		h_2216->SetDirectory(f);
		h_2216->Write();

		h_2016->SetDirectory(f);
		h_2016->Write();
	}
}
