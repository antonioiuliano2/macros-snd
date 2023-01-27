//Loading tracks with EDA, displaying vertices and preparing analysis
//To reconstruct other vertices, please use gEDA->GetTrackSet("TS")->SetDraw(kTRUE) before it
//Tested to launch shower reconstruction also
namespace VERTEX_PAR
{
  float DZmax = 3000.;
  //float ProbMinV   = 0.0001;  // minimum acceptable probability for chi2-distance between tracks
  float ProbMinV   = 0.01;
  float ImpMax     = 15.;    // maximal acceptable impact parameter [microns] (for preliminary check)
  bool  UseMom     = false;  // use or not track momentum for vertex calculations
  bool  UseSegPar  = true;  // use only the nearest measured segments for vertex fit (as Neuchatel)
  int   QualityMode= 0;      // vertex quality estimation method (0:=Prob/(sigVX^2+sigVY^2); 1:= inverse average track-vertex distance)
}

int FindMostCommonEvent(EdbTrackP *track);
void loadEDA_vertextracks(bool newversion = true, TString vertexfilename= "vertextree.root"){
 using namespace VERTEX_PAR;
 const int nvertices = 1;
 int vertexlist[nvertices] = {1914};
 //int vertexcolors[nvertices] = {kRed,kRed,kRed,kRed};
 //int vertexlist[nvertices] = {260, 273, 280, 393};
 int vertexcolors[nvertices] = {kRed};
 EdbDataProc *dproc = new EdbDataProc();

 TFile * inputfile = TFile::Open(vertexfilename.Data());
 EdbVertexRec *vertexrec; 

 TObjArray *drawnvertices = new TObjArray(100);
 TObjArray *drawntracks = new TObjArray(100);

// EdbTrackP *specialtrack = new EdbTrackP();

 EdbPVRec *ali = new EdbPVRec();
 EdbScanCond *scancond = new EdbScanCond();
 ali->SetScanCond(scancond);

 for (int i = 0; i < nvertices; i++){ //range for loop, C++11
  int vID = vertexlist[i];
  
  EdbVertex *vertex = 0;

  if (newversion){ 
    vertexrec = new EdbVertexRec();
    vertexrec->SetPVRec(ali);
    vertexrec->eDZmax=DZmax;
    vertexrec->eProbMin=ProbMinV;
    vertexrec->eImpMax=ImpMax;
    vertexrec->eUseMom=UseMom;
    vertexrec->eUseSegPar=UseSegPar;
    vertexrec->eQualityMode=QualityMode;

    vertex = dproc->GetVertexFromTree(*vertexrec,vertexfilename,vID);
  }
  else{
    vertexrec = (EdbVertexRec*) inputfile->Get("EdbVertexRec");
    vertex = (EdbVertex*) vertexrec->eVTX->At(vID);
  }

  vertex->SetXYZ(vertex->VX(),vertex->VY(),vertex->VZ()); //EDA USES X(), NOT VX(). Here I want the display to match my coordinates
  drawnvertices->Add(vertex); // assuming the array is filled with EdbVertex.
  for (int itrk = 0; itrk < vertex->N(); itrk++){
     EdbTrackP* track =  vertex->GetTrack(itrk);     
     for (int iseg = 0; iseg < track->N(); iseg++) track->GetSegment(iseg)->SetFlag(vertexcolors[i]); // to color them differently
 //    for (int iseg = 0; iseg < track->N(); iseg++) track->GetSegment(iseg)->SetFlag(vertexcolors[i]); // to color them differently
     drawntracks->Add(track);
//     specialtrack = track;
  }
 }

 ali->eTracks = drawntracks;
 ali->eVTX = drawnvertices;
 //EdbEDA * eda = new EdbEDA(ali); // init DataSet but doesn't read linked_track.root
 //eda->GetTrackSet("TS")->SetColorMode(kCOLOR_BY_ID);
 
 //gEDBDEBUGLEVEL = 3;
 //now, all tracks
 EdbEDA* eda = new EdbEDA("b000431.0.0.0.trk.root",-1,"1",kFALSE);
 eda->GetTrackSet("TS")->SetDraw(kFALSE);

 //preparing PIDs
 EdbPVRec * pvrec_linkedtracks = eda->GetTrackSet("TS")->GetPVRec();
 int npatterns = pvrec_linkedtracks->Npatterns();
 for (int ipat = 0; ipat < npatterns; ipat++){
  EdbPattern * pat = pvrec_linkedtracks->GetPattern(ipat);
  pat->SetPID(pat->GetSegment(0)->PID());
 }
 //eda->GetTrackSet("BT")->SetDraw(kFALSE); //used for shower
 /*int ntracks = eda->GetTrackSet("TS")->N();
 //copy into BT for showering
 for (int itrack = 0; itrack < ntracks;itrack++){ 
  EdbTrackP *track = eda->GetTrackSet("TS")->GetTrack(itrack);
  eda->GetTrackSet("BT")->AddTrack(track);
 }*/

 for (int ivtx = 0; ivtx < ali->eVTX->GetEntries(); ivtx++){
  EdbVertex* vtx = (EdbVertex*) ali->eVTX->At(ivtx);
  eda->GetVertexSet()->AddVertex(vtx);
 }


 for (int itrack = 0; itrack < drawntracks->GetEntries();itrack++){
  EdbTrackP *track = (EdbTrackP*) drawntracks->At(itrack);
  eda->GetTrackSet("SB")->AddTrack(track);
  //eda->GetTrackSet("TS")->RemoveTrack(track); //if we want to remove the track from other dataset
  
 }
/* eda->GetTrackSet("TS")->RemoveTrack(specialtrack); //charm colored differently
 eda->GetTrackSet("SB")->SetTrackAttribute(4);
 eda->GetTrackSet("SB")->AddTrack(specialtrack);*/
 eda->Run();
}


