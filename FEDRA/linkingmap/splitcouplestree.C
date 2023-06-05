//split couplestrees. Opposite of merge_trees. Instead of merging many couples into one file, it splits one file in multiple couples
//created 5 June 2023 by A.Iuliano
void splitcouplestree(int brickID, int plateID){
 //histo file
  
 //histograms to be stored
 TTimeStamp *tstamp = new TTimeStamp();

 const int platenumber = 2;
 //same parameters used as input for linking map!
 const int nx = 19;
 const int ny = 19;
 const float xmin = 0.;
 const float xmax = 190000.;
 const float ymin = 0.;
 const float ymax = 190000.;
 EdbCell2 *emulsioncell = new EdbCell2();
 emulsioncell->InitCell(nx,xmin,xmax,ny,ymin,ymax,1);

 EdbCouplesTree *mytree = new EdbCouplesTree();
 mytree->InitCouplesTree("couples",Form("p%0*i/%i.%i.0.0.cp.root",3,plateID,brickID,plateID),"READ");
 mytree->eCut = "eCHI2P<2.4&&s.eW>20&&eN1<=1&&eN2<=1&&s1.eFlag>=0&&s2.eFlag>=0"; //selecting couples for tracking

 TEventList *lst = mytree->InitCutList();
 int nlst =lst->GetN();
 cout<<"We have "<<nlst<<" good couples"<<endl;
 EdbCouplesTree *outputtree[nx][ny];
 for (int ix = 0; ix < nx; ix++){
  for (int iy = 0; iy < ny; iy++){
   //couples to write into
   outputtree[ix][iy] = new EdbCouplesTree();
   outputtree[ix][iy]->InitCouplesTree("couples",Form("p%0*i/%i.%i.%i.%i.cp.root",3,plateID,brickID,plateID,ix+1,iy+1),"NEW");
  }
 }

 int entr = 0;
 for(int i=0; i<nlst; i++ ) {
  entr = lst->GetEntry(i);
  mytree->GetEntry(entr);

  EdbSegP *seg_bt = new EdbSegP(); //these must be deleted or not in the loop? We need to write them, so I would say NO
  EdbSegP *seg_s1 = new EdbSegP();
  EdbSegP *seg_s2 = new EdbSegP();

  seg_bt->Copy(*mytree->eS);
  seg_s1->Copy(*mytree->eS1);
  seg_s2->Copy(*mytree->eS2);

  int ix = emulsioncell->IX(seg_bt->X());
  int iy = emulsioncell->IY(seg_bt->Y());
 
  outputtree[ix][iy]->Fill(seg_bt, seg_s1, seg_s2);

 }
 for (int ix = 0; ix < nx; ix++){
  for (int iy = 0; iy < ny; iy++){
   outputtree[ix][iy]->Close();
  }
 }
 cout<<"Start time: "<<tstamp->GetTime()<<endl;
 tstamp->Set();
 cout<<"End time: "<<tstamp->GetTime()<<endl;

}
