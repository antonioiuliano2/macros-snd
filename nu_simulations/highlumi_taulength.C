//plotting distance covered by tau lepton in High Lumi SNDLHC extended design (22 July 2026 created by A. Iuliano)
void highlumi_taulength(){
  //normalization numbers, from https://arxiv.org/pdf/2602.21881
  const double Nnutau = 2.4e+2; //expected number of tau neutrinos in the Target
  const double Nnutaubar = 1.2e+2; //expected number of tau antineutrinos in the Target

  TString xrootdprepath("root://eospublic.cern.ch/");
  TString prepath("/eos/experiment/sndlhc/users/aiulian/ADVSND/August_2024_con_scavo/2026_07_22_nutau_AdvSND_August2024/");
  //TString prepath("/eos/experiment/sndlhc/MonteCarlo/AdvSND/TP_2025/nu_tau_mock_digi_2/volume_volAdvTarget/");
  
  TChain treechain("cbmsim");
  int nfirstfile = 1;
  int nlastfile = 100;
  for (int ifile = nfirstfile; ifile<=nlastfile;ifile++){
    treechain.Add((xrootdprepath+prepath+TString(Form("%i/sndLHC.Genie-TGeant4.root",ifile))).Data());
  }

  TTreeReader reader(&treechain);
  TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");

  const int nentries = reader.GetEntries(true);
  cout<<"Number of events "<<nentries<<endl;

  const int nbinslength = 40;
  const double minlength = 0.01;
  const double maxlength = 100.;

  ROOT::RVec<double> logbins = ROOT::VecOps::Logspace<double, double>(TMath::Log10(minlength),TMath::Log10(maxlength),nbinslength+1);

  TH1D *htau_length = new TH1D("htau_length","Tau lepton length in SNDLHC extended design;dL[cm]",nbinslength,minlength,maxlength);
  TH1D *htaubar_length = new TH1D("htaubar_length","Tau lepton length in SNDLHC extended design;dL[cm]",nbinslength,minlength,maxlength);

  TH1D *htau_loglength = new TH1D("htau_loglength","Tau lepton length in SNDLHC extended design;dL[cm]",nbinslength,logbins.data());
  TH1D *htaubar_loglength = new TH1D("htaubar_loglength","Tau lepton length in SNDLHC extended design;dL[cm]",nbinslength,logbins.data());


  for (int ientry=0; ientry<nentries;ientry++){
    reader.SetEntry(ientry);
    //getting tau lepton from tau neutrino
     for (const ShipMCTrack& track: tracks){     
        int motherid = track.GetMotherId();
        if (motherid == 1){
            //safety check to avoid entries with no tau track stored
            if (TMath::Abs(tracks[motherid].GetPdgCode()) != 15) break; 
            //compute length
            double decay_vx = track.GetStartX();
            double decay_vy = track.GetStartY();
            double decay_vz = track.GetStartZ();

            double nu_vx = tracks[motherid].GetStartX();
            double nu_vy = tracks[motherid].GetStartY();
            double nu_vz = tracks[motherid].GetStartZ();

            //cout<<"Debug : "<<decay_vz<< " "<<nu_vz<<endl;

            double dL = TMath::Sqrt((decay_vx - nu_vx)*(decay_vx - nu_vx) + 
            (decay_vy - nu_vy)*(decay_vy - nu_vy) + (decay_vz - nu_vz)*(decay_vz- nu_vz));
            if (tracks[motherid].GetPdgCode() == 15){ 
                htau_length->Fill(dL);
                htau_loglength->Fill(dL);
            }
            else{ 
                htaubar_length->Fill(dL);
                htaubar_loglength->Fill(dL);
            }

            break; //only filling one entry per event, regardless of tau daughters number

        } //end check for tau daughter
     } // end loop over MCTracks
    } //end loop over entries
  gStyle->SetOptStat("irm");
  //normalize to expected number of tau and anti-tau events
  if (htau_length->Integral() > 0.)
   htau_length->Scale(Nnutau/htau_length->Integral());
  if (htaubar_length->Integral() > 0.)
   htaubar_length->Scale(Nnutaubar/htaubar_length->Integral());
  //sum them together
  htau_length->Add(htaubar_length);
  TCanvas *ctau = new TCanvas();
  htau_length->Draw("histo");

  if (htau_loglength->Integral() > 0.)
   htau_loglength->Scale(Nnutau/htau_loglength->Integral());
  if (htaubar_length->Integral() > 0.)
   htaubar_loglength->Scale(Nnutaubar/htaubar_loglength->Integral());
  //sum them together
  htau_loglength->Add(htaubar_loglength);
  TCanvas *ctau_log = new TCanvas();
  htau_loglength->Draw("histo");
  ctau_log->SetLogx();
}