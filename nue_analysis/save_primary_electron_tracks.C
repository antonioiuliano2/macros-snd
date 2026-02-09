void save_primary_electron_tracks(){
 using namespace ROOT;

 const int NueEvent = 1028;
 TString inputpath = TString("/eos/experiment/sndlhc/MonteCarlo/FEDRA/nuecc/nuecc_muon1.3E5/b000021/");

 TString inputfile = inputpath + TString(Form("b000021.0.0.%i.trk.root",NueEvent+1));
 RDataFrame df("tracks", inputfile.Data()); // Interface to TTree and TChain
 auto e_df = df.Define("nsegtrue","s.eMCTrack[s.eMCTrack==1 && s.eFlag==1].size()").Filter("nsegtrue > 0"); //the track contains at least a primary electron
 int ntracks_split = e_df.Count().GetValue();
 
 auto e_df1 = e_df.Define("ntracks_split",Form("%i",ntracks_split));

 auto e_df2 = e_df1.Define("s_eZ_electron","s.eZ[s.eMCTrack==1 && s.eFlag==1]");

 float upstream_e_Z = e_df2.Min("s_eZ_electron").GetValue();

 auto upstream_e_df = e_df2.Filter(Form("ROOT::VecOps::Any(s_eZ_electron==%f)",upstream_e_Z));

 upstream_e_df.Snapshot("tracks","test_withdataframe_badevent_singletrack.root");
}