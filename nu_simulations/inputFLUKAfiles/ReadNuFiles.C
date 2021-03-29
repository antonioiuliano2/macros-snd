#include <TROOT.h>
// basic file operations
#include <iostream>
#include <fstream>
using namespace std;

void ReadNuFiles(string fname)
{
	ifstream f(fname.c_str());
	Int_t run =0, evtNumber=0, FlukaID=0, generationN=0;
	Double_t Ekin=0., weight=0., x=0., y=0., x_cos=0., y_cos=0., age=0.;

	string ofname = fname;
	ofname.replace(ofname.end()-4, ofname.end(),".root");
	cout<<ofname<<endl;
	TFile *of = new TFile(ofname.c_str(),"RECREATE");
	TTree *t = new TTree("t","t");
	t->Branch("evtNumber",&evtNumber,"evtNumber/I");
	t->Branch("FlukaID",&FlukaID,"FlukaID/I");
	t->Branch("generationN",&generationN,"generationN/I");
	t->Branch("Ekin",&Ekin,"EKin/D");
	t->Branch("weight",&weight,"weight/D");
	t->Branch("x",&x,"x/D");
	t->Branch("y",&y,"y/D");
	t->Branch("x_cos",&x_cos,"x_cos/D");
	t->Branch("y_cos",&y_cos,"y_cos/D");

	if (f.is_open())
	{
		string line;
		while (!f.eof())
		{
			getline (f, line);
			if (line.find("#")==line.npos) //string search functions return npos if nothing is found
			{
				f>>run>>evtNumber>>FlukaID>>generationN>>Ekin>>weight>>x>>y>>x_cos>>y_cos>>age;
				t->Fill();
				//cout<<FlukaID<<endl;
			}
		}

		f.close();
	}
	else
		cout<<"File not found!"<<endl;

	of->Write();
	of->Close();

}
