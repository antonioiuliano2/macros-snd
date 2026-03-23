using namespace std;
using namespace TMath;

namespace VERTEX_PAR
{
  float DZmax = 3000.;
  //float ProbMinV   = 0.0001;  // minimum acceptable probability for chi2-distance between tracks
  float ProbMinV   = 0.001;
  float ImpMax     = 3.5;    // maximal acceptable impact parameter [microns] (for preliminary check)
  bool  UseMom     = false;  // use or not track momentum for vertex calculations
  bool  UseSegPar  = true;  // use only the nearest measured segments for vertex fit (as Neuchatel)
  int   QualityMode= 0;      // vertex quality estimation method (0:=Prob/(sigVX^2+sigVY^2); 1:= inverse average track-vertex distance)
}

// ----------------------------------------------------------------------
// Read a .dat file containing: cellx celly vID
// For each entry, open the corresponding vertex file and add the
// selected vertices to the provided EdbVertexRec.
// ----------------------------------------------------------------------
void LoadVerticesFromDat(const char *datfile,
                         const char *basepath,
                         int brickID,
                         TString vtxname,
                         EdbVertexRec &evr,
                         EdbDataProc *dproc)
{
  std::ifstream fin(datfile);
  if(!fin.is_open()) {
    printf("LoadVerticesFromDat: cannot open %s\n", datfile);
    return;
  }

  // group requested vertex IDs by (cellx, celly)
  std::map< std::pair<int,int>, std::vector<int> > requests;

  int cellx, celly, vID;
  while(fin >> cellx >> celly >> vID) {
    requests[std::make_pair(cellx,celly)].push_back(vID);
  }
  fin.close();

  // ------------------------------------------------------------------
  // ONE single DataProc / PVRec for all cells
  // ------------------------------------------------------------------
  EdbPVRec *gAli = dproc->PVR();

  EdbScanCond scancond;
  scancond.SetSigma0(1,1,0.0015,0.0015);
  scancond.SetDegrad(100);
  gAli->SetScanCond(&scancond);

  evr.SetPVRec(gAli);
  evr.eDZmax     = 3000;
  evr.eProbMin   = 0.001;
  evr.eImpMax    = 3.5;
  evr.eUseMom    = false;
  evr.eUseSegPar = true;
  evr.eQualityMode = 0;

  // ------------------------------------------------------------------
  // loop over requested cells
  // ------------------------------------------------------------------
  for(auto &entry : requests) {

    int cx = entry.first.first;
    int cy = entry.first.second;

    TString vtxFile;
    vtxFile.Form("%s/cell_%d_%d/b%06d/%s",
                 basepath, cx*10, cy*10, brickID, vtxname.Data()); // READING VTXFILES AFTER TRACKFINDTOOL

    printf("Opening %s\n", vtxFile.Data());

    // build selection string
    std::ostringstream cut;
    bool first = true;

    for (auto vid : entry.second) {
      if(!first) cut << " || ";
      cut << "vID==" << vid;
      first = false;
    }

    std::string cutString = cut.str();
    printf("Selection: %s\n", cutString.c_str());

    int nvtx = EdbDataProc::ReadVertexTree(evr,
                                    vtxFile.Data(),
                                    cutString.c_str());

    printf("Vertices added from this cell: %d\n", nvtx);
  }

  printf("Total vertices in PVRec: %d\n",
         gAli->eVTX ? gAli->eVTX->GetEntries() : 0);
}

void VTX_OldDisplay( const char *dsname,  EdbVertexRec *evr)
{
  TObjArray *varr = new TObjArray();
  TObjArray *tarr = new TObjArray();
  
  EdbVertex *v=0;
  EdbTrackP *t=0;
  
  int nv = evr->Nvtx();
  printf("nv=%d\n",nv);
  if(nv<1) return;
  
  for(int i=0; i<nv; i++) {
    v = (EdbVertex *)(evr->eVTX->At(i));
    varr->Add(v);
    //v->PrintGeom();
    for(int j=0; j<v->N(); j++) {
      EdbTrackP *t = v->GetTrack(j);
      tarr->Add( t );
    }
  }
  
  EdbPVRec *pvr = evr->ePVR;
  if(pvr) {
    int ntr = pvr->Ntracks();
    for(int i=0; i<ntr; i++) 
    {
      EdbTrackP *t = pvr->GetTrack(i);
      if(t->Flag()==999999) tarr->Add(t);
    }
  }
  
    
  gStyle->SetPalette(1);
  
  EdbDisplay *ds = EdbDisplay::EdbDisplayExist(dsname);
  if(!ds)  ds=new EdbDisplay(dsname,-10000.,10000.,-10000.,10000.,-10000., 10000.);
  ds->SetVerRec(evr);
  ds->SetArrTr( tarr );
  printf("%d tracks to display\n", tarr->GetEntries() );
  ds->SetArrV( varr );
  printf("%d vertex to display\n", varr->GetEntries() );
  //ds->SetDrawTracks(14);
  ds->SetDrawTracks(5);
  ds->SetDrawVertex(1);
  
  ds->GuessRange(2000,2000,30000);
  ds->SetStyle(1);
  ds->Draw();
  
}

