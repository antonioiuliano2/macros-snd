//Manual check vertices, inspired by macros-ship/FEDRA/manual_check_vertices.C, but with EDA
EdbVertexRec * vertexrec;
EdbEDA *gEDA;
TEventList *lst;
TTree *vtxtree;

float vID_next;

TTree *manualcheck;

int ivtx_next;

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

void Init_qualitytree(TString qualitytree_filename){
 TFile *vertexqualitylist = TFile::Open(qualitytree_filename.Data());
 manualcheck = (TTree*) vertexqualitylist->Get("manualcheck");
 manualcheck->SetBranchAddress("vID",&vID_next);
}

int draw_vertex(int vID){

  Int_t manualcheck;

 //clearing arrays of tracks and vertices
  gEDA->GetVertexSet()->Clear();
  gEDA->GetTrackSet("TS")->Clear();

  Int_t ivtx = lst->GetIndex(vID);
  if (ivtx < 0){
   cout<<" Vertex not found, check selection and number "<<endl;
   return ivtx;
  }

  EdbVertex *vertex = (EdbVertex*) ((TObjArray*)(vertexrec->ePVR->eVTX)) ->At(ivtx);
  gEDA->GetVertexSet()->AddVertex(vertex); // assuming the array is filled with EdbVertex.
  for (int itrack = 0; itrack < vertex->N();itrack++){
   EdbTrackP *track = vertex->GetTrack(itrack);
   gEDA->GetTrackSet("TS")->AddTrack(track);
  }//end loop on tracks  
   //set EDA Vertex
 
  gEDA->Run();
   //we show some information and we ask the user about the vertex
  cout<<"drawn vertex ID "<<vID<<endl;
  return ivtx;
}

void next_manualcheck(){
  if (ivtx_next >= manualcheck->GetEntries()){
   cout<<"arrived at end of file, restart from first"<<endl;
   ivtx_next=0;
  }
  manualcheck->GetEntry(ivtx_next);
  draw_vertex(vID_next);
 
  ivtx_next++;
}


void manual_check_vertices_EDA(TString vertexfilename= "vertextree0.root", TCut vtxcut = "n>2", TString qualitytree_filename = "example_manualcheck_file.root"){
 
 using namespace VERTEX_PAR;
 TFile * inputfile = TFile::Open(vertexfilename.Data());
 TTree *vtxtree = (TTree*) inputfile->Get("vtx");
 vtxtree->Draw(">>lst", vtxcut );
 lst = (TEventList*)gDirectory->GetList()->FindObject("lst");

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

 map<int,EdbTrackP*>emptymap;

 dproc->ReadVertexTree(*vertexrec, vertexfilename.Data(), vtxcut,emptymap);

 cout<<"Reading number of vertices: "<<ali->eVTX->GetEntries()<<endl;
 gEDA = new EdbEDA(ali,false);
 gEDA->GetVertexSet()->StorePrevious();

 cout<<"Loading ended, please draw a vertex with draw_vertex(vID), or next_manualcheck() for reading it from file"<<endl;

 ivtx_next = 0; //start from first
 Init_qualitytree(qualitytree_filename);
}//end code
