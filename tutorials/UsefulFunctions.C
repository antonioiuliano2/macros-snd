//list of useful functions (Since I always lose tracks of them)

vector<double> eff_ClopperPears(int foundevents, int totalevents){
  //using Clopper Pearson confidence limits, more reliable and recommended by Lista and PDG
  vector<double> efficiency; //value, lower limit, upper limit

  efficiency.push_back((double) foundevents/totalevents);

  double conflevel = 0.683; //default, 1 sigma

  //computing lower and upper limits
  double efflow = TEfficiency::ClopperPearson(totalevents,foundevents,conflevel,false);
  efficiency.push_back(efflow);
  double effhigh = TEfficiency::ClopperPearson(totalevents,foundevents,conflevel,true);
  efficiency.push_back(effhigh);
  return efficiency;
}

//efficiency formula (simple one,errors are problematic when efficiencies are too close to 1 and 0)

vector<double> eff_formula(int foundevents, int totalevents){
  vector<double> efficiency; //value and error
  
  efficiency.push_back((double) foundevents/totalevents);

  double efferr = TMath::Sqrt(efficiency[0] * (1- efficiency[0])/totalevents);
  efficiency.push_back(efferr);
  
  return efficiency;
}

//when events are weighted, you still should consider the actual number of simulated events for the error!
vector<double> eff_formula_weighted(int foundweight, int totalweight, int Nevents_total){
  vector<double> efficiency; //value and error
  
  efficiency.push_back((double) foundweight/totalweight);

  //totalweight and foundweight are weighted, I need to divide with the actual number of events simulated!
  double efferr = TMath::Sqrt(efficiency[0] * (1- efficiency[0])/Nevents_total);
  efficiency.push_back(efferr);
  
  return efficiency;

//getting charge from pdgcode, handling case when pdgcode is not found  
double GetParticleCharge (int pdgcode, TDatabasePDG *pdg){
  //from PDG, get charge
  double charge = 0.;
  if (pdg->GetParticle(pdgcode)) charge = pdg->GetParticle(pdgcode)->Charge();
  else if (pdgcode > 1e+8) charge = 1.; //heavy nuclei are charged, but they are not recognized by the TDatabasePDG
  return charge;
}

ROOT::RVec <int> charmpdgs = {421,411,4122,431, 4132, 4232, 4332, 441};