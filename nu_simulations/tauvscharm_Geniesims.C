//compare tau signal vs genie background
double SumArray(ROOT::RVec<double> array, ROOT::RVec<double> phiarray){
    double total = 0.;
    double maxphi = ROOT::VecOps::Max(abs(phiarray));
    for (int i = 0; i < array.size(); i++){
        if (phiarray[i] != maxphi) total += array[i]; //sum over all particle except maximum phi
    }
    return total;
}

double SumArray_NoCharm(ROOT::RVec<double> array, ROOT::RVec<int> pdgarray, ROOT::RVec<double> phiarray){
//sum array content, excluding the one belonging to charm
    ROOT::RVec <int> charmpdgs = {421,411,4122,431, 4132, 4232, 4332, 441};
    double total = 0.;
    int ncharm=0;
    double maxphi = 0; //a very small initial value
    //finding maxphi (excluding charm)
    for (int iparticle = 0; iparticle < pdgarray.size(); iparticle++){
        int pdgcode = pdgarray[iparticle];
        int ischarm = charmpdgs[charmpdgs == TMath::Abs(pdgcode)].size();
        if (ischarm==0 && TMath::Abs(phiarray[iparticle]) > maxphi) maxphi = TMath::Abs(phiarray[iparticle]);
    }
    //doing the sum
    for (int i = 0; i < pdgarray.size(); i++){
        int pdgcode = pdgarray[i];
        int ischarm = charmpdgs[charmpdgs == TMath::Abs(pdgcode)].size();
        if (ischarm == 0){ 
         if (maxphi != phiarray[i])
          total += array[i]; //sum over all particle except maximum phi
        }
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

ROOT::RVec<double> angledifferencewitharray(double ang1, ROOT::RVec<double> ang2array){
  //it cannot go over pi, I need to take the shortest one
  ROOT::RVec<double> deltaarray;
  double delta;
  for (auto & ang2:ang2array){
   delta = angledifference(ang1,ang2);
   deltaarray.push_back(delta);
  }
  return deltaarray;
}

//get array content for charm
void tauvscharm_Geniesims(){
    //****************************tau (signal)***************************************
    //ROOT::RDataFrame dftau("gst","/home/utente/Simulations/sim_snd/GenieEvents_SNDAcceptance/CCDIS/nutau/nutau_CCDIS_FairShip.root"); 
    ROOT::RDataFrame dftau("gst","/home/utente/Simulations/GenieEventsSHiP/GenieEvents_12_19/CCDIS/nu_tau/genie-nu_tau.root");
    auto dftau1 = dftau.Define("phil","TMath::ATan2(pyl,pxl)");

    //computing phi for each particle, deltaphi with respect to charm/tau
    auto dftau2 = dftau1.Define("phif","atan2(pyf,pxf)");

    auto dftau3 = dftau2.Define("deltaphif",angledifferencewitharray,{"phil","phif"});

    //vector sum per components of final hadronic momentum
    auto dftau4 = dftau3.Define("pxf_tot",SumArray,{"pxf","deltaphif"})
                        .Define("pyf_tot",SumArray,{"pyf","deltaphif"})
                        .Define("pzf_tot",SumArray,{"pzf","deltaphif"});

    //getting angles
    auto dftau5 = dftau4.Define("txl","pxl/pzl")
                        .Define("tyl","pyl/pzl");
    auto dftau6 = dftau5.Define("txf","pxf/pzf")
                        .Define("tyf","pyf/pzf");

    //sum of angles for final hadronic state
    auto dftau7 = dftau6.Define("txf_tot",SumArray,{"txf","deltaphif"})
                        .Define("tyf_tot",SumArray,{"tyf","deltaphif"});

    //getting ideal phi of final hadronic system in the ideal case of momenta
    auto dftau8 = dftau7.Define("phif_tot","TMath::ATan2(pyf_tot,pxf_tot)");
    //getting ideal phi of tau lepton
    //deltaphi
    auto dftau9 = dftau8.Define("deltaphi",angledifference,{"phil","phif_tot"});

    //getting detected phi (phi_det) of tau lepton from angles
    auto dftau10 = dftau9.Define("phil_det","TMath::ATan2(tyl,txl)");
    //getting detected phi from angles of final hadronic system
    auto dftau11 = dftau10.Define("phif_tot_det","TMath::ATan2(tyf_tot, txf_tot)");
    //deltaphi
    auto dftau_final = dftau11.Define("deltaphi_det",angledifference,{"phil_det","phif_tot_det"});

    //****************************charm (background) (charm is NOT always pdgf[0], we add a step to find its momenta)*********************************
    //ROOT::RDataFrame dfcharm("gst","/home/utente/Simulations/sim_snd/GenieEvents_SNDAcceptance/CharmCCDIS/numu/numu_CharmCCDIS_FairShip.root");
    ROOT::RDataFrame dfcharm("gst","/home/utente/Simulations/GenieEventsSHiP/GenieEvents_12_19/CharmCCDIS/nu_mu/genie-nu_mu.root");

    auto dfcharm0 = dfcharm.Define("phif","atan2(pyf,pxf)");
    //find charm momentum 
    auto dfcharm1 = dfcharm0.Define("pxcharm",GetCharmValue,{"pxf","pdgf"})
                               .Define("pycharm",GetCharmValue,{"pyf","pdgf"})
                               .Define("pzcharm",GetCharmValue,{"pzf","pdgf"})
                               .Define("pdgcharm",GetCharmValueInt,{"pdgf","pdgf"});              
    //getting ideal phi of charm
    auto dfcharm2 = dfcharm1.Define("phicharm","TMath::ATan2(pycharm,pxcharm)");
    
    auto dfcharm3 = dfcharm2.Define("deltaphif",angledifferencewitharray,{"phicharm","phif"}); 
    //vector sum per components of final hadronic momentum
    auto dfcharm4 = dfcharm3.Define("pxf_tot",SumArray_NoCharm,{"pxf","pdgf","deltaphif"})
                               .Define("pyf_tot",SumArray_NoCharm,{"pyf","pdgf","deltaphif"})
                               .Define("pzf_tot",SumArray_NoCharm,{"pzf","pdgf","deltaphif"});

    //getting angles
    auto dfcharm5 = dfcharm4.Define("txcharm","pxcharm/pzcharm")
                            .Define("tycharm","pycharm/pzcharm");
    auto dfcharm6 = dfcharm5.Define("txf","pxf/pzf")
                            .Define("tyf","pyf/pzf");
    auto dfcharm7 = dfcharm6.Define("txl","pxl/pzl")
                             .Define("tyl","pyl/pzl");
                            
    //sum of angles for final hadronic state
    auto dfcharm8 = dfcharm7.Define("txf_tot",SumArray_NoCharm,{"txf","pdgf","deltaphif"})
                               .Define("tyf_tot",SumArray_NoCharm,{"tyf","pdgf","deltaphif"});

    //getting ideal phi of final hadronic system in the ideal case of momenta
    auto dfcharm9 = dfcharm8.Define("phif_tot","TMath::ATan2(pyf_tot,pxf_tot)");
    //deltaphi
    auto dfcharm10 = dfcharm9.Define("deltaphi",angledifference,{"phicharm","phif_tot"});

    //getting detected phi (phi_det) of charm lepton
    auto dfcharm11 = dfcharm10.Define("phicharm_det","TMath::ATan2(tycharm,txcharm)");
    //getting detected phi of final hadronic system
    auto dfcharm12 = dfcharm11.Define("phif_tot_det","TMath::ATan2(tyf_tot, txf_tot)");
    //deltaphi
    auto dfcharm_final = dfcharm12.Define("deltaphi_det",angledifference,{"phicharm_det","phif_tot_det"});

    //drawing histograms
    auto hdeltaphitau = dftau_final.Define("deltaphi_deg","TMath::Abs(deltaphi * TMath::RadToDeg())")
                                   .Histo1D({"hdeltaphitau","Phi difference Tau;#Delta#phi[rad]",60,0,180},"deltaphi_deg");
    auto hdeltaphicharm = dfcharm_final.Define("deltaphi_deg","TMath::Abs(deltaphi * TMath::RadToDeg())")
                                       .Histo1D({"hdeltaphicharm","Phi difference Charm;#Delta#phi[rad]",60,0,180},"deltaphi_deg");

    auto hdeltaphitau_det = dftau_final.Define("deltaphi_det_deg","TMath::Abs(deltaphi_det * TMath::RadToDeg())")
                                       .Histo1D({"hdeltaphitau_det","Realistic Phi difference Tau;#Delta#phi[rad]",60,0,180},"deltaphi_det_deg");
    auto hdeltaphicharm_det = dfcharm_final.Define("deltaphi_det_deg","TMath::Abs(deltaphi_det * TMath::RadToDeg())")
                                           .Histo1D({"hdeltaphicharm_det","Realistic Phi difference Charm;#Delta#phi[rad]",60,0,180},"deltaphi_det_deg");

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