//how much times does reading vertices require? How we can optimize it?
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

int timecheck_readvertices(){
 using namespace VERTEX_PAR;
 //reading vertex file, getting vertex tree
 TFile *inputfile = TFile::Open("vertextree.root");
 TTree *vtxtree = (TTree*) inputfile->Get("vtx");
 
 EdbVertexRec *vertexrec;
 EdbDataProc *dproc = new EdbDataProc();

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
 
 const int nvertices = vtxtree->GetEntries();
 cout<<"Test reading "<<nvertices<<" vertices "<<endl;
 
 TStopwatch timewatch;
 map<int,EdbTrackP*>emptymap;
 //attuale metodo
 /*
 for (int vID = 0; vID < nvertices; vID++){
  EdbVertex * vertex = dproc->GetVertexFromTree(*vertexrec,"vertextree.root",vID);
 }
 */
 //metodo "nuovo"
 //first loop, filling the array
 dproc->ReadVertexTree(*vertexrec, "vertextree.root", "1",emptymap);
 
 TObjArray *vrec = (TObjArray*) ali->eVTX;
 //sanity check
 if (nvertices != vrec->GetEntries()){
  cout<<"ERROR: vertices numbers do not match!!"<<endl;
  return 1;
 }
 //second loop, getting vertices
 for (int vID = 0; vID < nvertices; vID++){
  EdbVertex * vertex = (EdbVertex*) vrec->At(vID);
 }
 //testing track matching
 //EdbVertex * testvertex = (EdbVertex*) vrec->At(3726);
 //EdbTrackP* testtrack = (EdbTrackP*) testvertex->GetTrack(0);
 //accessing track 669, which vertices has?
 EdbTrackP* testtrack = emptymap[669];
 cout<<"VertexS has ID "<<testtrack->VertexS()->ID()<<endl;
 cout<<"VertexE has ID "<<testtrack->VertexE()->ID()<<endl;
 
 timewatch.Stop();
 cout<<"Finished reading "<<nvertices<<" vertices "<<endl;
 timewatch.Print();
 return 0;
}
