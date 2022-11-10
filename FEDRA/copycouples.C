//apply selection on couples file, saving it into new file. (BE CAREFUL NOT TO RECREATE OLD FILES)

void copycouples(){
 const int plateID = 54;
 //input file
 TFile *inputfile = TFile::Open(Form("/home/scanner/sndlhc/RUN0/b000031/p%03i/31.%i.0.0.cp.root",plateID,plateID));
 TTree *inputtree = (TTree*) inputfile->Get("couples");

 TCut selection("TMath::Abs(s.eX-96000)<20000 && TMath::Abs(s.eY-96000) < 20000");
 
 cout<<"copying "<<inputtree->GetEntries()<<" couples of plate "<<plateID<<" with selection "<<selection<<endl;
 //only selected central region

 TFile *outputfile = new TFile(Form("/home/scanner/sndlhc/testlocalcorrection_RUN0/b000031/p%03i/31.%i.0.0.cp.root",plateID,plateID),"NEW");
 TTree *selectedcouples = inputtree->CopyTree(selection);

 cout<<"copyied "<<selectedcouples->GetEntries()<<" couples"<<endl;

 outputfile->cd();
 selectedcouples->Write();
 outputfile->Close();


}