// void draw_OLD_FEDRAdisp_candidates(TString datfile, TString basepath)
// {
//     // Initialize ROOT GUI application first
//     gSystem->Load("libEdd.so");

//     // Create DataProc and PVRec
//     EdbDataProc *dproc = new EdbDataProc();
//     EdbPVRec *gAli = dproc->PVR();

//     // Create vertex collection
//     EdbVertexRec evr;
//     evr.SetPVRec(gAli);

//     // Load vertices from .dat file
//     LoadVerticesFromDat(datfile, basepath, evr, dproc);

//     if (evr.Nvtx() == 0) {
//         printf("No vertices loaded. Exiting.\n");
//         return;
//     }

//     printf("NVTX = %d\n", evr.Nvtx());
//     printf("NTRK = %d\n", gAli->Ntracks());

//     // Display vertices
//     VTX_Display("DIS_candidates", &evr);

//     // Force GUI update before entering the interactive loop
//     gSystem->ProcessEvents();
// }

void drawEDAcandidates()
{
  TString datfile = "/eos/home-d/dannc/MuDISemu_analysis/SPECT_REGEN/muon_Euniform_RUN1/DATA_vtx_candidates.dat"; // path to .dat file with candidate list
  TString basepath = "/eos/experiment/sndlhc/emulsionData/emureco_Napoli/RUN1/b000121/cells/"; // base path to vertex files e.g. "/eos/experiment/sndlhc/emulsionData/emureco_Napoli/RUN1/b000121/cells/"
  int brickID = 21; // hardcoded for now
  TString vtxname = Form("b%06d.0.0.0.vtx.refit.root",brickID);
  using namespace VERTEX_PAR;
  EdbDataProc *dproc = new EdbDataProc();
  EdbPVRec *ali = dproc->PVR();

  // Create vertex collection
  EdbVertexRec evr;
  evr.SetPVRec(ali);

  // Load vertices from .dat file
  LoadVerticesFromDat(datfile, basepath, brickID, vtxname, evr, dproc);


  TObjArray *drawnvertices = new TObjArray(100);
  TObjArray *drawntracks = new TObjArray(100);


  EdbScanCond *scancond = new EdbScanCond();
  ali->SetScanCond(scancond);


  for (int i = 0; i <  evr.Nvtx(); i++){ //range for loop, C++11
  
    EdbVertex *vertex = 0;

    vertex = (EdbVertex*) evr.eVTX->At(i);

    vertex->SetXYZ(vertex->VX(),vertex->VY(),vertex->VZ()); //EDA USES X(), NOT VX(). Here I want the display to match my coordinates
    drawnvertices->Add(vertex); // assuming the array is filled with EdbVertex.
    for (int itrk = 0; itrk < vertex->N(); itrk++){
      EdbTrackP* track =  vertex->GetTrack(itrk);     
      for (int iseg = 0; iseg < track->N(); iseg++) track->GetSegment(iseg)->SetFlag(kRed); // to color them differently
      //    for (int iseg = 0; iseg < track->N(); iseg++) track->GetSegment(iseg)->SetFlag(vertexcolors[i]); // to color them differently
      drawntracks->Add(track);
    }
  }

  ali->eTracks = drawntracks;
  ali->eVTX = drawnvertices;
  EdbEDA * eda = new EdbEDA(ali); // init DataSet but doesn't read linked_track.root
  eda->GetTrackSet("TS")->SetColorMode(kCOLOR_BY_ID);
  /* eda->GetTrackSet("TS")->RemoveTrack(specialtrack); //charm colored differently
  eda->GetTrackSet("SB")->SetTrackAttribute(4);
  eda->GetTrackSet("SB")->AddTrack(specialtrack);*/
  eda->Run();
}
