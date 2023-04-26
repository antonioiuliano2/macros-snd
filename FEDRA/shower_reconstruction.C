/*void print_help_message()
{
    cout<< "\nUsage: \n\t  emshowerrec -set=ID [ -o=DATA_DIRECTORY -v=DEBUG] \n\n";
    cout<< "\t\t  -set=ID   - id of the dataset formed as BRICK.PLATE.MAJOR.MINOR \n";
    cout<< "\t\t  DEBUG     - verbosity level: 0-print nothing, 1-errors only, 2-normal, 3-print all messages\n";
    cout<< "\t\t  -vtr    - start showering from linked tracks attached to vertices with IP<250 (not yet included)\n";
    cout<< "\t\t  -ltr    - start showering from linked tracks (all; standard)\n";
    cout<< "\t\t  -btr    - start showering from basetracks (all; useful if no linked tracks done yet)\n";
    cout<< "\t\t  -otr    - only tracking, no showering\n";
    cout<< "\nExample: \n";
    cout<< "\t  emshow -id=4554.10.1.0 -o/scratch/BRICKS -lt\n";
    cout<< "\n  The data location directory if not explicitly defined will be taken from .rootrc as: \n";
    cout<< "\t  emrec.outdir:      /scratch/BRICKS \n";
    cout<< "\t  emrec.EdbDebugLevel:      1\n";
    cout<<endl;
}*/

void set_default(TEnv &cenv)
{
    // default parameters for shower reconstruction
    // determined by experimental and simulation studies
    cenv.SetValue("showerrec.nbrick",1);
    cenv.SetValue("showerrec.cpcut","s.eW>13&&eCHI2P<2.5&&s1.eFlag>=0&&s2.eFlag>=0&&eN1==1&&eN2==1");
    cenv.SetValue("showerrec.trkcut","nseg>1");
    cenv.SetValue("showerrec.firstsegment",0);
    cenv.SetValue("showerrec.ConeRadius", 800);
    cenv.SetValue("showerrec.ConeAngle", 0.1);
    cenv.SetValue("showerrec.ConnectionDR", 150);
    cenv.SetValue("showerrec.ConnectionDT", 0.15);
    cenv.SetValue("showerrec.NPropagation", 3);
    cenv.SetValue("showerrec.outdir", "..");
    cenv.SetValue("showerrec.env", "showerrec.rootrc");
    cenv.SetValue("showerrec.EdbDebugLevel", 1);
}

