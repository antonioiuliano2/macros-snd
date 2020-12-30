void myGenieDisplay(int ievent){

    TDatabasePDG *pdg = TDatabasePDG::Instance();

    TEveManager::Create();
    ROOT::RVec <int> charmpdgs = {421,411,4122,431, 4132, 4232, 4332, 441};
  
    //momentum display (momenta vectors, start from 0 and go to (px,py,pz))

    //TFile *inputfile = TFile::Open("/home/utente/Simulations/sim_snd/GenieEvents_SNDAcceptance/CCDIS/nutau/nutau_CCDIS_FairShip.root");
    TFile *inputfile = TFile::Open("/home/utente/Simulations/sim_snd/GenieEvents_SNDAcceptance/CharmCCDIS/numu/numu_CharmCCDIS_FairShip.root");
    TTree *gst = (TTree*) inputfile->Get("gst");

    int neu;
    double pxl,pyl,pzl; //primary lepton momentum
    const int maxnf = 1000;
    int nf;
    double pxf[maxnf], pyf[maxnf], pzf[maxnf]; //final state momentum
    int pdgf[maxnf];

    gst->SetBranchAddress("neu",&neu);
    gst->SetBranchAddress("pxl",&pxl);
    gst->SetBranchAddress("pyl",&pyl);
    gst->SetBranchAddress("pzl",&pzl); 
 
    gst->SetBranchAddress("nf",&nf);
    gst->SetBranchAddress("pxf",&pxf);
    gst->SetBranchAddress("pyf",&pyf);
    gst->SetBranchAddress("pzf",&pzf); 
    gst->SetBranchAddress("pdgf",&pdgf); 
       
    auto ls_lepton = new TEveStraightLineSet();
    auto ls_hadronic = new TEveStraightLineSet();
    auto ls_charm = new TEveStraightLineSet();
    ls_charm->SetLineColor(kWhite);
    gst->GetEntry(ievent);

    ls_lepton->SetLineColor(kYellow);
    ls_lepton->AddLine(0.,0.,0.,pxl,pyl,pzl);

    TEveText *tl =new TEveText(pdg->GetParticle(neu-1)->GetName());
    tl->SetFontSize(10);
    tl->RefMainTrans().SetPos(pxl/4.,pyl/4.,0.);

    cout<<"how many particles in final state "<<nf<<endl;
    for (int i=0; i < nf; i++){
        int ischarm = charmpdgs[charmpdgs==pdgf[i]].size(); 
        if(ischarm) ls_charm->AddLine(0.,0.,0.,pxf[i],pyf[i],pzf[i]);
        else ls_hadronic->AddLine(0.,0.,0.,pxf[i],pyf[i],pzf[i]);

        TEveText *tf =new TEveText(pdg->GetParticle(pdgf[i])->GetName());
        tf->SetFontSize(10);
        tf->RefMainTrans().SetPos(pxf[i]/2.,pyf[i]/2.,0.);
        gEve->AddElement(tf);
    }

    //do the drawing
    gEve->AddElement(ls_charm);
    gEve->AddElement(ls_lepton);
    gEve->AddElement(ls_hadronic);  
    gEve->Redraw3D();

    gEve->AddElement(tl);
    TGLViewer * v = gEve->GetDefaultGLViewer();
    //cout<<v<<endl;
    v->SetCurrentCamera(TGLViewer::kCameraOrthoXOY );
          
}