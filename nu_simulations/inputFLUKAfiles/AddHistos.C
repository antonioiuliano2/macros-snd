#include "TROOT.h"
using namespace std;
using namespace TMath;

void AddHistos(string fname)
{
	TH1F *h_1014 = new TH1F("1014","nu_mu",500,0,5000);
	TH2F *h_1214 = new TH2F("1214","nu_mu log10(ptot) vs log10(pt+0.01)",200,0,4,200,-4.,1.);
	//TH3F *h_1414 = new TH3F("1414","nu_mu log10(ptot) vs log10(pt+0.01) vs phi angle",20,0,4,20,-4.,1.,35,0,3.5);
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


	Double_t Ekin=0., px=0., py=0., pz=0., pT=0., x_cos=0., y_cos=0., phi = 0., weight=0.;
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
		phi = ATan2(py,px);
		if(fn.Contains("NeutMuon.root") && !fn.Contains("Anti"))
		{
			h_1014->Fill(Ekin,weight);
			h_1214->Fill(Log10(Ekin),Log10(pT+0.01),weight);
			//h_1414->Fill(Log10(Ekin),Log10(pT+0.01),phi, weight);
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
				
		//h_1414->SetDirectory(f);
		//h_1414->Write();
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

void generate_neutrinos(){ //test of generated neutrino distributiosn

 TFile * fInputFile = TFile::Open("NeutMuon.root");
 TH1D * hnu_p =  (TH1D*) fInputFile->Get("1014");

 Float_t deltaz = 48000; //distance between center of proton target and center of neutrino target

 Float_t pzv;
 Double_t start[3];

 Int_t idbase=1200;
 char ts[20];
 TH1D* pxhist[3000];//!
 TH1D* pyslice[3000][100];//!
 printf("Reading (log10(p),log10(pt)) Hists from file: %s\n",fInputFile->GetName());

//gRandom->SetSeed(0); //set 0 to make it change everytime

Int_t idhnu=1214;
sprintf(ts,"%d",idhnu);
	    //pickup corresponding (log10(p),log10(pt)) histogram
if (fInputFile->FindObjectAny(ts)){
           TH2F* h2tmp = (TH2F*) fInputFile->Get(ts);
           printf("HISTID=%d, Title:%s\n",idhnu,h2tmp->GetTitle());
	         sprintf(ts,"px_%d",idhnu);
           //make its x-projection, to later be able to convert log10(p) to its bin-number
           pxhist[idhnu]=h2tmp->ProjectionX(ts,1,-1);
           Int_t nbinx=h2tmp->GetNbinsX();
           //printf("idhnu=%d  ts=%s  nbinx=%d\n",idhnu,ts,nbinx);
	         //project all slices on the y-axis
		   cout<<"TEST"<<endl;
           for (Int_t k=1;k<nbinx+1;k+=1){
	           sprintf(ts,"h%d%d",idhnu,k);
             //printf("idnu %d idhnu %d bin%d  ts=%s\n",idnu,idhnu,k,ts);
             pyslice[idhnu][k]=h2tmp->ProjectionY(ts,k,k);
	  		   }
} //end if find object

 //loop over histogram entries


 int neutrinopdgs = 14;

 TH2D* hppt_sndsw = new TH2D("hppt_sndsw","Neutrinos ppt distribution from SNDSW;log10(ptot);log10(pt+0.01)",200,0,4,200,-4,1);
 TH2D* hxy_sndsw = new TH2D("hxy_sndsw","Neutrinos xy distribution from SNDSW;x[cm];y[cm]",200,-100,100,200,-100,100);
 TH1D* hspectrumdet = new TH1D("hnu_mu","Spectrum muon neutrinos arrived at detector;P[GeV/c]",400,0,400);

int neu = 14;
int Nentries = hnu_p->GetEntries();
if (neu<0) idhnu+=1000;
cout<<neu<<" "<<idhnu<<endl;
cout<<"Nentries totali: "<<Nentries<<endl;
Double_t w = hnu_p->Integral()/hnu_p->GetEntries(); //i assume each neutrino to have the same weight (I cannot infer the original weights)
 
for (int i = 0; i < Nentries; i++){
  pzv = hnu_p->GetRandom(); //getting p randomly from 1D histogram (pzv==pz in GENIE reference == p)
  //if (i%100000==0) cout<<i<<endl;

  // Incoming neutrino, get a random px,py
  Double_t pout[3];
  pout[2]=-1.;
  Double_t txnu=0;
  Double_t tynu=0;
  while (pout[2]<0.) {
      //Get pt of this neutrino from 2D hists.
      //Int_t idhnu=TMath::Abs(neu)+idbase;
     // if (neu<0) idhnu+=1000;
      //cout<<neu<<" "<<idhnu<<endl;
      Int_t nbinmx=pxhist[idhnu]->GetNbinsX();
      Double_t pl10=log10(pzv);
      Int_t nbx=pxhist[idhnu]->FindBin(pl10);
      //printf("idhnu %d, p %f log10(p) %f bin,binmx %d %d \n",idhnu,pzv,pl10,nbx,nbinmx);
      if (nbx<1) nbx=1;
      if (nbx>nbinmx) nbx=nbinmx;
      Double_t ptlog10=pyslice[idhnu][nbx]->GetRandom();
      //hist was filled with: log10(pt+0.01)
      Double_t pt=pow(10.,ptlog10)-0.01;
      //rotate pt in phi:
      Double_t phi=gRandom->Uniform(0.,TMath::Pi());
      pout[0] = cos(phi)*pt;
      pout[1] = sin(phi)*pt;
      pout[2] = pzv*pzv-pt*pt;

      if (pout[2]>=0.) {
        pout[2]=TMath::Sqrt(pout[2]);
       // if (gRandom->Uniform(-1.,1.)<0.) pout[0]=-pout[0]; //why these lines?
       // if (gRandom->Uniform(-1.,1.)<0.) pout[1]=-pout[1];

        txnu=pout[0]/pout[2];
        tynu=pout[1]/pout[2];
        //cout << "Info GenieGenerator: neutrino pxyz " << pout[0] << ", " << pout[1] << ", " << pout[2] << endl;
        //printf("param %e %e %e \n",bparam,mparam[6],mparam[7]);
       }
   }
  //if (i%100000==0) cout<<pout[0]<<" "<<pout[1]<<" "<<pout[2]<<" "<<pzv<<endl;
   //end px,py,pz generation, now I can follow my previous procedure for neutrino fluxes

   start[0] = txnu * deltaz; //projecting produced neutrinos to neutrino detector, aggiungere x e y non cambia significativamente il risultato
   start[1] = tynu * deltaz;
   start[2] = -deltaz;
   
   hxy_sndsw->Fill(start[0],start[1]);

   hspectrumdet->Fill(pzv,w);
   Double_t ptot = TMath::Sqrt(pout[0] * pout[0] + pout[1] * pout[1] + pout[2] * pout[2]);
   Double_t pt = TMath::Sqrt(pout[0] * pout[0] + pout[1] * pout[1]);
   hppt_sndsw->Fill(TMath::Log10(ptot),TMath::Log10(pt+0.01));
  } //Nentries
  new TCanvas();
  hspectrumdet->Draw();

  new TCanvas();
  hppt_sndsw->Draw("COLZ");
 
  new TCanvas();
  hxy_sndsw->Draw("COLZ");
}