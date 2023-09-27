//Loop over a list of interesting candidates, savepics() to obtain pictures (Created by A.Iuliano on date 2023/09/27)
EdbVertexRec * vertexrec;
EdbEDA *gEDA;
EdbDataProc *dproc;

float vID_next;

TTree *manualcheck;

int ivtx_next;
float ix;
float iy;
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
void savepics(){
 system(Form("mkdir %i_%i_%i",(int) ix, (int) iy,(int) vID_next));
 gEDA->SavePictures();
 //gEDA->GetGLViewer()->SavePicture("test.gif");
 system(Form("mv x.gif %i_%i_%i/",(int) ix, (int) iy,(int) vID_next));
 system(Form("mv y.gif %i_%i_%i/",(int) ix, (int) iy,(int) vID_next));
 system(Form("mv z.gif %i_%i_%i/",(int) ix, (int) iy,(int) vID_next));
 system(Form("mv b.gif %i_%i_%i/",(int) ix, (int) iy,(int) vID_next));
}

void nextentry(){
 ivtx_next++;
 manualcheck->GetEntry(ivtx_next);
 //getting filename again
 TString prefix = "root:://eospublic.cern.ch/";
 TString path = "/eos/experiment/sndlhc/emulsionData/2022/emureco_Napoli/RUN3/b000044/trackfiles/rootfiles/57_21/";
 TString cellfile = Form("cell_%i_%i/vertextree.root",(int) ix, (int) iy);
 TString vertexfilename = prefix + path + cellfile;
 //opening file and getting vertex
 EdbVertex *vertex = dproc->GetVertexFromTree(*vertexrec, vertexfilename.Data(), (int) vID_next);
 gEDA->AddVertex(vertex);
 for (int itrack = 0; itrack < vertex->N();itrack++){
   EdbTrackP *track = vertex->GetTrack(itrack);
   gEDA->GetTrackSet("TS")->AddTrack(track);
 }//end loop on tracks 
 gEDA->Run();
 //after running, removing
 gEDA->GetVertexSet()->RemoveVertex(vertex);
 for (int itrack = 0; itrack < vertex->N();itrack++){
   EdbTrackP *track = vertex->GetTrack(itrack);
   gEDA->GetTrackSet("TS")->RemoveTrack(track);
 }//end loop on tracks 
}//end next entry code


void prepare_vertex_rec(TCut vtxcut = "n>2", TString qualitytree_filename = "example_manualcheck_file.root"){
 
 using namespace VERTEX_PAR;

 dproc = new EdbDataProc();

 TString prefix = "root:://eospublic.cern.ch/";
 TString path = "/eos/experiment/sndlhc/emulsionData/2022/emureco_Napoli/RUN3/b000044/trackfiles/rootfiles/57_21/";

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

 ivtx_next = 0; //start from first
 TFile *vertexqualitylist = TFile::Open(qualitytree_filename.Data());
 manualcheck = (TTree*) vertexqualitylist->Get("manualcheck");
 manualcheck->SetBranchAddress("ix",&ix);
 manualcheck->SetBranchAddress("iy",&iy);
 manualcheck->SetBranchAddress("vID",&vID_next);
 //already do the first one
 manualcheck->GetEntry(ivtx_next);

 TString cellfile = Form("cell_%i_%i/vertextree.root",(int) ix, (int) iy);
 TString vertexfilename = prefix + path + cellfile;

 EdbVertex *myvertex = dproc->GetVertexFromTree(*vertexrec, vertexfilename.Data(), (int) vID_next);
 gEDA = new EdbEDA(ali, false);
 gEDA->SetDrawAreaSet(kFALSE);
 gEDA->Run();
 for (int itrack = 0; itrack < myvertex->N();itrack++){
   EdbTrackP *track = myvertex->GetTrack(itrack);
   gEDA->GetTrackSet("TS")->RemoveTrack(track);
 }//end loop on tracks 
 //savepics();
 gEDA->GetVertexSet()->RemoveVertex(myvertex);
 //next one, as cross check
 //vID_next = vID_next + 1;

 
 //savepics(); 
}//end code
//example
/*void auto_loop_EDA_vertex.C(){
  prepare_vertex_rec();
  savepics();
  nextentry();
  savepics();
  nextentry();
  savepics();
}
*/