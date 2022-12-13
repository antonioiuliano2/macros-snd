#include <iostream>
#include <vector>

//Computing volume track probability from base-track probabilities
//Combination part derived from 
//https://stackoverflow.com/questions/12991758/creating-all-possible-k-combinations-of-n-items-in-c
//Launch with
//.L combination_basetracks_efficiencies.C
//main()
//global variables
using namespace std;

vector<int> people;
vector<int> combination;

int countiterations = 0;
int iplateoffset = 6; //I did not start from 1 to 25, but from 7 (just for printing)

const int nfilms = 25;
double plateefficiency[nfilms];

void SetEfficiencyArray(){
    //TFile *inputfile = TFile::Open("efficiency_plate_requiredallfilms.root");
    TFile *inputfile = TFile::Open("efficiency_plate_allquarters.root");
    TEfficiency *heffplates = (TEfficiency*) inputfile->Get("hexpected_clone");
    cout<<"Starting code, getting efficiency from tgraph"<<endl;
    for (int ifilm = 0; ifilm < nfilms; ifilm++){
        plateefficiency[ifilm] = heffplates->GetEfficiency(iplateoffset+2+ifilm);
        cout<<"Efficiency for film "<<ifilm+iplateoffset+1<<": "<<plateefficiency[ifilm]<<endl;
    }
}

double pevent(const vector<int>&combination){ 
 //probability of having a particular combination of base-tracks
 //if base-track in a plate is there, multiply for eff[film], otherwise multiply for 1-eff[film]   
 double prob = 1.;
 const int nfilms = 25;
 /*
 double plateefficiency[nfilms]{
    0.920287,
    0.892932,
    0.930555,
    0.917941,
    0.95256,
    0.957046,
    0.943092,
    0.966633,
    0.877558,
    0.962151,
    0.97265,
    0.970084,
    0.968353,
    0.970078,
    0.939952,
    0.88511,
    0.968246,
    0.925841,
    0.944591,
    0.813822,
    0.933855,
    0.942422,
    0.882333,
    0.909087,
    0.838794,
  };
  */
 bool filmfound[nfilms]; //will tell us if film contains a basetrack
 std::fill_n(filmfound,nfilms,false); //should be already false by default, but let us initialize it for safety
 //looping over all films, storing the ones "found" in the combination
 for (int iplate:combination){
    filmfound[iplate-1] = true; //plates number start from 1, array elements from 0
 }
 for (int ifilm = 0; ifilm < nfilms; ifilm++){
    if (filmfound[ifilm]) prob = prob * plateefficiency[ifilm]; //plate was found, p;
    else prob = prob * (1-plateefficiency[ifilm]); //plate was not found, 1-p;
 }
 cout<<"prob "<<prob<<endl;
 return prob;
}

void pretty_print(const vector<int>& v) {
  //static int count = 0;
  cout << "combination no " << (++countiterations) << ": [ ";
  for (int i = 0; i < v.size(); ++i) { cout << v[i] + iplateoffset << " "; }
  cout << "] ";
}
double totprob = 0.; //global variabile, reporting total probability

void go(int offset, int k) {

  if (k == 0) {
    //print the combination for check
    pretty_print(combination);
    //compute probability of this combination, add to total probability
    totprob += pevent(combination);
    return;
  }
  for (int i = offset; i <= people.size() - k; ++i) {
    combination.push_back(people[i]);
    go(i+1, k-1);
    combination.pop_back();
  }
}

int main() {
  const int n = 25, mink = 18;
  SetEfficiencyArray();
  double probiteration = 0;
  for (int i = 0; i < n; ++i) { people.push_back(i+1); }
  for (int k = mink; k <= n; ++k) {
    cout<<"Starting combinations of "<<k<<" plates over "<<n<<endl;
    go(0, k);countiterations = 0;
    cout<<"Probability of having "<<k<<" plates over "<<n<<": "<<totprob - probiteration<<endl;
    probiteration = totprob;
    }
  cout<<totprob<<endl;
  
  return 0;
}

