/*script for display
*/


TString inputfile_vtxname = "vertextree.root"; // vertices_SB_0_177 vertices_R_0_51 vertices_SB_0_33
TString inputfile_trkname = "linked_tracks.root";

//trova vertici legati a un evento MC
void find_vtx_rel_event(int EVTID){
    
    //int EVTID=1;
    
    int vid = -1; int trmin=2; float amin=0.00;
    
    TFile *inputfile_trk = TFile::Open(inputfile_trkname,"READ");
    if (inputfile_trk == NULL) cout<<"ERROR: inputfile_trk not found"<<endl;
    TTree *tracktree = (TTree*) inputfile_trk->Get("tracks");
    
    EdbDataProc *dproc = new EdbDataProc();
    EdbPVRec *gAli = dproc->PVR();
    EdbScanCond *scancond = new EdbScanCond();
    gAli->SetScanCond(scancond);
    
    EdbVertexRec *gEVR = new EdbVertexRec();
    gEVR->SetPVRec(gAli);

    gEVR->eDZmax=3000.;
    gEVR->eProbMin=0.01;
    gEVR->eImpMax=15.;
    gEVR->eUseMom=false;
    gEVR->eUseSegPar=true;
    gEVR->eQualityMode=0;

 
    //arrays for vertices drawing
    TObjArray *varr = new TObjArray();
    TObjArray *tarr = new TObjArray();
    
    cout << "Using trk file: " << inputfile_trkname << endl;
    cout << "Using vertices file: " << inputfile_vtxname << endl;
    
    cout << "Drawing vertices and tracks related to MC event " << EVTID << endl;
    //*********************LOOP ON VERTICES****************************
    cout << endl << "--- Printing vertices --- " << endl;
        
    int ONLYONCE=0, YES=0;
    map<int,EdbTrackP*> trackID_map;

    int test = dproc->ReadVertexTree(*gEVR,inputfile_vtxname,"1",trackID_map);
    int nvertices = gEVR->eVTX->GetEntries();
    for (int ivtx = 0; ivtx < nvertices; ivtx++){ //
        ONLYONCE=0;
        YES=0;
        
        EdbVertex *v = (EdbVertex*) gEVR->eVTX->At(ivtx);
        int ntracks = v->N();
        
        //LOOP ON TRACKS
        for(int j=0; j<ntracks; j++){
            EdbTrackP *t1=0;
            //acceding to the track
            t1 = v->GetTrack(j);
            
            if(t1->MCEvt()==EVTID){
                
                if(ONLYONCE==0){
                    cout<< ivtx << "\tID: " << v->ID() << " Flag: " <<v->Flag()<< " ntracks: " << v->N() << " probability: " << v->V()->prob() << " maxaperture: "<< v->MaxAperture() << "\t" << v->VZ() << endl;
                    varr->Add(v); //adding vertex to array for drawing
                    ONLYONCE=1;
                    YES=1;
                }
            }
        } //end of tracks loop
        if(YES==1){
            for(int jj=0; jj<ntracks; jj++){
                EdbTrackP *tv=0;
                //acceding to the track
                tv = v->GetTrack(jj);
                
                tarr->Add(tv); //adding track to array for drawing
                cout << tv->Track() <<"\t\tTrack " << tv->MCTrack() << " evID " << tv->MCEvt() << " with " << tv->N() << " seg, Z " << tv->GetSegment(0)->Z() << " Pl " << tv->GetSegment(0)->Plate() << "-" << tv->GetSegmentLast()->Plate() << " incoming " << v->GetVTa(jj)->Zpos() << ", pdg " << tv->Aid(0) << ", Th " << tv->Theta() << ", Charge " << tv->GetSegment(0)->W()-70 << " IP " <<  v->GetVTa(jj)->Imp() << endl;
                //sqrt(tv->TX()*tv->TX()+tv->TY()*tv->TY())
            }
        } //end of second tracks loop
    }//end of vertex loop
    
    
    //*********************LOOP ON TRACKS****************************
    
    cout << "--- Printing tracks --- " << endl;
    
    tracktree->BuildIndex("trid"); //to read entry with given trackid
    //setting branches
    
    int nseg=0;
    EdbSegP *trk = 0;
    TClonesArray *segments = new TClonesArray("EdbSegP",100);
    TClonesArray *fittedsegments = new TClonesArray("EdbSegP",100);
    
    tracktree->SetBranchAddress("nseg",&nseg);
    tracktree->SetBranchAddress("t.",&trk);
    tracktree->SetBranchAddress("s",&segments);
    tracktree->SetBranchAddress("sf",&fittedsegments);
    
    gStyle->SetPalette(1);
    
    const float zmin = -100000.;
    const float zmax = 0.;
    
    //
    
    for(int j=0; j<tracktree->GetEntries(); j++){
        tracktree->GetEntry(j);        
        EdbSegP *seg0 = (EdbSegP*) segments->At(0);
        EdbSegP *seglast = (EdbSegP*) segments->At(nseg-1);
        if(trk->MCEvt()==EVTID){
            cout << j << "\t\tTrack " << trk->MCTrack() << "(" << seglast->MCTrack() << ") with " << nseg << " seg, evID " << trk->MCEvt() << "(" << seglast->MCEvt() << "), pdg " << trk->Aid(0) << ", Charge " << seg0->W()-70 << ", mum: " << trk->Aid(1) << ", Th " << trk->Theta() << ", Z " << seg0->Z() <<  ", Pl "; //start loop on segments associated to the track

            //temporary object for reading the file and building EdbTrackP
            EdbTrackP * temptrack = new EdbTrackP();
            // temptrack->Copy(EdbTrackP(trk));
            for (int i = 0; i< nseg; i++){
                temptrack->SetID(trk->ID());
                EdbSegP *seg = (EdbSegP*) segments->At(i);
                EdbSegP *segf = (EdbSegP*) fittedsegments->At(i);
                seg->SetDZ(300);
                segf->SetDZ(300);
                temptrack->AddSegment(new EdbSegP(*((EdbSegP*)(seg))));
                temptrack->AddSegmentF(new EdbSegP(*((EdbSegP*)(segf))));
                temptrack->SetSegmentsTrack(temptrack->ID()); //track segments association
                temptrack->SetCounters();
                if(i==0) cout << seg->Plate() << "-";
                else if(i==nseg-1) cout << seg->Plate() << endl;
            }
            tarr->Add(temptrack);
            //ds->TrackDraw(temptrack);
        }
        
    }
    
    const char *dsname="display-v";
    EdbDisplay   *ds=0;
    ds = EdbDisplay::EdbDisplayExist(dsname);
    //if(!ds)  ds=new EdbDisplay(dsname,-300000.,100000.,-300000.,100000.,-80000., 0.);
    if(!ds)  ds=new EdbDisplay(dsname,-1000000.,200000.,-300000.,100000.,-160000., 0.);
    ds->SetVerRec(gEVR);
    ds->SetArrTr( tarr );
    printf("%d tracks to display\n", tarr->GetEntries() );
    ds->SetArrV( varr );
    printf("%d vertex to display\n", varr->GetEntries() );
    ds->SetDrawTracks(4);
    ds->SetDrawVertex(1);
    ds->Draw();
    
}
