EdbDataProc  *dproc=0;
EdbPVRec     *gAli=0;
#include <stdlib.h>

int FindMostCommonPdgSeg(EdbTrackP *track)
{
  map <int, int> nseg_pdg;
  int mostcommmonpdg = -1;
  int maxnsegments = -1;
  //looping over track segments, counting them
  for (int iseg = 0; iseg < track->N(); iseg++){

    EdbSegP * myseg = track->GetSegment(iseg);
    nseg_pdg[myseg->Vid(0)]++; 
  }
  //which Particle PDG has the most segments?

  for (auto && [pdgcode,nsegments]:nseg_pdg){
    //find maxnsegments
    if (nsegments > maxnsegments){
      maxnsegments = nsegments;
      mostcommmonpdg = pdgcode;
    } //end if condition

  } //end loop on map
  return mostcommmonpdg;
}

int FindMostCommonEvent(EdbTrackP *track)
{
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

int FindMostCommonMCTrack(EdbTrackP *track)
{
  map <int, int> nseg_MCTrack;
  int mostcommonmctrack = -1;
  int maxnsegments = -1;
  //looping over track segments, counting them
  for (int iseg = 0; iseg < track->N(); iseg++){

    EdbSegP * myseg = track->GetSegment(iseg);
    nseg_MCTrack[myseg->MCTrack()]++; 
  }
  //which MCTrack has the most segments?

  for (auto && [MCTrack,nsegments]:nseg_MCTrack){
    //find maxnsegments
    if (nsegments > maxnsegments){
      maxnsegments = nsegments;
      mostcommonmctrack = MCTrack;
    } //end if condition

  } //end loop on map
  return mostcommonmctrack;
}

bool CheckDiffMCEvt(EdbTrackP *track)
{
	Int_t track_MCEvt = FindMostCommonEvent(track);
	bool flag = false;
	for (int iseg = 0; iseg < track-> N(); iseg++){
		EdbSegP *myseg = track->GetSegment(iseg);	
		if (track_MCEvt != myseg->MCEvt()) flag = true;
	}
	return flag;
}
