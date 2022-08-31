void couples_loop(){

 TH2D *htxty = new TH2D("htxty","2D angular distribution;TX;TY",300,-1.5,1.5,300,-1.5,1.5);
 TH1D *htx = new TH1D("htx","1D angular distribution;TX",300,-1.5,1.5);
 TH1D *hty = new TH1D("hty","1D angular distribution;TY",300,-1.5,1.5);

 TH1D *htheta = new TH1D("htheta","1D Theta distribution;#Theta[rad]",20,0.,0.2);

 TH2D *hxy = new TH2D("hxy","2D position distribution;x[#mum];y[#mum]",190,0,190,190,0,190);

 EdbCouplesTree *mytree = new EdbCouplesTree();
 mytree->InitCouplesTree("couples","/home/scanner/sndlhc/RUN0/b000031/p058/31.58.0.0.cp.root","READ");
 mytree->eCut = "eCHI2P<2.4&&s.eW>20&&eN1<=1&&eN2<=1&&s1.eFlag>=0&&s2.eFlag>=0"; //couples used in track reconstruction
 //how many entries above the cut?
 TEventList *lst = mytree->InitCutList();
 int nlst =lst->GetN();
 cout<<"We have "<<nlst<<" good couples"<<endl;

 int entr = 0;
 for(int i=0; i<nlst; i++ ) {
  entr = lst->GetEntry(i);
  mytree->GetEntry(entr);

  EdbSegP *myseg = mytree->eS;
  
  //filling histograms
  hxy->Fill(myseg->X()/1000., myseg->Y()/1000.);
  htxty->Fill(myseg->TX(),myseg->TY());
  htx->Fill(myseg->TX());
  hty->Fill(myseg->TY());
  htheta->Fill(TMath::ATan(myseg->Theta()));
 }
 htxty->Draw("COLZ");
 TCanvas *ctheta = new TCanvas("ctheta","theta angle of good segments");
 htheta->Draw();
 TCanvas *cxy = new TCanvas("cxy","xy position of good segments",800,800);
 hxy->Draw("COLZ");

 TCanvas *c1Dtxty = new TCanvas("c1Dtxty","angular 1D");
 c1Dtxty->Divide(1,2);
 c1Dtxty->cd(1);
 htx->Draw();
 c1Dtxty->cd(2);
 hty->Draw();
}
