EdbPVRec     *gAli=0;
void efficiency_study(){ //efficiency estimation, as used in OPERA paper

 //TCut trcut = "t.eFlag>=0  &&t.eProb>0.01";
 TCut trcut = "t.eFlag>=0 &&t.eProb>0.01&&nseg >= 18";
 const int nfilms = 25;

 TEfficiency* heff = NULL;

 TH1D *hexpected = new TH1D("hexpected", "Tracks expected to be found in each plate", 58,0,58);
 TH1D *hfound = new TH1D("hfound", "Tracks with an associated segment in each plate", 58,0,58); 

 TH2D *hxy = new TH2D("hxy", "2D position of tracks",190,0,190,190,0,190);

 //reading tracks from linked_tracks.root file
 EdbDataProc *dproc = new EdbDataProc();
 dproc->InitVolume(100, trcut); //100 is the code for track reading (as from fedra/src/libEIO/EdbDataSet.cxx)
 gAli = dproc->PVR();
 const int ntracks = gAli->eTracks->GetEntries();
 printf("ntracks after cut = %d\n", ntracks);

 //begin track loop
 EdbTrackP *trk = NULL;
 EdbSegP *seg = NULL;
 EdbSegP *firstseg = NULL;
 EdbSegP *lastseg = NULL;

 int nplate;
 for (int itrk = 0; itrk <ntracks; itrk++){
  trk = (EdbTrackP*)(gAli->eTracks->At(itrk)); //accessing track object
  //expected to found the track from first to last segment
  firstseg = trk->GetSegmentFirst();
  lastseg = trk->GetSegmentLast();
  for (int i = 7; i <= 31;i++){
  //for (int i = firstseg->Plate(); i <= lastseg->Plate();i++){
   hexpected->Fill(i);
  }
  hexpected->Fill(0);//for having y range set from 0 to 1 I add a bin with null efficiency for Plate 0
  hexpected->Fill(1);//for having y range set from 0 to 1 I add a bin with null efficiency for Plate 1
  hexpected->Fill(2);//for having y range set from 0 to 1 I add a bin with null efficiency for Plate 2
  hexpected->Fill(3);//for having y range set from 0 to 1 I add a bin with null efficiency for Plate 3 
  
  for (int iseg = 0; iseg < trk->N();iseg++){ //loop on associated segments
    seg = (EdbSegP*) trk->GetSegment(iseg);
    nplate = seg->Plate();
    //if(nplate!=15)
    hfound->Fill(nplate);
  }
  
  hxy->Fill(trk->X()/1000., trk->Y()/1000.);
 } //end of track loop

 //Getting efficiency for all plates
 TCanvas *c1 = new TCanvas();
 if (TEfficiency::CheckConsistency(*hfound,*hexpected)){
  heff = new TEfficiency(*hfound, *hexpected);
  heff->Draw();
  heff->SetTitle(";Plate;#epsilon");
  //estimating global efficiency and its error
  double effplate[nfilms]; //efficiency in each plate
  double mean = 0;
  double sd = 0; //standard deviation
  double sd_mean = 0; //standard deviation of the mean (standard error)
  
  for (int ibin=0; ibin < nfilms; ibin++){ 
   effplate[ibin] = heff->GetEfficiency(ibin+8);//counting only plates used, witout first and last   
   mean+=effplate[ibin];
   cout<<ibin<<" "<<effplate[ibin]<<endl;
  }
  mean = mean/nfilms;
  for (int ibin=0; ibin < nfilms; ibin++){
   sd+=pow((effplate[ibin]-mean),2);
  }
  sd=TMath::Sqrt(sd/(nfilms-1));
  sd_mean = sd/TMath::Sqrt(nfilms);
  cout<<"Mean efficiency: "<<mean<<"with error: "<<sd_mean<<endl;
 }

 TCanvas *c2 = new TCanvas();
 hxy->Draw("COLZ");
}
