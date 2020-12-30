//compare tau signal vs genie background
double SumArray(ROOT::RVec<double> array){
    double total = 0.;
    for (auto &value:array){
        total += value;
    }
    return total;
}

double SumArray_NoCharm(ROOT::RVec<double> array, ROOT::RVec<int> pdgarray){
//sum array content, excluding the one belonging to charm
    ROOT::RVec <int> charmpdgs = {421,411,4122,431, 4132, 4232, 4332, 441};
    double total = 0.;
    int ncharm=0;
    for (int i = 0; i < pdgarray.size(); i++){
        int pdgcode = pdgarray[i];
        int ischarm = charmpdgs[charmpdgs == TMath::Abs(pdgcode)].size();
        if (ischarm == 0) total += array[i];
        else ncharm++;
    }
    if (ncharm!=1) cout<<"Warning: only one charm expected, found "<<ncharm<<endl;
    return total;
}

double GetCharmValue(ROOT::RVec<double> array, ROOT::RVec<int> pdgarray){
    ROOT::RVec <int> charmpdgs = {421,411,4122,431, 4132, 4232, 4332, 441};
    double value;
    int ncharm=0;
    for (int i = 0; i < pdgarray.size(); i++){
        int pdgcode = pdgarray[i];
        int ischarm = charmpdgs[charmpdgs == TMath::Abs(pdgcode)].size();
        if (ischarm > 0){
            value = array[i];
            ncharm++;
        }        
    }
    if (ncharm!=1) cout<<"Warning: only one charm expected, found "<<ncharm<<endl;
    return value;
}

int GetCharmValueInt(ROOT::RVec<int> array, ROOT::RVec<int> pdgarray){
    ROOT::RVec<double> doublearray = array;
    return (int) GetCharmValue(array,pdgarray);
}

