//selecting neutrinos within x and y acceptance of detectors
bool inacceptance(double x, double y){
  const double xmin = -47.6;
  const double xmax = -8.0;
  const double ymin = 15.5;
  const double ymax = 55.1;

  if (x < xmin || x > xmax) return false;
  if (y < ymin || y > ymax) return false;
  return true;
}
//saving 1D Ekin distribution of neutrinos within acceptance. {A.Iuliano, 18 December 2020}
//ineutrino: 0 = nue, 1 = numu, 2 = nutau, 3 = anue, 4= anumu, 5 = anutau
int Acceptance1DSpectra(int ineutrino){
  //names of histogram and files change with neutrino topology
  const int nfiles = 6;
  if (ineutrino < 0 || ineutrino >= nfiles){
    cout<<"Outside Range, stopping now"<<endl;
    return 1;
  }
  TString filenames[nfiles] =
    {"NeutElec.root","NeutMuon.root","NeutTau_filter.root","AntiNeutElec.root","AntiNeutMuon.root","AntiNeutTau_filter.root"};
  TString histonames[nfiles] =
    {"1012","1014","1016","2012","2014","2016"};
  TString histonames2D[nfiles] =
    {"1212","1214","1216","2212","2214","2216"};
  
  //open file with neutrino input file, adding pt distribution
  ROOT::RDataFrame df("t",filenames[ineutrino].Data());
  auto dfpt = df.Define("pt","TMath::Sqrt(TMath::Power(Ekin.EKin * x_cos,2) + TMath::Power(Ekin.EKin * y_cos,2))");
  auto df_logE = dfpt.Define("log10Ekin","TMath::Log10(Ekin.EKin)");
  auto df_full = df_logE.Define("log10pt_offset","TMath::Log10(pt+0.01)");
  
  //require neutrinos in detector acceptance
  auto df_selected = df_full.Filter(inacceptance,{"x","y"});
  auto columns = df_selected.GetColumnNames();
  for (auto &col: columns) cout<<col<<endl;


  TFile *spectrumfile = new TFile("neutrinos_SNDacceptance.root","UPDATE");
  
  //drawing histograms, (N.D.R. like Annarita, I need to fill them with the weights too!)
  TCanvas *cEkin = new TCanvas();
  auto hEkin = df_selected.Histo1D({histonames[ineutrino].Data(),"Energy of neutrinos in acceptance;E[GeV]",250,0.,5000.},"Ekin.EKin","weight");
  hEkin->DrawClone();
  TCanvas *cxy = new TCanvas();
  auto hxy = df_selected.Histo2D({"hxy","Selected position of neutrinos;x[cm];y[cm]",50,-50.,0.,60,0.,60.},"x","y","weight");
  hxy->DrawClone("COLZ");

  TCanvas *cppt = new TCanvas();
  auto hppt = df_selected.Histo2D({histonames2D[ineutrino].Data(),"2D spectrum of neutrinos in acceptance;log10Ekin;log10Pt+0.01",200,0.,4.,200,-4.,1.},"log10Ekin","log10pt_offset","weight");
  hppt->DrawClone("COLZ");
  

  hEkin->Write();
  hppt->Write();

  spectrumfile->Close();

  return 0;
}
