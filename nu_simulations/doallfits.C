//Generic function to fit a spline
TGraph* fitspline(int neutrino, TString process, TString target){
  //naming definitions
  const int nneutrinos = 6;
  TString nuname[nneutrinos] = {"nue","numu","nutau","anue","anumu","anutau"};
  TString foldername[nneutrinos]={"nu_e_W184","nu_mu_W184","nu_tau_W184","nu_e_bar_W184","nu_mu_bar_W184","nu_tau_bar_W184"};
  
  TFile *file = TFile::Open((nuname[neutrino]+TString("_xsec.root")).Data());
  //interested only in W184
  TString graphpath = foldername[neutrino]+TString("/")+process+TString("_")+target;

  TGraph *g1 = (TGraph*) file->Get(graphpath.Data());
  if (!g1){
    cout <<"object "<<graphpath.Data()<<" not found! Please check names and paths"<<endl;
    file->ls(foldername[neutrino]);
    return 0;
  }

  const float Emin = 0;
  const float Emax = 5000.;
  
  //TF1 *fitfunc = new TF1((TString("xsecfit_")+process+TString("_")+target).Data(),"pol1",Emin,Emax);
  
  TCanvas *c0 = new TCanvas();
  g1->GetXaxis()->SetTitle("E[GeV]");
  g1->GetYaxis()->SetTitle("Differential cross section [10^-38 cm^2 /GeV]");
  g1->Draw("AP*");

  g1->Fit("pol1");

  return g1;
}

void doallfits(int nupdg){
  const int nneutrinos = 6;
  fstream splinelist("listsplines_tofit.txt",fstream::in);
  //internal ordering, using for arrays in fitspline
  map <int, int> nuordering = {{12,0},{14,1},{16,2},{-12,3},{-14,4},{-16,5}};
  
  TString process, target;
  TString nuname[nneutrinos] = {"nue","numu","nutau","anue","anumu","anutau"};

  fstream fittedsplinelist((nuname[nuordering[nupdg]] + TString("_fittedsplines.txt")).Data(),fstream::out);

  TString filename = nuname[nuordering[nupdg]] + TString("_fittedsplines.root");
  
  TFile *outputfile = new TFile(filename.Data(),"RECREATE");
  TGraph * fitgraph;
  float p0, p1;
  //reading list
  bool inheader = true;
  while (!splinelist.eof() ){
    //prepare header
    if (inheader){
      splinelist >> process;
      splinelist >> target;
      fittedsplinelist << process <<" "<<target<<" "<<"p0 p1"<<endl;
      inheader = false;
    }
    
    splinelist >> process;
    splinelist >> target;
    
    if (process.Length() > 0 ){
      fitgraph = fitspline(nuordering[nupdg],process,target);
      outputfile->cd();
      if(fitgraph){

	fitgraph->Write();
	p0 = fitgraph->GetFunction("pol1")->GetParameter(0);
	p1 = fitgraph->GetFunction("pol1")->GetParameter(1);
	fittedsplinelist << process <<" "<<target<<" "<<p0<<" "<<p1<<endl;

      }
      else{
	fittedsplinelist << process <<" "<<target<<" "<<-1<<" "<<-1<<endl;
      }
    } //end check
    
  }//end loop
  outputfile->Close();
}