void shower_reconstruction()
{
/*    if (argc < 2)   {
        print_help_message();
        return 0;
}*/
    
    TEnv cenv("showerrecenv");
    set_default(cenv);
    cenv.ReadFile( cenv.GetValue("showerrec.env", "showerrec.rootrc"),kEnvLocal);
    gEDBDEBUGLEVEL        = cenv.GetValue("showerrec.EdbDebugLevel", 1);
    const char *env       = cenv.GetValue("showerrec.env", "showerrec.rootrc");
    const char *outdir    = cenv.GetValue("showerrec.outdir", "..");

    int brick = cenv.GetValue("showerrec.nbrick", 1);
    int nplate = 0;
    int major = cenv.GetValue("showerrec.major", 0);
    int minor = cenv.GetValue("showerrec.minor", 0);

    cout<<"test "<<brick<<endl;
    cenv.SetValue("showerrec.env", env);        
    
    cenv.SetValue("showerrec.outdir", outdir);

    cenv.WriteFile("showerrec.save.rootrc");

    
    EdbScanProc sproc;
    sproc.eProcDirClient=outdir;
    printf("\n----------------------------------------------------------------------------\n");
    printf("tracking set %d.%d.%d.%d\n", brick,nplate, major,minor);
    printf("----------------------------------------------------------------------------\n\n");

    EdbID id(brick,nplate,major,minor);
    EdbScanSet *ss = sproc.ReadScanSet(id);
    ss->Brick().SetID(brick);
  
    EdbDataProc *dproc = new EdbDataProc();
    EdbPVRec * eEdbPVRec = new EdbPVRec();
    EdbScanCond *scancond = new EdbScanCond();
    eEdbPVRec->SetScanCond(scancond);
 
    TCut c = cenv.GetValue("showerrec.cpcut","1");
    TCut trackcut = cenv.GetValue("showerrec.trkcut","1");
    bool do_showerfrom_bt = false; //use base tracks for shower, or volume tracks
    if (do_showerfrom_bt){
                //loop on plates
          int npl = ss->eIDS.GetEntries();

          for(int i=0; i<npl; i++) {
           EdbID *idplate = ss->GetID(i);
      
           EdbPlateP *plate = ss->GetPlate(idplate->ePlate);
           //read pattern information
           EdbPattern *p = new EdbPattern();
           sproc.ReadPatCPnopar(*p,*idplate, c);
           p->SetZ(plate->Z());
           p->SetSegmentsZ();
           p->SetID(i);
           p->SetPID(i);
           p->SetSegmentsPID();
          //plate->Print();
           p->Transform(    plate->GetAffineXY()   );
           p->TransformShr( plate->Shr() );
           p->TransformA(   plate->GetAffineTXTY() );
           p->SetSegmentsPlate(idplate->ePlate);
           eEdbPVRec->AddPattern(p);
          } //end of loop on patt
    }
    else{
          dproc->ReadTracksTree(*eEdbPVRec, "linked_tracks.root","nseg>2");
          int npl = eEdbPVRec->Npatterns();
 	  for(int i=0; i<npl; i++){
		 EdbPattern *pat = eEdbPVRec->GetPattern(i);
		 pat->SetPID(pat->GetSegment(0)->PID());
	  }
    }
          //get tracks tree and apply selections
          TFile *inputfile = TFile::Open("linked_tracks.root");
          TTree *trackstree = (TTree*) inputfile->Get("tracks");

          TFile *outputfile = new TFile("selected_linkedtracks.root","RECREATE");
          TTree *selectedtrackstree = trackstree->CopyTree(trackcut);
          selectedtrackstree->Write();
          std::cout<<"Selected "<<selectedtrackstree->GetEntries()<<" tracks with cut "<<trackcut<<std::endl;

          float ConeRadius = cenv.GetValue("showerrec.ConeRadius", 800);
          float ConeAngle = cenv.GetValue("showerrec.ConeAngle", 0.1);
          float ConnectionDR = cenv.GetValue("showerrec.ConnectionDR", 150);
          float ConnectionDT = cenv.GetValue("showerrec.ConnectionDT", 0.15);
          float NPropagation = cenv.GetValue("showerrec.NPropagation", 3);

          EdbShowerRec *eShowerRec = new EdbShowerRec();
          //Setting parameters
          eShowerRec->SetAlgoParameter(ConeRadius,0);
          eShowerRec->SetAlgoParameter(ConeAngle,1);
          eShowerRec->SetAlgoParameter(ConnectionDR,2);
          eShowerRec->SetAlgoParameter(ConnectionDT,3);
          eShowerRec->SetAlgoParameter(NPropagation,4);


               // Print parameters
          eShowerRec->PrintParameters();
    
         // Create Initiator BT array:
          TObjArray * eInBTArray=new TObjArray();

    // Reset eShowerRec Arrays: InBTArray and RecoShowerArray....
          eShowerRec->ResetInBTArray();
          eShowerRec->ResetRecoShowerArray();

          TClonesArray *seg  = new TClonesArray("EdbSegP", 60);
          int nseg;       
          
          selectedtrackstree->SetBranchAddress("s",  &seg);
          selectedtrackstree->SetBranchAddress("nseg",  &nseg);       

          const int ntracks = selectedtrackstree->GetEntries();
          int firstsegment = cenv.GetValue("showerrec.firstsegment",0); //take first segment of track
          
          int whichsegment;
          for (int itrk = 0; itrk < ntracks; itrk++){
           selectedtrackstree->GetEntry(itrk);
           if (firstsegment == 0) whichsegment = nseg - 1;
           else whichsegment = 0;       
           EdbSegP *segtest = new EdbSegP(*((EdbSegP*) seg->At(whichsegment)));           
           //set array with inBT (array of initiator base tracks)
           eInBTArray->Add(segtest);                  
          }
          eShowerRec->SetInBTArray(eInBTArray);
          eShowerRec->PrintInitiatorBTs();

          //set edbpvrec
          eShowerRec->SetEdbPVRec(eEdbPVRec);

          cout << " eShowerRec->SetUseAliSub(0)..." << endl;
          eShowerRec->SetUseAliSub(0);

          cout << " eShowerRec->Execute()..." << endl;

          //Start actual reconstruction
          eShowerRec->Execute();

          //Print output
          eShowerRec->PrintRecoShowerArray();
          outputfile->Close();         
}

