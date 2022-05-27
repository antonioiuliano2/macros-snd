//Drawing tracks simple distribution with RDataFrame (26 May)

void CopyNoCOV(EdbSegP&tr, EdbSegP*firstseg){
  //covariance matrix does get problem in destruction, for now we need to exclude it
  tr.SetPID(firstseg->PID());
  tr.Set(firstseg->ID(),firstseg->X(),firstseg->Y(),firstseg->TX(),firstseg->TY(),firstseg->W(),firstseg->Flag());
  tr.SetZ(firstseg->Z());
  //if(firstseg->CheckCOV()) tr.SetCOV(firstseg->COV());
  tr.SetSZ(firstseg->SZ());
  tr.SetVid(firstseg->Vid(0),firstseg->Vid(1));
  tr.SetAid(firstseg->Aid(0),firstseg->Aid(1),firstseg->Side());
  tr.SetProb(firstseg->Prob());
  tr.SetVolume(firstseg->Volume());
  tr.SetDZ(firstseg->DZ());
  tr.SetDZem(firstseg->DZem());
  tr.SetP(firstseg->P());
  tr.SetChi2(firstseg->Chi2());
  tr.SetTrack(firstseg->Track());
  tr.SetMC( firstseg->MCEvt(), firstseg->MCTrack() );
  tr.SetScanID(firstseg->ScanID());

}

double GetVX(TClonesArray sf){
  //Since covariance matrix cannot be copied in EdbSegP branch, leads to segfault at the end, access attributes like this
  EdbSegP* firstseg = (EdbSegP*) sf.At(0);
  return firstseg->COV()(0,0);
}

EdbSegP GetFirstSegment(TClonesArray sf){
  EdbSegP* firstseg = (EdbSegP*) sf.At(0);
  EdbSegP tr;
  //EdbSegP tr(*firstseg);

  
  CopyNoCOV(tr,firstseg);
  
  return tr;
}



using namespace ROOT;
void rdataframe_anglestracks(){
  TFile *tracksfile = TFile::Open("b000003.0.0.0.trk.root");
  TTree *trackstree = (TTree*) tracksfile->Get("tracks");

  //position map binning
  const int nbinsx = 10;
  const float xmin = 0;
  const float xmax = 10;
  const int nbinsy = 10;
  const float ymin = 0;
  const float ymax = 10;

  //angle bins
  const int nbinstx = 75;
  const float txmin = -1.5;
  const float txmax = 1.5;
  const int nbinsty = 75;
  const float tymin = -1.5;
  const float tymax  = 1.5; 

  RDataFrame df(*trackstree);

  //nseg before cut, to have all track segments
  auto hnseg = df.Fill<int>(TH1I("hnseg", "number of segments;nseg", 5, 0, 5), {"nseg"}); //integer filling is more convoluted

  auto dftr0 = df.Define("varx",GetVX,{"sf"}); //cov matrix need to be stored in another branch

  auto dftr = dftr0.Define("tr",GetFirstSegment,{"sf"});
  //definition of variables to fill histogram with (only difference, coordinates in cm instead of micron);
  auto dftr1 = dftr.Define("TX","tr.TX()  ")
                   .Define("TY","tr.TY()  ")
                   .Define("Xcm","tr.X()*1e-4  ")
                   .Define("Ycm","tr.Y()*1e-4  ");

  //3D angles
  auto dftr2 = dftr1.Define("tantheta","tr.Theta()").Define("theta","TMath::ATan(tantheta)");

  //selecting good tracks (aka long, in this case
  auto dfgoodtr = dftr2.Filter("nseg>2");

  auto hxy = dfgoodtr.Histo2D({"hxy","xy map;x[cm];y[cm];TX",nbinsx,xmin,xmax,nbinsy,ymin,ymax},"Xcm","Ycm");

  auto htx = dfgoodtr.Histo1D({"htx","TX of tracks;TX", nbinstx,txmin,txmax},"TX");
  auto htxmap = dfgoodtr.Profile2D({"htxmap","TX of tracks in xy map;x[cm];y[cm];TX",nbinsx,xmin,xmax,nbinsy,ymin,ymax, txmin,txmax},"Xcm","Ycm","TX");

  auto hty = dfgoodtr.Histo1D({"hty","TY of tracks;TY", nbinsty,tymin,tymax},"TY");
  auto htymap = dfgoodtr.Profile2D({"htymap","TY of tracks in xy map;x[cm];y[cm];TY",nbinsx,xmin,xmax,nbinsy,ymin,ymax, tymin,tymax},"Xcm","Ycm","TY");

  auto htxty = dfgoodtr.Histo2D({"htxty","TY and TX of tracks;TX;TY",nbinstx,txmin,txmax, nbinsty,tymin,tymax},"TX","TY");

  auto hsintheta = dfgoodtr.Define("sintheta","TMath::Sin(theta)")
                           .Histo1D({"hsintheta","SinTheta distribution;sin#theta",110,0,1.1},"sintheta");

  auto hcostheta = dfgoodtr.Define("costheta","TMath::Cos(theta)")
                           .Histo1D({"hcostheta","CosTheta distribution;cos#theta",110,0,1.1},"costheta");

  //just for testing COV
  auto hvarx = dfgoodtr.Histo1D("varx");

  //Drawing plots
  TCanvas *ctx = new TCanvas("ctx","TX Canvas",1600,800);
  ctx->Divide(2,1);
  ctx->cd(1);
  htx->DrawClone();
  ctx->cd(2);
  htxmap->DrawClone("COLZ");

  //Drawing plots
  TCanvas *cty = new TCanvas("cty","TY Canvas",1600,800);
  cty->Divide(2,1);
  cty->cd(1);
  hty->DrawClone();
  cty->cd(2);
  htymap->DrawClone("COLZ");

  TCanvas *ctxty = new TCanvas("ctxty","TXTY Canvas",800,800);
  htxty->DrawClone("COLZ");

  TCanvas *cxy = new TCanvas("cxy","XY Canvas",800,800);
  hxy->DrawClone("COLZ && TEXT");

  TCanvas *csincostheta = new TCanvas("csincostheta","Sin and Cos Theta");
  csincostheta->Divide(2,1);
  csincostheta->cd(1);
  hsintheta->DrawClone();
  csincostheta->cd(2);
  hcostheta->DrawClone();

  TCanvas *cnseg = new TCanvas();
  hnseg->DrawClone();

}
