//example of quality plots to be compared between different laboratories
void quality_labcomparison(){
 TString eosprefix("root:://eospublic.cern.ch/");
 //a film from each lab (first link, second link, alignment)
 const int nlabs = 3;
 TString labnames[nlabs] = {"Napoli","Bologna","CERN"};
 EColor labcolors[nlabs] ={kBlue,kRed,kBlack};
 TString firstlinkcpfiles[nlabs] = {
    "/eos/experiment/sndlhc/emulsionData/2022/emureco_Napoli/RUN1/b000022/p021/22.21.10.10.firstlinkcp.root",
    "/eos/experiment/sndlhc/emulsionData/2022/emureco_Bologna/RUN1/b000051/p021/51.21.0.0.firstlinkcp.root",
    "/eos/experiment/sndlhc/emulsionData/2022/CERN/emu_reco/RUN1/b000041/p021/41.21.10.10.firstlinkcp.root"
 };

 TString cpfiles[nlabs] = {
    "/eos/experiment/sndlhc/emulsionData/2022/emureco_Napoli/RUN1/b000022/p021/22.21.10.10.cp.root",
    "/eos/experiment/sndlhc/emulsionData/2022/emureco_Bologna/RUN1/b000051/p021/51.21.0.0.cp.root",
    "/eos/experiment/sndlhc/emulsionData/2022/CERN/emu_reco/RUN1/b000041/p021/41.21.10.10.cp.root"
 };
 
 TString alfiles[nlabs] = {
    "/eos/experiment/sndlhc/emulsionData/2022/emureco_Napoli/RUN1/b000021/AFF/21.21.0.0_21.20.0.0.al.root",
    "/eos/experiment/sndlhc/emulsionData/2022/emureco_Bologna/RUN1/b000051/AFF/51.21.0.0_51.20.0.0.al.root",
    "/eos/experiment/sndlhc/emulsionData/2022/CERN/emu_reco/RUN1/b000041/AFF/41.21.0.0_41.20.0.0.al.root"
 };
 //at least 35 segments over 57 were used to produce these plots
 TString trackingeff[nlabs] = {
    "/home/utente/cernbox/Synched/Archivio_cronologico/Ottobre_2023/efficiency_Bologna.root",
    "/home/utente/cernbox/Synched/Archivio_cronologico/Ottobre_2023/heff_CERN.root",
    "/home/utente/cernbox/Synched/Archivio_cronologico/Ottobre_2023/heff_Naples.root"
 };

 TCanvas *c_shr = new TCanvas("c_shr","Shrinkage correction comparison");
 TCanvas *c_chi2 = new TCanvas("c_chi2","Chi square comparison");
 TCanvas *c_al = new TCanvas("c_al","Alignment comparison");
 TCanvas *c_eff = new TCanvas("c_eff","Efficiency comparison");
 c_al->Divide(2,2); //being 2D, it is difficult to superimpose them 
 for (int i=0; i<nlabs;i++){
    c_shr->cd();
    TFile *inputfile = TFile::Open((eosprefix+firstlinkcpfiles[i]).Data());
    TH1F *shr1 = (TH1F*) inputfile->Get("shr1"); //shrinkage correction plot
    shr1->SetTitle(labnames[i]);
    shr1->Scale(1/shr1->Integral());
    shr1->SetLineColor(labcolors[i]);
    if (i==0) shr1->Draw("histo");
    else shr1->Draw("SAME&&histo");

    c_chi2->cd();
    TFile *inputfile_cp = TFile::Open((eosprefix+cpfiles[i]).Data());
    TH1F *hchi = (TH1F*) inputfile_cp->Get("hchi"); //shrinkage correction plot
    hchi->SetTitle(labnames[i]);
    hchi->Scale(1/hchi->Integral());
    hchi->SetLineColor(labcolors[i]);
    if (i==0) hchi->Draw("histo");
    else hchi->Draw("SAME&&histo");

    c_al->cd(i+1);
    TFile *inputfile_al = TFile::Open((eosprefix+alfiles[i]).Data());
    TH2F *xy_final = (TH2F*) inputfile_al->Get("xy_final"); //shrinkage correction plot
    xy_final->SetTitle(labnames[i]);
    xy_final->Draw("COLZ");

    c_eff->cd();
    TFile *inputfile_eff = TFile::Open((trackingeff[i]).Data());
    TProfile *heff = (TProfile*) inputfile_eff->Get("hexpected_clone");
    heff->SetTitle(labnames[i]);
    heff->SetLineColor(labcolors[i]);
    if (i==0) heff->Draw();
    else heff->Draw("SAME");

 }
 c_shr->BuildLegend();
 c_chi2->BuildLegend();
 c_eff->BuildLegend();
}