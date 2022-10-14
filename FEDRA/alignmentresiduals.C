//REDRAWING ALIGNMENT RESIDUALS (RE-CREATED 14 October 2022)

void alignmentresiduals(){
 //accessing file
 EdbCouplesTree *aligncouples = new EdbCouplesTree();
 aligncouples->InitCouplesTree("couples","/home/scanner/sndlhc/RUN1/b000024/AFF/24.307.0.0_24.306.0.0.al.root","READ");

 TFile *inputfile = TFile::Open("/home/scanner/sndlhc/RUN1/b000024/AFF/24.307.0.0_24.306.0.0.al.root","READ");
 EdbLayer *la1 = (EdbLayer*) inputfile->Get("corr_layer1"); //corrlayer2 is identical, but let us put it anyway
 EdbLayer *la2 = (EdbLayer*) inputfile->Get("corr_layer2");
 TH2D *hdxy = new TH2D("hdxy","Position residuals;x[#mum];y[#mum]",120,-120,120,120,-120,120);
 TH2D *hdxy_original = new TH2D("hdxy_original","Position residuals;x[#mum];y[#mum]",9,-120,120,9,-120,120);
 
 //looping over couples
 int ncouples = aligncouples->eTree->GetEntries();
 for (int i= 0; i < ncouples; i++){
  aligncouples->GetEntry(i);
  //applying actual transformations as in reports
  EdbSegP *s1 = aligncouples->eS1; 
  EdbSegP *s2 = aligncouples->eS2; 
  float dx = la2->X(*s2)-la1->X(*s1); //X() and Y() functions apply both propagation and affine transformations
  float dy = la2->Y(*s2)-la1->Y(*s1);

  hdxy->Fill(dx,dy);
  hdxy_original->Fill(dx,dy);
 } 
 //drawing histogram
 TCanvas *cxy = new TCanvas("cxy","xy residual",800,800);
 hdxy->Draw("COLZ");

 TCanvas *cxy1D = new TCanvas("cxy1D","1D distributions");
 cxy1D->Divide(1,2);
 cxy1D->cd(1);
 TH1D *hdx = hdxy->ProjectionX();
 hdx->Draw();
 cxy1D->cd(2);
 TH1D *hdy = hdxy->ProjectionY();
 hdy->Draw();

 TCanvas *cxy_original = new TCanvas("cxy_original","xy residual",800,800);
 hdxy_original->SetMinimum(10290); //values from the actual plot
 hdxy_original->SetMaximum(30583);
 hdxy_original->Draw("COLZ");
}
