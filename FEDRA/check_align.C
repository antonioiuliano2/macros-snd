/*
Checking alignment performances. 

*/
void align_check(int brick, int lastplate=57, int firstplate=1);
//checks dz between plates after alignment
void checkdz(int brick, int lastplate=57, int firstplate=1){
 //syntax for brickname is 002, 022, 111
 TString dir = TString("/home/scanner/sndlhc/TESTTUNGSTENO2021");

 TString setposition = TString(Form("/b%06i/b%06i.0.0.0.set.root",brick,brick)); //example: /b000002/b000002.0.0.0.set.root

 TH1F *hdzA = new TH1F("hdzA",Form("Dz obtained after alignment for brick %i",brick),300,0,3000);

 TGraph *hgraphA = new TGraph();
 hgraphA->SetName("hgraph");

 TFile *fileA = TFile::Open((dir+setposition).Data());
 EdbScanSet *setA = (EdbScanSet*) fileA->Get("set");

 float dzA;
 //loop on plates
 for (int i = firstplate; i< lastplate; i++){
  dzA = setA->GetDZP2P(i,i+1);
  cout<<"from plate "<<i<<" we have dz "<<dzA<<endl;
  //filling histograms and graphs
  hdzA->Fill(dzA);
  
  hgraphA->SetPoint(i,(i+i+1)/2.,dzA);
 }
 //comparing the histograms
 TCanvas *cdz = new TCanvas("cdz");
 cdz->Divide(1,2);
 cdz->cd(1);
 hdzA->Draw();
 hdzA->GetXaxis()->SetTitle("dZ[#mum]");
 cdz->cd(2);
 hgraphA->SetTitle(Form("Dz obtained after alignment for brick %i",brick));
 hgraphA->Draw("AP*");
 hgraphA->GetXaxis()->SetTitle("nplate");
 hgraphA->GetYaxis()->SetTitle("dZ[#mum]");

}

//check alignment plots (currently xy and zphi)

void align_check(int brick,int lastplate, int firstplate){
 TString path = TString("/home/scanner/sndlhc/TESTTUNGSTENO2021");
 TFile *inputfile;
 //FOOT bricks are made of many plates, needed more than 1 canvas
 const int nplates = lastplate - firstplate;
 const int canvasdimx = 6;
 const int canvasdimy = 6;
 const int canvasarea = canvasdimx * canvasdimy;
 //how many canvases do I need?
 const int ncanvases=nplates/canvasarea+1; 
 TCanvas *c[ncanvases];
 TCanvas *cxy[ncanvases];

 for (int icanvas = 0; icanvas < ncanvases; icanvas++){
 c[icanvas] = new TCanvas((TString::Format("%i_dz_phi_coarse_%d",brick,icanvas)).Data());
 c[icanvas]->SetTitle((TString::Format("%i_dz_phi_coarse_%d",brick,icanvas)).Data());
 c[icanvas]->Divide(canvasdimx,canvasdimy); //NEED TO OPEN OTHER CANVASES WHEN PLATES INCREASE

 cxy[icanvas] = new TCanvas((TString::Format("%i_dxy_final_%d",brick,icanvas)).Data());
 cxy[icanvas]->SetTitle((TString::Format("%i_dxy_final_%d",brick,icanvas)).Data());
 cxy[icanvas]->Divide(canvasdimx,canvasdimy);
 }

 TH2F *hzphi[nplates];
 TH2F *hxy[nplates];

 //const int lastplate= 57;
 int iplate = 0;
 int whichcanvas = 0;
 for (int i = firstplate; i <= lastplate; i++){
  if (i == firstplate) continue;
  //computing in which canvas and subcanvas plot the histograms
  whichcanvas = iplate/canvasarea;
  c[whichcanvas]->cd(iplate-whichcanvas*canvasarea+1);
  //opening the file with the report
  inputfile = TFile::Open((path+TString(Form("/b%06i/AFF/b%06i.%i.0.0_b%06i.%i.0.0.al.root",brick,brick,i,brick,i-1))).Data()); //example:AFF/2.10.0.0_2.9.0.0.al.root
  if (inputfile){ //check if file is present
   if (inputfile->GetListOfKeys()->Contains("zphi_coarse") ){ //check if histogram is present (i.e. alignment was not interrupted leading to a zombie-like file, even if not seen as zombie)
   hzphi[iplate] = (TH2F*)inputfile->Get("zphi_coarse");
   hzphi[iplate]->SetTitle(Form("plate %i to plate %i",i,i-1));
   hzphi[iplate]->Draw("COLZ");   
  }

  cxy[whichcanvas]->cd(iplate-whichcanvas*canvasarea+1);
  if (inputfile->GetListOfKeys()->Contains("xy_final") ){
  hxy[iplate] = (TH2F*)inputfile->Get("xy_final");
  hxy[iplate]->SetTitle(Form("plate %i to plate %i",i,i-1));
  hxy[iplate]->Draw("COLZ");
   }
  }
  iplate++;
 }
}

int check_align(int bricknumber = 1,int lastplate=60, int firstplate=1){
 align_check(bricknumber,lastplate,firstplate);
 checkdz(bricknumber,lastplate,firstplate);
 return 0;
}
