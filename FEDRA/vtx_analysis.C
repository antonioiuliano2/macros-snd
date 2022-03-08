#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <utility>
#include <string>
#include "TH1.h"
#include "TCanvas.h"
#include "TVector.h"
#include "TVector3.h"
#include "TLegend.h"
#include "TFile.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "/home/simsndlhc/macros-snd/FEDRA/trackutils.h"
namespace VERTEX_PAR
{
  float DZmax = 3000.;
  //float ProbMinV   = 0.0001;  // minimum acceptable probability for chi2-distance between tracks
  float ProbMinV   = 0.01;
  float ImpMax     = 15.;    // maximal acceptable impact parameter [microns] (for preliminary check)
  bool  UseMom     = false;  // use or not track momentum for vertex calculations
  bool  UseSegPar  = true;  // use only the nearest measured segments for vertex fit (as Neuchatel)
  int   QualityMode= 0;      // vertex quality estimation method (0:=Prob/(sigVX^2+sigVY^2); 1:= inverse average track-vertex distance)
}

void vtx_analysis(TString brickID, bool nu = false){
	
    TString inputfile_vtxname = "vertextree.root";
  	using namespace VERTEX_PAR;   
    TFile *inputfile_vtx = TFile::Open(inputfile_vtxname,"READ");
    if (inputfile_vtx == NULL) cout<<"ERROR: inputfile_vtx not found"<<endl;
	TTree *vtxtree = (TTree*) inputfile_vtx->Get("vtx");
	if( nu == true) cout << "++++Running analysis for neutrino vertices++++" << endl;
	else cout << "++++Running analysis for bkg vertices++++" << endl;
 	EdbVertexRec *vertexrec;
 	EdbDataProc *dproc = new EdbDataProc();
	EdbPVRec *ali = new EdbPVRec();
 	EdbScanCond *scancond = new EdbScanCond();
	ali->SetScanCond(scancond);
	
	int ntracks_event[13000] = {0};
    int mostfrequentevent[13000] = {0};
    const int maxtracks = 100;
    
    Int_t MCMotherID[maxtracks]={-99};
    TBranch *b_MCMotherID;
    vtxtree->SetBranchAddress("MCMotherID", MCMotherID, &b_MCMotherID);
	
	TH1F *h_ntrks = new TH1F("h_ntrks","ntrks; ntrks; events",19,1.5,20.5);
	TH1F *h_maxaperture = new TH1F("h_maxaperture","max aperture; maperture; events",100,0,1);
	TH1F *h_frac = new TH1F("h_frac","frac; frac; events",10,0.15,1.15);
	TH1F *h_frac2 = new TH1F("h_frac2","frac (2nd version); frac; events",10,0.15,1.15);
	TH1F *h_quality = new TH1F("h_quality","Quality; eQuality; events",100,0,0.1);
	
	TH1F *h_IP_dau = new TH1F("h_IP_dau","Impact Parameter Dau; Impact Parameter; tracks",150/5,0,150);
	
	TH1F *h_nonprimary = new TH1F("h_nonprimary","non primary / ntrks; ; events",20,0,1);
	
	const int nvertices = vtxtree->GetEntries();
	cout<<"Reading number of vertices: "<<nvertices<<endl;
   	for (int i = 0; i < nvertices; i++){
   		int vID = i;
   		
   		map<int,int> frequencyEvent;
        map<int,int>::iterator it;
        ntracks_event[vID] = 0;
        mostfrequentevent[vID] = -1;
        
   		EdbVertex *vertex = 0;
   		vertexrec = new EdbVertexRec();
 		vertexrec->SetPVRec(ali);
 		vertexrec->eDZmax=DZmax;
 		vertexrec->eProbMin=ProbMinV;
 		vertexrec->eImpMax=ImpMax;
 		vertexrec->eUseMom=UseMom;
 		vertexrec->eUseSegPar=UseSegPar;
 		vertexrec->eQualityMode=QualityMode;
   		vertex = dproc->GetVertexFromTree(*vertexrec,inputfile_vtxname,vID);
   		
   		vtxtree->GetEntry(vID);
   		
   		if(vertex->Flag() == 2 || vertex->Flag() == 5) continue; // excluding impossible vertex topologies (back neutral)
   		
   		if(nu == false)	{	h_ntrks->Fill(vertex->N());
   		
   							h_maxaperture->Fill(vertex->MaxAperture());
   		
   							h_quality->Fill(vertex->Quality());
   						}
   		bool nu_vtx = false;
   		int ntracks = 0;
   		int nonprimary = 0;
		/******* Vertex purity check **************************************************/  		
   		for (int itrk = 0; itrk < vertex->N(); itrk++){
                EdbTrackP *track = vertex->GetTrack(itrk);
                int eventtrack=-99;
                if(vertex->GetVTa(itrk)->Zpos()==1){ // track-start attached to vertex (1st seg)
                    eventtrack = track->GetSegmentFirst()->MCEvt();
                }
                else eventtrack = track->GetSegmentLast()->MCEvt(); //track-end attached to vertex (last seg)
                frequencyEvent[eventtrack]++;      
		}
        for (it = frequencyEvent.begin(); it!=frequencyEvent.end();it++){
			//cout << "\t\t" << it->first << "\t" << it->second << endl;
            if(it->second > ntracks_event[vID]){
            	ntracks_event[vID] = it->second;
                mostfrequentevent[vID] = it->first;
            }
		}
        float frac = (float)ntracks_event[vID]/(float)vertex->N();
		h_frac->Fill(frac);
		// or as alternative, no matter what Zpos, taking as MCEvt the most frequent one occurring in a track (to be checked if valid)
		ntracks_event[vID] = 0;
        mostfrequentevent[vID] = -1;
		for (int itrk = 0; itrk < vertex->N(); itrk++){
                EdbTrackP *track = vertex->GetTrack(itrk);
                int eventtrack=-99;
                eventtrack = FindMostCommonEvent(track);
                frequencyEvent[eventtrack]++;
		}
        for (it = frequencyEvent.begin(); it!=frequencyEvent.end();it++){
            if(it->second > ntracks_event[vID]){
            	ntracks_event[vID] = it->second;
                mostfrequentevent[vID] = it->first;
            }
		}
        float frac2 = (float)ntracks_event[vID]/(float)vertex->N();
		h_frac2->Fill(frac2);
		/*****************************************************************************/
	if	(nu == false){	
		for (int itrk = 0; itrk < vertex->N(); itrk++){
			EdbTrackP *track = vertex->GetTrack(itrk);
			h_IP_dau->Fill(vertex->GetVTa(itrk)->Imp());
		}
	}
	
	if	(nu == true){
		for (int itrk = 0; itrk < vertex->N(); itrk++){
			EdbTrackP *track = vertex->GetTrack(itrk);
            if(MCMotherID[itrk] == 0){
            	ntracks++;
				h_IP_dau->Fill(vertex->GetVTa(itrk)->Imp());
				nu_vtx = true;	
            }
            else nonprimary++;
		}
		//cout << "ntracks motherID=0 "<< ntracks << endl;
		if(ntracks > 1) h_ntrks->Fill(ntracks);
		if(nonprimary !=0 && ntracks >1) h_nonprimary->Fill((float)nonprimary/vertex->N());
    
    	if(nu_vtx == true && ntracks > 1){
    		cout << "vID " << vID << " ntracks " << ntracks  << " nonprimary " << nonprimary << endl;
   			h_maxaperture->Fill(vertex->MaxAperture());
   			if(vertex->MaxAperture() > 0.2) cout << "MaxAperture Greater than 0.2, vID " << vID << endl;
   			h_quality->Fill(vertex->Quality());
    	}    
    }
    
    
     
        
	}//closes loop on vertices
	
			

	// Draw Section
	TCanvas *c_htrk = new TCanvas("c_htrk", "c_htrk", 600,500);
	c_htrk->SetGrid();
	h_ntrks->SetLineWidth(2);
	h_ntrks->SetLineColor(kRed);
	h_ntrks->SetFillStyle(3005);
	h_ntrks->SetFillColor(kRed);
	h_ntrks->Draw();
	
	TCanvas *c_hmaperture = new TCanvas("c_hmaperture", "c_hmaperture", 600,500);
	c_hmaperture->SetGrid();
	h_maxaperture->SetLineWidth(2);
	h_maxaperture->SetLineColor(kRed);
	h_maxaperture->SetFillStyle(3005);
	h_maxaperture->SetFillColor(kRed);
	h_maxaperture->Draw();
	
	TCanvas *c_hfrac = new TCanvas("c_hfrac", "c_hfrac", 600,500);
	c_hfrac->SetGrid();
	h_frac->SetLineWidth(2);
	h_frac->SetLineColor(kRed);
	h_frac->SetFillStyle(3005);
	h_frac->SetFillColor(kRed);
	h_frac->Draw();
	
	TCanvas *c_hfrac2 = new TCanvas("c_hfrac2", "c_hfrac2", 600,500);
	c_hfrac2->SetGrid();
	h_frac2->SetLineWidth(2);
	h_frac2->SetLineColor(kRed);
	h_frac2->SetFillStyle(3005);
	h_frac2->SetFillColor(kRed);
	h_frac2->Draw();
	
	TCanvas *c_hIPdau = new TCanvas("c_hIPdau", "c_hIPdau", 600,500);
	c_hIPdau->SetGrid();
	h_IP_dau->SetLineWidth(2);
	h_IP_dau->SetLineColor(kRed);
	h_IP_dau->SetFillStyle(3005);
	h_IP_dau->SetFillColor(kRed);
	h_IP_dau->Draw();
	
	TCanvas *c_hquality = new TCanvas("c_hquality", "c_hquality", 600,500);
	c_hquality->SetGrid();
	h_quality->SetLineWidth(2);
	h_quality->SetLineColor(kRed);
	h_quality->SetFillStyle(3005);
	h_quality->SetFillColor(kRed);
	h_quality->Draw();
	
	Double_t factor = 1.;
	TH1F *h_ntrks_n = (TH1F*)h_ntrks->Clone("h_ntrks_n");
	h_ntrks_n->Scale(factor/h_ntrks->GetEntries());
	
	TH1F *h_maxaperture_n = (TH1F*)h_maxaperture->Clone("h_maxaperture_n");
	h_maxaperture_n->Scale(factor/h_maxaperture->GetEntries());
	
	TH1F *h_IP_dau_n = (TH1F*)h_IP_dau->Clone("h_IP_dau_n");
	h_IP_dau_n->Scale(factor/h_IP_dau->GetEntries());
	
	TCanvas *c_htrk_n = new TCanvas("c_htrk_n", "c_htrk_n", 600,500);
	c_htrk_n->SetGrid();
	h_ntrks_n->SetLineWidth(2);
	h_ntrks_n->SetLineColor(kRed);
	h_ntrks_n->SetFillStyle(3005);
	h_ntrks_n->SetFillColor(kRed);
	h_ntrks_n->SetNameTitle("h_ntrks_n","ntrks normalized");
	h_ntrks_n->Draw("HIST");
	
	TCanvas *c_hmaperture_n = new TCanvas("c_hmaperture_n", "c_hmaperture_n", 600,500);
	c_hmaperture_n->SetGrid();
	h_maxaperture_n->SetLineWidth(2);
	h_maxaperture_n->SetLineColor(kRed);
	h_maxaperture_n->SetFillStyle(3005);
	h_maxaperture_n->SetFillColor(kRed);
	h_maxaperture_n->SetNameTitle("h_maxaperture_n","max aperture normalized");
	h_maxaperture_n->Draw("HIST");
	
	TCanvas *c_hIPdau_n = new TCanvas("c_hIPdau_n", "c_hIPdau_n", 600,500);
	c_hIPdau_n->SetGrid();
	h_IP_dau_n->SetLineWidth(2);
	h_IP_dau_n->SetLineColor(kRed);
	h_IP_dau_n->SetFillStyle(3005);
	h_IP_dau_n->SetFillColor(kRed);
	h_IP_dau_n->SetNameTitle("h_IP_dau_n", "Impact Parameter Dau normalized");
	h_IP_dau_n->Draw("HIST");
	
if(nu == true){
	TCanvas *c_hnonprimary = new TCanvas("c_hnonprimary", "c_hnonprimary", 600,500);
	c_hnonprimary->SetGrid();
	h_nonprimary->SetLineWidth(2);
	h_nonprimary->SetLineColor(kRed);
	h_nonprimary->SetFillStyle(3005);
	h_nonprimary->SetFillColor(kRed);
	h_nonprimary->Draw();
	}
	
	TFile *outFile = new TFile("hvtx."+brickID+".root", "RECREATE");
	h_ntrks->Write();
	h_maxaperture->Write();
	h_frac->Write();
	h_frac2->Write();
	h_IP_dau->Write();
	h_ntrks_n->Write();
	h_maxaperture_n->Write();
	h_IP_dau_n->Write();
	if(nu == true) h_nonprimary->Write();
	outFile->Close();
	
	//inputfile_vtx->Close();
	
	

}