void drawEDAvertices(bool newversion = true, TString vertexfilename= "vertextree.root", bool applycut = false){
 //EDA tracks loading and initialization
 EdbEDA* eda = new EdbEDA("b000431.0.0.0.trk.root",-1,"1",kFALSE);
 eda->GetTrackSet("TS")->SetDraw(kFALSE);
 
 using namespace VERTEX_PAR;
 TFile * inputfile = TFile::Open(vertexfilename.Data());
 TTree *vtxtree = (TTree*) inputfile->Get("vtx");
 EdbVertexRec *vertexrec;
 EdbDataProc *dproc = new EdbDataProc();


 TObjArray *drawnvertices = new TObjArray(1000000);
 TObjArray *drawntracks = new TObjArray(1000000);

// EdbTrackP *specialtrack = new EdbTrackP();

 EdbPVRec *ali = new EdbPVRec();
 EdbScanCond *scancond = new EdbScanCond();
 ali->SetScanCond(scancond);
 
 vertexrec = new EdbVertexRec();
 vertexrec->SetPVRec(ali);
 vertexrec->eDZmax=DZmax;
 vertexrec->eProbMin=ProbMinV;
 vertexrec->eImpMax=ImpMax;
 vertexrec->eUseMom=UseMom;
 vertexrec->eUseSegPar=UseSegPar;
 vertexrec->eQualityMode=QualityMode;

 EdbVertex *vertex = 0;
 const int nvertices = vtxtree->GetEntries();
 cout<<"Reading number of vertices: "<<nvertices<<endl;
 map<int,EdbTrackP*>emptymap;

 if (newversion){
  dproc->ReadVertexTree(*vertexrec, "vertextree.root", "1",emptymap);
 }

 for (int i = 0; i < nvertices; i++){ //range for loop, C++11
  int vID = i;

  if (newversion){
    vertex = (EdbVertex*) ((TObjArray*)(ali->eVTX)) ->At(vID);
  } 
  else{ 
    vertexrec = (EdbVertexRec*) inputfile->Get("EdbVertexRec");
    vertex = (EdbVertex*) vertexrec->eVTX->At(vID);
  }
  if(applycut){
   if (vertex->N() > 10) drawnvertices->Add(vertex);
   for (int itrk = 0; itrk < vertex->N(); itrk++){
     EdbTrackP* track =  vertex->GetTrack(itrk);          
     if (vertex->N() > 10) drawntracks->Add(track);
   }
  }
  else{
  	drawnvertices->Add(vertex); // assuming the array is filled with EdbVertex.
  	for (int itrk = 0; itrk < vertex->N(); itrk++){
    	EdbTrackP* track =  vertex->GetTrack(itrk);          
 //    	for (int iseg = 0; iseg < track->N(); iseg++) track->GetSegment(iseg)->SetFlag(vertexcolors[i]); // to color them differently
     	drawntracks->Add(track);
//     	specialtrack = track;
  	}
  }
 }

 ali->eTracks = drawntracks;
 ali->eVTX = drawnvertices; 

 //adding vertices
 for (int ivtx = 0; ivtx < ali->eVTX->GetEntries(); ivtx++){
  EdbVertex* vtx = (EdbVertex*) ali->eVTX->At(ivtx);
  eda->GetVertexSet()->AddVertex(vtx);
 }

 //adding tracks
 for (int itrack = 0; itrack < drawntracks->GetEntries();itrack++){
  EdbTrackP *track = (EdbTrackP*) drawntracks->At(itrack);
  eda->GetTrackSet("SB")->AddTrack(track);
  //eda->GetTrackSet("TS")->RemoveTrack(track); //if we want to remove the track from other dataset
  
 }

 //eda->GetTrackSet("TS")->SetColorMode(kBLACKWHITE);
 eda->Run();
}