double angledifference(double ang1, double ang2){
  //it cannot go over pi, I need to take the shortest one
  double delta = ang1 - ang2;
  double pi = TMath::Pi();
  if (delta < pi && delta > (-1 * pi)) delta = delta; //do nothing
  else if (delta > pi) delta = 2 * pi - delta; //positive, but more than 180°
  else delta = -2 * pi - delta; //negative, but less than 180°
  return delta;
}
//get array content for charm
void tauvscharm_Geniesims(){
    //****************************tau (signal)***************************************
    ROOT::RDataFrame dftau("gst","/home/utente/Simulations/sim_snd/GenieEvents_SNDAcceptance/CCDIS/nutau/nutau_CCDIS_FairShip.root"); 

    //vector sum per components of final hadronic momentum
    auto dftau1 = dftau.Define("pxf_tot",SumArray,{"pxf"})
                       .Define("pyf_tot",SumArray,{"pyf"})
                       .Define("pzf_tot",SumArray,{"pzf"});

    //getting angles
    auto dftau2 = dftau1.Define("txl","pxl/pzl")
                        .Define("tyl","pyl/pzl");
    auto dftau3 = dftau2.Define("txf","pxf/pzf")
                        .Define("tyf","pyf/pzf");

    //sum of angles for final hadronic state
    auto dftau4 = dftau3.Define("txf_tot",SumArray,{"txf"})
                        .Define("tyf_tot",SumArray,{"tyf"});

    //getting ideal phi of final hadronic system in the ideal case of momenta
    auto dftau5 = dftau4.Define("phif_tot","TMath::ATan2(pyf_tot,pxf_tot)");
    //getting ideal phi of tau lepton
    auto dftau6 = dftau5.Define("phil","TMath::ATan2(pyl,pxl)");
    //deltaphi
    auto dftau7 = dftau6.Define("deltaphi",angledifference,{"phil","phif_tot"});

    //getting detected phi (phi_det) of tau lepton from angles
    auto dftau8 = dftau7.Define("phil_det","TMath::ATan2(tyl,txl)");
    //getting detected phi from angles of final hadronic system
    auto dftau9 = dftau8.Define("phif_tot_det","TMath::ATan2(tyf_tot, txf_tot)");
    //deltaphi
    auto dftau_final = dftau9.Define("deltaphi_det",angledifference,{"phil_det","phif_tot_det"});

    //****************************charm (background) (charm is NOT always pdgf[0], we add a step to find its momenta)*********************************
    ROOT::RDataFrame dfcharm("gst","/home/utente/Simulations/sim_snd/GenieEvents_SNDAcceptance/CharmCCDIS/numu/numu_CharmCCDIS_FairShip.root");
    //find charm momentum 
    auto dfcharm0 = dfcharm.Define("pxcharm",GetCharmValue,{"pxf","pdgf"})
                           .Define("pycharm",GetCharmValue,{"pyf","pdgf"})
                           .Define("pzcharm",GetCharmValue,{"pzf","pdgf"})
                           .Define("pdgcharm",GetCharmValueInt,{"pdgf","pdgf"});                        
    //vector sum per components of final hadronic momentum
    auto dfcharm1 = dfcharm0.Define("pxf_tot",SumArray_NoCharm,{"pxf","pdgf"})
                           .Define("pyf_tot",SumArray_NoCharm,{"pyf","pdgf"})
                           .Define("pzf_tot",SumArray_NoCharm,{"pzf","pdgf"});

    //getting angles
    auto dfcharm2 = dfcharm1.Define("txcharm","pxcharm/pzcharm")
                            .Define("tycharm","pycharm/pzcharm");
    auto dfcharm3 = dfcharm2.Define("txf","pxf/pzf")
                            .Define("tyf","pyf/pzf");
    auto dfcharm3bis = dfcharm3.Define("txl","pxl/pzl")
                             .Define("tyl","pyl/pzl");
                            
    //sum of angles for final hadronic state
    auto dfcharm4 = dfcharm3bis.Define("txf_tot",SumArray_NoCharm,{"txf","pdgf"})
                            .Define("tyf_tot",SumArray_NoCharm,{"tyf","pdgf"});

    //getting ideal phi of final hadronic system in the ideal case of momenta
    auto dfcharm5 = dfcharm4.Define("phif_tot","TMath::ATan2(pyf_tot,pxf_tot)");
    //getting ideal phi of charm
    auto dfcharm6 = dfcharm5.Define("phicharm","TMath::ATan2(pycharm,pxcharm)");
    //deltaphi
    auto dfcharm7 = dfcharm6.Define("deltaphi",angledifference,{"phicharm","phif_tot"});

    //getting detected phi (phi_det) of charm lepton
    auto dfcharm8 = dfcharm7.Define("phicharm_det","TMath::ATan2(tycharm,txcharm)");
    //getting detected phi of final hadronic system
    auto dfcharm9 = dfcharm8.Define("phif_tot_det","TMath::ATan2(tyf_tot, txf_tot)");
    //deltaphi
    auto dfcharm_final = dfcharm9.Define("deltaphi_det",angledifference,{"phicharm_det","phif_tot_det"});

    //drawing histograms
    auto hdeltaphitau = dftau_final.Define("deltaphi_deg","deltaphi * TMath::RadToDeg()")
                                   .Histo1D({"hdeltaphitau","Phi difference Tau;#Delta#phi[rad]",72,-180,180},"deltaphi_deg");
    auto hdeltaphicharm = dfcharm_final.Define("deltaphi_deg","deltaphi * TMath::RadToDeg()")
                                       .Histo1D({"hdeltaphicharm","Phi difference Charm;#Delta#phi[rad]",72,-180,180},"deltaphi_deg");

    auto hdeltaphitau_det = dftau_final.Define("deltaphi_det_deg","deltaphi_det * TMath::RadToDeg()")
                                       .Histo1D({"hdeltaphitau_det","Realistic Phi difference Tau;#Delta#phi[rad]",72,-180,180},"deltaphi_det_deg");
    auto hdeltaphicharm_det = dfcharm_final.Define("deltaphi_det_deg","deltaphi_det * TMath::RadToDeg()")
                                           .Histo1D({"hdeltaphicharm_det","Realistic Phi difference Charm;#Delta#phi[rad]",72,-180,180},"deltaphi_det_deg");

    TCanvas *cdeltaphi = new TCanvas();
    hdeltaphitau->SetLineColor(kRed);
    hdeltaphitau->DrawClone();
    hdeltaphicharm->DrawClone("SAMES");
    cdeltaphi->BuildLegend();

    TCanvas *cdeltaphi_det = new TCanvas();
    hdeltaphitau_det->SetLineColor(kRed);
    hdeltaphicharm_det->DrawClone();
    hdeltaphitau_det->DrawClone("SAMES");
    cdeltaphi_det->BuildLegend();

    //snapshots
    dftau_final.Snapshot("mygst","nutauCCDIS_annotated.root",
                           {"pxl","pyl","pzl","txl","tyl","phil","phil_det",
                           "pxf_tot","pyf_tot","pzf_tot","txf_tot","tyf_tot","phif_tot","phif_tot_det",
                           "deltaphi","deltaphi_det"});
    dfcharm_final.Snapshot("mygst","numuCharmCCDIS_annotated.root",
                           {"pdgcharm","pxcharm","pycharm","pzcharm","txcharm","tycharm","phicharm","phicharm_det",
                           "pxf_tot","pyf_tot","pzf_tot","txf_tot","tyf_tot","phif_tot","phif_tot_det",
                           "deltaphi","deltaphi_det"});

}