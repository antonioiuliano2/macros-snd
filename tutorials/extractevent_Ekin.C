//testing function to extract event randomly from TTree with a given energy

int extractevent_Ekin(double Ekin, double DeltaE, TTree* inputneutrinos){

  TCut nucut(Form("Ekin >= (%f - %f) && Ekin < (%f + %f)",Ekin,DeltaE/2., Ekin, DeltaE/2.));
  inputneutrinos->Draw(">>nulist", nucut );
  TEventList *nulist = (TEventList*)gDirectory->GetList()->FindObject("nulist");
  int nselectedevents = nulist->GetN();
  //integer function returns randomly integer between 0 e nselectedevents;
  int myevent = nulist->GetEntry(gRandom->Integer(nselectedevents+1));
 
  return myevent;
}

void extractevent_Ekin(){
    double energy = 230.;
    double erange = 10.;
    TFile *inputfile = TFile::Open("NeutMuon.root");
    TTree *neutrinos = (TTree*) inputfile->Get("t");
    cout<<"Test with energy "<<energy<<" I get the event "<<extractevent_Ekin(energy, erange, neutrinos)<<endl;
}