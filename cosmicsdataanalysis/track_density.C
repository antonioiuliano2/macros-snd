//track density, at least 4 segments. For muon cosmic study
void track_density(){
 const TCut trcut("nseg>3");
 //const TCut trcut("nseg>6");

 //TFile *trackfile = TFile::Open("b000001.0.0.0_last10plates.trk.root");
 TFile *trackfile = TFile::Open("b000004.0.0.0_firstfiveplates.trk.root");
 TTree *tracks = (TTree*) trackfile->Get("tracks");

 TCanvas *c = new TCanvas("cxy","XY Canvas distribution",800,800);

 //gStyle->SetOptStat(0);
 tracks->Draw("s[0].eY*1e-4:s[0].eX*1e-4>>hxy_tracks(19,0,19,19,0,19)",trcut);
 TH2D *hxy_tracks = (TH2D*) gDirectory->FindObject("hxy_tracks");
 hxy_tracks->SetTitle("Tracks xy distribution;x[cm];y[cm]");

 hxy_tracks->Draw("COLZ && TEXT");

 const int minbin = 3;
 const int maxbin = 16;
 ROOT::RVec<int> nmuons_array;

 for (int ibinx = minbin+1; ibinx <=maxbin; ibinx++){ //bin 0 is actually underflow!
  for (int ibiny = minbin+1; ibiny <=maxbin; ibiny++){
   int nmuons = hxy_tracks->GetBinContent(ibinx,ibiny);
    nmuons_array.push_back(nmuons);
    cout<<nmuons<<endl;   
  }
 }
 cout<<"N values "<<nmuons_array.size()<<endl;
 cout<<"Valor Medio: "<<ROOT::VecOps::Mean(nmuons_array)<<" con errore "<<ROOT::VecOps::StdDev(nmuons_array)/TMath::Sqrt(nmuons_array.size())<<endl;

 TLine *lx0 = new TLine(minbin,minbin,minbin,maxbin);
 lx0->SetLineColor(kRed);
 lx0->SetLineWidth(3);

 TLine *lx1 = new TLine(maxbin,minbin,maxbin,maxbin);
 lx1->SetLineColor(kRed);
 lx1->SetLineWidth(3);
 
 TLine *ly0 = new TLine(minbin,minbin,maxbin,minbin);
 ly0->SetLineColor(kRed);
 ly0->SetLineWidth(3);

 TLine *ly1 = new TLine(minbin,maxbin,maxbin,maxbin);
 ly1->SetLineColor(kRed);
 ly1->SetLineWidth(3);

 lx0->Draw("SAME");
 lx1->Draw("SAME");
 ly0->Draw("SAME");
 ly1->Draw("SAME");


 TCanvas *ctheta = new TCanvas();

 tracks->Draw("TMath::ATan(t.Theta())>>htheta(24,0,1.2)");

 TCanvas *ctxty = new TCanvas("ctxty","TX TY Canvas",800,800);
 tracks->Draw("t.eTY:t.eTX>>htxty(20,-1,1,20,-1,1)",trcut,"COLZ");
 TH2D *htxty = (TH2D*) gDirectory->FindObject("htxty");
 htxty->SetTitle("Angular distribution; TX; TY");

 TCanvas *c_costheta = new TCanvas();
 tracks->Draw("TMath::Cos(TMath::ATan(t.Theta()))>>hcostheta(10,0.5,1.)",trcut,"E");

 TH2D *hcostheta = (TH2D*) gDirectory->FindObject("hcostheta");
 hcostheta->SetTitle("Angular distribution; cos#theta");

 TF1 *fang_simple = new TF1("fang_simple","pol2");
 fang_simple->SetLineColor(kBlue);
 hcostheta->Fit(fang_simple,"R");

 TF1 *fang = new TF1("fang","pol2 * 1/TMath::Sqrt(1-x*x)");
 fang->SetParameters(100000,0,100000);
 //hcostheta->Fit(fang,"R+");

 //c_costheta->BuildLegend();
 
}