void drawshower(int whichshower = -1, int nplates = 29, bool isEDA = true){
    //drawing shower with entry ishower. If not provided, draw all showers according to selection
    TCut cutentries = "sizeb>60";
    TFile *showerfile = TFile::Open("shower1.root");
    TTree *showertree = (TTree*) showerfile->Get("treebranch");

    int sizeb; 
    const int maxsize = 10000; //as in ShowerRec
    int idb[maxsize]; //IDs of basetracks
    int plateb[maxsize], ntrace1simub[maxsize]; //number of plate of base track
    float xb[maxsize],yb[maxsize],zb[maxsize],txb[maxsize],tyb[maxsize];
    //setting branch addresses
    showertree->SetBranchAddress("sizeb",&sizeb);
    showertree->SetBranchAddress("idb",&idb);
    showertree->SetBranchAddress("plateb",&plateb);
    showertree->SetBranchAddress("xb",&xb);
    showertree->SetBranchAddress("yb",&yb);
    showertree->SetBranchAddress("zb",&zb);
    showertree->SetBranchAddress("txb",&txb);
    showertree->SetBranchAddress("tyb",&tyb);
    showertree->SetBranchAddress("ntrace1simub",&ntrace1simub);

    map<int, EdbTrackP*> trackmap;

    TObjArray *sarr = new TObjArray();
    TObjArray *tarr = new TObjArray();

    showertree->Draw(">>lst", cutentries );
    TEventList *lst = (TEventList*)gDirectory->GetList()->FindObject("lst");
    int ngoodshowers =lst->GetN();

    int ishower = whichshower;
    if (whichshower >= 0) ngoodshowers = 1;
    //start loop over all good showers, filling array with segments
    cout<<"Drawing shower nr "<<whichshower<<"nplates set to "<<nplates<<endl;
    cout<<"Looping over "<<ngoodshowers<<" showers "<<endl;
    for (int ientry = 0; ientry < ngoodshowers; ientry++){
     if(whichshower < 0) ishower = lst->GetEntry(ientry);
     showertree->GetEntry(ishower);
     for (int iseg = 0; iseg < sizeb; iseg++){

        EdbSegP *newseg = new EdbSegP(idb[iseg], xb[iseg],yb[iseg], txb[iseg], tyb[iseg]);
        newseg->SetZ(zb[iseg]);
        newseg->SetDZ(300.);
        newseg->SetPID(plateb[iseg]);
        newseg->SetPlate(nplates - plateb[iseg]);
	int trackID = ntrace1simub[iseg];
        newseg->SetTrack(trackID);
        sarr->Add(newseg);

        //need a virtual 1-seg track for EDA, which draws only tracks
        if (!trackmap[trackID]){ 
		trackmap[trackID] = new EdbTrackP();
		trackmap[trackID]->SetID(trackID);
	        trackmap[trackID]->SetPID(newseg->PID());
		tarr->Add(trackmap[trackID]);
	}
        trackmap[trackID]->AddSegment(newseg);

     }
    }

    if (isEDA){
        EdbPVRec *ali = new EdbPVRec();
        ali->eTracks = tarr;
        EdbEDA * eda = new EdbEDA(ali);
        eda->GetTrackSet("TS")->SetColorMode(kCOLOR_BY_ID);
        eda->Run();
    }
    else{
     //DISPLAY OF SEGMENTS
     const char *dsname = "Test shower reconstruction";
     EdbDisplay * ds = new EdbDisplay(dsname,-100000.,100000.,-100000.,100000.,-40000., 0.);
     //ds->SetVerRec(gEVR);
     ds->SetDrawTracks(4);
     ds->SetArrSegP( sarr );
     ds->Draw();
    }


}
