//script to study MCTrue neutrino interactions (Antonio, 8 November 2021)
//Ideally, this time should organize it into function, so I do not have to write the code each time from scratch
//anytime someone ask me to generate distribution into short time

//used functions


double GetParticleCharge (int pdgcode, TDatabasePDG *pdg){
  //from PDG, get charge
  double charge = 0.;
  if (pdg->GetParticle(pdgcode)) charge = pdg->GetParticle(pdgcode)->Charge();
  else if (pdgcode > 1e+8) charge = 1.; //test storing heavy nuclei
  return charge;
}

bool CheckNeutrinoVertexPosition(TVector3 nu_vertex){
  //target position (to select events in target - values need to be checked for each simulation)
  const Double32_t targetxmin = -47.5000; //instead of -47.6000, 1mm border
  const Double32_t targetxmax = -8.0000;
  const Double32_t targetymin = 15.5000;
  const Double32_t targetymax = 55.0000; //instead of 55.1, 1mm border
     
  if (nu_vertex.X() > targetxmin && nu_vertex.X() < targetxmax && 
      nu_vertex.Y() > targetymin && nu_vertex.Y() < targetymax) return true;
  else return false;
}

bool NeutrinoVertexLocation(int trackID, const ShipMCTrack& track, vector<int> &primary, vector<int> &primaryvisible);


TDatabasePDG *pdg = TDatabasePDG::Instance();
TH1D *hp = new TH1D("hp","Particle momentum at primary vertex;P[GeV/c]",100,0,1000);
TH1D *hn = new TH1D("hn","Number of primaries at primary vertex;Ntracks",50,0,50);
//START MAIN PROGRAM

void studyMCinteractions(){

 //reading input file

 TChain treechain("cbmsim");
 treechain.Add("sndLHC.Genie-TGeant4.root");


 //accessing branches
 TTreeReader reader(&treechain);

 TTreeReaderArray<ShipMCTrack> tracks(reader,"MCTrack");

 TTreeReaderArray<EmulsionDetPoint> emudetpoints(reader,"EmulsionDetPoint");
 TTreeReaderArray<ScifiPoint> scifipoints(reader,"ScifiPoint");

 //variables to be used in the main code
 double weight; //statistical weight, related to density and length of tranversed material
 double nu_vx, nu_vy, nu_vz;

 const int nentries = reader.GetEntries(true);
 cout<<"Number of events "<<nentries<<endl;
 //normalization
 double totalweight;
 double localizedweight;

 //IDs of tracks of interest (Tracks at neutrino vertex)
 vector<int> primary;
 vector<int> primaryvisible;

 //***********************************START OF MAIN LOOP*************************//

 for(int ientry = 0;ientry<nentries;ientry++){
   //clearing list of tracks of interest
   primaryvisible.clear();
   primary.clear();

   if (ientry % 10000== 0) cout<<"arrived at entry "<<ientry<<endl;
   reader.SetEntry(ientry);

   //FIRST CONDITION: check if neutrino interaction is within SND target
   nu_vx = tracks[0].GetStartX();
   nu_vy = tracks[0].GetStartY();
   nu_vz = tracks[0].GetStartZ();
   TVector3 Vn = TVector3(nu_vx, nu_vy, nu_vz);

   bool isintarget = CheckNeutrinoVertexPosition(Vn);

   if (!isintarget) continue; //only process events within nu target

      
   weight = tracks[0].GetWeight();
   totalweight += weight;

   int itrack = 0;
    //*********************************START TRACK LOOP****************************//
     for (const ShipMCTrack& track: tracks){     
         //look for charged particles from primary vertex
         NeutrinoVertexLocation(itrack, track, primary, primaryvisible);
         itrack++;
     } //end track loop
     hn->Fill(primary.size());
 
     if (primaryvisible.size()>0) localizedweight += weight;    
 }//end event loop

 cout<<"Fraction of visible primary vertices "<<(double) localizedweight/totalweight<<endl;

 //drawing histograms
 TCanvas *cp = new TCanvas();
 hp->Draw();

 TCanvas *cn = new TCanvas();
 hn->Draw();

}

bool NeutrinoVertexLocation(int trackID, const ShipMCTrack& track, vector<int> &primary, vector<int> &primaryvisible){
     //********************************SECOND CONDITION: VERTEX LOCATION********/
    const double maxtantheta = 1.;
    const double minmomentum = 1.;
    //getting particle charge and momentum
    double momentum = track.GetP();
    double charge = 0.;
    double pdgcode = track.GetPdgCode();

    double tx = track.GetPx()/track.GetPz();
    double ty = track.GetPy()/track.GetPz();
    double tantheta = TMath::Sqrt(tx * tx + ty * ty);
    if (pdg->GetParticle(pdgcode)) charge = pdg->GetParticle(pdgcode)->Charge(); 
    //is it from the primary neutrino interaction?
    if(track.GetMotherId()==0 && TMath::Abs(charge)>0){
          
          primary.push_back(trackID);
          hp->Fill(momentum);
          //is it visible?      
          if(tantheta<maxtantheta && momentum > minmomentum){ 
            primaryvisible.push_back(trackID); //I do not add the tau lepton or charmed hadron since it decays too soon
            return true;
            //cout<<ientry<<" "<<pdgcode<<" "<<momentum<<" "<<tantheta<<endl;
            }
           else return false;
         }
    else return false;
}
