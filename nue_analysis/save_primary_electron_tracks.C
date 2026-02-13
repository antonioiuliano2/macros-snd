int save_primary_electron_tracks(int NueEvent){
 using namespace ROOT;

 EnableImplicitMT();
 TString inputpath = TString("/eos/experiment/sndlhc/MonteCarlo/FEDRA/nuecc/nuecc_muon1.3E5/b000021/");

 TString inputfile = inputpath + TString(Form("b000021.0.0.%i.trk.root",NueEvent+1));

 if (gSystem->AccessPathName(inputfile.Data())){
    cout<<"File trk number "<<NueEvent+1<<" does not exist"<<endl;
    return 1;
 }
 RDataFrame df("tracks", inputfile.Data()); // Interface to TTree and TChain
 auto e_df = df.Define("s_eZ_electron","s.eZ[s.eMCTrack==1 && s.eFlag==1]")
    .Define("nsegtrue","s_eZ_electron.size()").Filter("nsegtrue > 0"); //the track contains at least a primary electron
 int ntracks_split = e_df.Count().GetValue(); 
 
 auto e_df1 = e_df.Define("ntracks_split",Form("%i",ntracks_split));

 if (ntracks_split==0){
   cout<<"No volume tracks with electron base-tracks"<<endl;
   e_df1.Snapshot("tracks",
   Form("/afs/cern.ch/work/a/aiulian/public/nueecc_study/primaryelectroninfo_files_updaterdataframe/primaryelectroninfo_%i.root",NueEvent+1));
   return 1;
 }

 //auto e_df2 = e_df1.Define("s_eZ_electron","s.eZ[s.eMCTrack==1 && s.eFlag==1]");

 float upstream_e_Z = e_df1.Min("s_eZ_electron").GetValue();

 auto upstream_e_df = e_df1.Filter(Form("ROOT::VecOps::Any(s_eZ_electron==%f)",upstream_e_Z));

 cout<<"Counting Runs "<<e_df1.GetNRuns()<<" "<<e_df.GetNRuns()<<" "<<df.GetNRuns()<<endl;
 upstream_e_df.Snapshot("tracks",
 Form("/afs/cern.ch/work/a/aiulian/public/nueecc_study/primaryelectroninfo_files_updaterdataframe/primaryelectroninfo_%i.root",NueEvent+1));

 return 0;
}

int main(){
 TStopwatch timewatch;
 timewatch.Start();
 int firstevent = 40;
 int lastevent = 3999;
 for (int ievent = firstevent; ievent <=lastevent; ievent++){
  save_primary_electron_tracks(ievent);
 }

 timewatch.Stop();
 timewatch.Print();

 return 0;
}
