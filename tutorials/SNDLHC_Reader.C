#define SNDLHC_Reader_cxx
#include "SNDLHC_Reader.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

double GetParticleCharge (int pdgcode, TDatabasePDG *pdg){
  //from PDG, get charge
  double charge = 0.;
  if (pdg->GetParticle(pdgcode)) charge = pdg->GetParticle(pdgcode)->Charge();
  else if (pdgcode > 1e+8) charge = 1.; //heavy nuclei are charged, but they are not recognized by the TDatabasePDG
  return charge;
}

bool CheckNeutrinoVertexPosition(TVector3 nu_vertex){
  //target position (to select events in target - values need to be checked for each simulation)
  const Double32_t targetxmin = 8.; 
  const Double32_t targetxmax = 49.;
  const Double32_t targetymin = 15.5;
  const Double32_t targetymax = 56.5;
     
  if (nu_vertex.X() > targetxmin && nu_vertex.X() < targetxmax && 
      nu_vertex.Y() > targetymin && nu_vertex.Y() < targetymax) return true;
  else return false;
}

Int_t SNDLHC_Reader::GetMotherPdg(Int_t trackID){
  if (trackID <= 0){
    cout<<"GetMotherPdg: Error, non positive trackID: "<<trackID<<endl;
    return 0;
  }
  int motherId = MCTrack_fMotherId[trackID];
  return MCTrack_fPdgCode[motherId];
}

void SNDLHC_Reader::Loop()
{
//   How to launch the loop, from a folder with simulation file "ship.conical.Genie-TGeant4.root"
//      root> .L /yourpath/SNDLHC_Reader.C
//      root> SNDLHC_Reader t
//      root> t.Loop();       // Loop on all entries
//
//      Other useful code structures
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.fChain;     // Get tree for quick inspection 
//      (t.fChain->Scan("MCTrack.fPdgCode:MCTrack.fMotherId"), t.fChain->Draw("MCTrack.fStartX"), etc.)
//
//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   //Particle database, converting from int pdgcode to particle. Usage: 
   //pdg->GetParticle(pdgcode)->Charge() (Mass(), etc. see https://root.cern.ch/doc/master/classTParticlePDG.html)
   //N.B. please check if particle is found with if(pdg->GetParticle(pdgcode))! Code will crash otherwise!
   TDatabasePDG *pdg = TDatabasePDG::Instance();
   //*************HISTOGRAMS AND VARIABLES************
   //neutrino position
   TH2D *hvxy = new TH2D("hvxy","Position of neutrino interaction vertices in the transverse plane;x[cm];y[cm]", 200,-100,100,200,-100,100);
   TH2D *hvzy = new TH2D("hvzy","Position of neutrino interaction vertices in the zy plane;z[cm];y[cm]",60,-30,30,200,-100,100);
   //neutrino position only in target
   TH2D *hvxy_intarget = new TH2D("hvxy_intarget","Position of neutrino interaction vertices in the transverse plane;x[cm];y[cm]", 100,0,100,100,0,100);
   TH2D *hvzy_intarget = new TH2D("hvzy_intarget","Position of neutrino interaction vertices in the zy plane;z[cm];y[cm]",60,-30,30,100,0,100);
   //muon filter position
   TH2D *hmuzy = new TH2D("hmuzy","Position of Muon Filter hits in the zy plane;z[cm];y[cm]",200,40,240,80,0,80);
   //*************START LOOP*****************
   cout<<"Starting loop over "<<nentries<<" events "<<endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      //taking the next event
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      //first, we need to get general neutrino event information
      //(itrk 0 is the first neutrino, which "starts" at the interaction point but it is not propagated)   
      //event weight: mean density times length: sum(x_i*rho_i)/sum(x_i) [g/cm3] * sum(x_i) [cm]
      Double32_t weight = MCTrack_fW[0];
      //position of interaction vertex
      TVector3 nu_vertex = TVector3(MCTrack_fStartX[0], MCTrack_fStartY[0], MCTrack_fStartZ[0]);
      bool isintarget = CheckNeutrinoVertexPosition(nu_vertex); //did the interaction happen in the target?
      //neutrino momentum
      TVector3 nu_P = TVector3(MCTrack_fPx[0], MCTrack_fPy[0], MCTrack_fPz[0]); 
        
      hvxy->Fill(nu_vertex.X(), nu_vertex.Y(),weight);
      hvzy->Fill(nu_vertex.Z(), nu_vertex.Y(),weight);
      if (isintarget){
         hvxy_intarget->Fill(nu_vertex.X(), nu_vertex.Y(),weight);
         hvzy_intarget->Fill(nu_vertex.Z(), nu_vertex.Y(),weight);
      }

      //*******loop on tracks****
      for(int itrk = 0; itrk < MCTrack_;itrk++){
        Int_t trackpdg = MCTrack_fPdgCode[itrk];
        Int_t procID = MCTrack_fProcID[itrk];
        //looking for muons from pions
        if (itrk > 0){//no sense looking for mother of primary muon
         Int_t motherpdg = GetMotherPdg(itrk);
         if (TMath::Abs(trackpdg) == 13 && TMath::Abs(motherpdg) == 211 && procID == 4){ //muons from pion decays
            //cout<< "Event "<<jentry <<" muon ID "<<itrk<< " with pdg "<<trackpdg<<" mother "<<motherpdg<<endl;
         }
         
        }
	    //here you can do whatever you need to do with the tracks, for now leaving blank
      } //***end of loop over tracks***  

      //******loop on MuonFilter hits*****
      for(int ihit = 0; ihit < MuFilterPoint_; ihit++){
        TVector3 mufilterhit_pos = TVector3(MuFilterPoint_fX[ihit], MuFilterPoint_fY[ihit], MuFilterPoint_fZ[ihit]);        
        Double32_t charge = GetParticleCharge(MuFilterPoint_fPdgCode[ihit],pdg);
        Int_t detID = MuFilterPoint_fDetectorID[ihit];   
        //detID notation: 1000-1010 for first 5 layers;
        //for last 3 layers: 1000-1076 horizontal, 100000-100076 vertical

        if (TMath::Abs(charge)>0.){ //usually we are interested only in charged particles
         hmuzy->Fill(mufilterhit_pos.Z(), mufilterhit_pos.Y(), weight);
        }        
      } //***end of loop over MuonFilter hits***   

   }
   //**************END LOOP******************
   //drawing histograms
   TCanvas * c1 = new TCanvas();
   c1->Divide(1,2);
   c1->cd(1);
   hvxy->Draw("COLZ");
   c1->cd(2);
   hvzy->Draw("COLZ");
      
   TCanvas * c2 = new TCanvas();
   c2->Divide(1,2);
   c2->cd(1);
   hvxy_intarget->Draw("COLZ");
   c2->cd(2);
   hvzy_intarget->Draw("COLZ");

   TCanvas * c3 = new TCanvas();
   hmuzy->Draw("COLZ");
}
