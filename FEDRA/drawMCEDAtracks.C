
int FindMostCommonEvent(EdbTrackP *track);

void drawMCEDAtracks(){
 //which tracks do you want to draw?
 //TCut mycut("t.eMCEvt==48");

 TCut firstcut("1"); //first general cut

//getting tracks set from file
 EdbEDA* gEDA = new EdbEDA("linked_tracks.root",-1,firstcut, kFALSE);
 
 EdbEDATrackSet *alltrackset = gEDA->GetTrackSet("TS"); 
 EdbEDATrackSet *selectedtrackset = gEDA->GetTrackSet("SF");


 const Int_t minnseg = 45;
 const Int_t whichevent = 4;
 const Int_t whichtrack = 219;

 alltrackset->SetNsegCut(minnseg);
 gEDA->Run();
 gEDA->GetMainTab()->ExecCuts(0); //callid EdbEDAMainTab() to recompute sets with nsegcut, 0 does not redraw

 int ntracks = alltrackset->N();

 //loop over tracks
 for (int itrack = 0; itrack < ntracks; itrack++){

  EdbTrackP *track = alltrackset->GetTrack(itrack);
  //if (track->MCEvt()==whichevent) selectedtrackset->AddTrack(track);
  //if (FindMostCommonEvent(track) == whichevent) selectedtrackset->AddTrack(track);
	//if (itrack == whichtrack) selectedtrackset->AddTrack(track);
 }

// color selection
 alltrackset->SetColorMode(kCOLOR_BY_ID);
 selectedtrackset->SetTrackAttribute(-1);
 selectedtrackset->SetColorMode(kCOLOR_BY_PARTICLE);
 alltrackset->SetDraw(kFALSE);
 selectedtrackset->SetDraw(kTRUE);
 gEDA->Redraw();
 gEDA->Run();

 

} 

int FindMostCommonEvent(EdbTrackP *track){

  map <int, int> nseg_MCEvt;
  int mostcommonevent = -1;
  int maxnsegments = -1;
  //looping over track segments, counting them
  for (int iseg = 0; iseg < track->N(); iseg++){

    EdbSegP * myseg = track->GetSegment(iseg);
    nseg_MCEvt[myseg->MCEvt()]++; 
  }
  //which MCEvent has the most segments?

  for (auto && [MCEvent,nsegments]:nseg_MCEvt){
    //find maxnsegments
    if (nsegments > maxnsegments){
      maxnsegments = nsegments;
      mostcommonevent = MCEvent;
    } //end if condition

  } //end loop on map
  return mostcommonevent;
}
