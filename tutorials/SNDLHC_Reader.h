//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Mar 13 19:45:01 2021 by ROOT version 6.23/01
// from TTree cbmsim//cbmroot_0
// found on file: ship.conical.Genie-TGeant4.root
//////////////////////////////////////////////////////////

#ifndef SNDLHC_Reader_h
#define SNDLHC_Reader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "TClonesArray.h"
#include "TVector3.h"
#include "TObject.h"
/*#include "ShipMCTrack.h"
#include "FairMultiLinkedData_Interface.h"
#include "vetoPoint.h"
#include "EmulsionDetPoint.h"
#include "ScifiPoint.h"
#include "MuFilterPoint.h"*/
#include "TNamed.h"

class SNDLHC_Reader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.
   static constexpr Int_t kMaxcbmroot_0_Stack_MCTrack = 100000;
   static constexpr Int_t kMaxcbmroot_0_veto_vetoPoint = 1000;
   static constexpr Int_t kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint = 100000;
   static constexpr Int_t kMaxcbmroot_0_Scifi_ScifiPoint = 100000;
   static constexpr Int_t kMaxcbmroot_0_MuFilter_MuFilterPoint = 100000;
   static constexpr Int_t kMaxcbmroot_0_Event_MCEventHeader = 1;

   // Declaration of leaf types
   Int_t           MCTrack_;
   UInt_t          MCTrack_fUniqueID[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   UInt_t          MCTrack_fBits[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Int_t           MCTrack_fPdgCode[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Int_t           MCTrack_fMotherId[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fPx[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fPy[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fPz[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fM[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fStartX[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fStartY[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fStartZ[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fStartT[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Double_t        MCTrack_fW[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Int_t           MCTrack_fProcID[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Int_t           MCTrack_fNPoints[kMaxcbmroot_0_Stack_MCTrack];   //[cbmroot_0.Stack.MCTrack_]
   Int_t           vetoPoint_;
   UInt_t          vetoPoint_fUniqueID[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   UInt_t          vetoPoint_fBits[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
//   FairMultiLinkedData vetoPoint_fLink[kMaxcbmroot_0_veto_vetoPoint];
   Int_t           vetoPoint_fTrackID[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   UInt_t          vetoPoint_fEventId[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fPx[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fPy[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fPz[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fTime[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fLength[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fELoss[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Int_t           vetoPoint_fDetectorID[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fX[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fY[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Double_t        vetoPoint_fZ[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   Int_t           vetoPoint_fPdgCode[kMaxcbmroot_0_veto_vetoPoint];   //[cbmroot_0.veto.vetoPoint_]
   TVector3        vetoPoint_fLpos[kMaxcbmroot_0_veto_vetoPoint];
   TVector3        vetoPoint_fLmom[kMaxcbmroot_0_veto_vetoPoint];
   Int_t           EmulsionDetPoint_;
   UInt_t          EmulsionDetPoint_fUniqueID[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   UInt_t          EmulsionDetPoint_fBits[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
//   FairMultiLinkedData EmulsionDetPoint_fLink[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];
   Int_t           EmulsionDetPoint_fTrackID[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   UInt_t          EmulsionDetPoint_fEventId[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fPx[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fPy[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fPz[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fTime[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fLength[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fELoss[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Int_t           EmulsionDetPoint_fDetectorID[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fX[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fY[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Double_t        EmulsionDetPoint_fZ[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Int_t           EmulsionDetPoint_fPdgCode[kMaxcbmroot_0_EmulsionDet_EmulsionDetPoint];   //[cbmroot_0.EmulsionDet.EmulsionDetPoint_]
   Int_t           ScifiPoint_;
   UInt_t          ScifiPoint_fUniqueID[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   UInt_t          ScifiPoint_fBits[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
//   FairMultiLinkedData ScifiPoint_fLink[kMaxcbmroot_0_Scifi_ScifiPoint];
   Int_t           ScifiPoint_fTrackID[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   UInt_t          ScifiPoint_fEventId[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fPx[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fPy[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fPz[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fTime[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fLength[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fELoss[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Int_t           ScifiPoint_fDetectorID[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fX[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fY[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Double_t        ScifiPoint_fZ[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Int_t           ScifiPoint_fPdgCode[kMaxcbmroot_0_Scifi_ScifiPoint];   //[cbmroot_0.Scifi.ScifiPoint_]
   Int_t           MuFilterPoint_;
   UInt_t          MuFilterPoint_fUniqueID[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   UInt_t          MuFilterPoint_fBits[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
  // FairMultiLinkedData MuFilterPoint_fLink[kMaxcbmroot_0_MuFilter_MuFilterPoint];
   Int_t           MuFilterPoint_fTrackID[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   UInt_t          MuFilterPoint_fEventId[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fPx[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fPy[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fPz[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fTime[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fLength[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fELoss[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Int_t           MuFilterPoint_fDetectorID[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fX[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fY[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Double_t        MuFilterPoint_fZ[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
   Int_t           MuFilterPoint_fPdgCode[kMaxcbmroot_0_MuFilter_MuFilterPoint];   //[cbmroot_0.MuFilter.MuFilterPoint_]
 //  FairMCEventHeader *MCEventHeader_;
   UInt_t          MCEventHeader_TNamed_fUniqueID;
   UInt_t          MCEventHeader_TNamed_fBits;
   TString         MCEventHeader_TNamed_fName;
   TString         MCEventHeader_TNamed_fTitle;
   UInt_t          MCEventHeader_fRunId;
   UInt_t          MCEventHeader_fEventId;
   Double_t        MCEventHeader_fX;
   Double_t        MCEventHeader_fY;
   Double_t        MCEventHeader_fZ;
   Double_t        MCEventHeader_fT;
   Double_t        MCEventHeader_fB;
   Int_t           MCEventHeader_fNPrim;
   Bool_t          MCEventHeader_fIsSet;
   Double_t        MCEventHeader_fRotX;
   Double_t        MCEventHeader_fRotY;
   Double_t        MCEventHeader_fRotZ;

   // List of branches
   TBranch        *b_cbmroot_0_Stack_MCTrack_;   //!
   TBranch        *b_MCTrack_fUniqueID;   //!
   TBranch        *b_MCTrack_fBits;   //!
   TBranch        *b_MCTrack_fPdgCode;   //!
   TBranch        *b_MCTrack_fMotherId;   //!
   TBranch        *b_MCTrack_fPx;   //!
   TBranch        *b_MCTrack_fPy;   //!
   TBranch        *b_MCTrack_fPz;   //!
   TBranch        *b_MCTrack_fM;   //!
   TBranch        *b_MCTrack_fStartX;   //!
   TBranch        *b_MCTrack_fStartY;   //!
   TBranch        *b_MCTrack_fStartZ;   //!
   TBranch        *b_MCTrack_fStartT;   //!
   TBranch        *b_MCTrack_fW;   //!
   TBranch        *b_MCTrack_fProcID;   //!
   TBranch        *b_MCTrack_fNPoints;   //!
   TBranch        *b_cbmroot_0_veto_vetoPoint_;   //!
   TBranch        *b_vetoPoint_fUniqueID;   //!
   TBranch        *b_vetoPoint_fBits;   //!
  // TBranch        *b_vetoPoint_fLink;   //!
   TBranch        *b_vetoPoint_fTrackID;   //!
   TBranch        *b_vetoPoint_fEventId;   //!
   TBranch        *b_vetoPoint_fPx;   //!
   TBranch        *b_vetoPoint_fPy;   //!
   TBranch        *b_vetoPoint_fPz;   //!
   TBranch        *b_vetoPoint_fTime;   //!
   TBranch        *b_vetoPoint_fLength;   //!
   TBranch        *b_vetoPoint_fELoss;   //!
   TBranch        *b_vetoPoint_fDetectorID;   //!
   TBranch        *b_vetoPoint_fX;   //!
   TBranch        *b_vetoPoint_fY;   //!
   TBranch        *b_vetoPoint_fZ;   //!
   TBranch        *b_vetoPoint_fPdgCode;   //!
   TBranch        *b_vetoPoint_fLpos;   //!
   TBranch        *b_vetoPoint_fLmom;   //!
   TBranch        *b_cbmroot_0_EmulsionDet_EmulsionDetPoint_;   //!
   TBranch        *b_EmulsionDetPoint_fUniqueID;   //!
   TBranch        *b_EmulsionDetPoint_fBits;   //!
 //  TBranch        *b_EmulsionDetPoint_fLink;   //!
   TBranch        *b_EmulsionDetPoint_fTrackID;   //!
   TBranch        *b_EmulsionDetPoint_fEventId;   //!
   TBranch        *b_EmulsionDetPoint_fPx;   //!
   TBranch        *b_EmulsionDetPoint_fPy;   //!
   TBranch        *b_EmulsionDetPoint_fPz;   //!
   TBranch        *b_EmulsionDetPoint_fTime;   //!
   TBranch        *b_EmulsionDetPoint_fLength;   //!
   TBranch        *b_EmulsionDetPoint_fELoss;   //!
   TBranch        *b_EmulsionDetPoint_fDetectorID;   //!
   TBranch        *b_EmulsionDetPoint_fX;   //!
   TBranch        *b_EmulsionDetPoint_fY;   //!
   TBranch        *b_EmulsionDetPoint_fZ;   //!
   TBranch        *b_EmulsionDetPoint_fPdgCode;   //!
   TBranch        *b_cbmroot_0_Scifi_ScifiPoint_;   //!
   TBranch        *b_ScifiPoint_fUniqueID;   //!
   TBranch        *b_ScifiPoint_fBits;   //!
 //  TBranch        *b_ScifiPoint_fLink;   //!
   TBranch        *b_ScifiPoint_fTrackID;   //!
   TBranch        *b_ScifiPoint_fEventId;   //!
   TBranch        *b_ScifiPoint_fPx;   //!
   TBranch        *b_ScifiPoint_fPy;   //!
   TBranch        *b_ScifiPoint_fPz;   //!
   TBranch        *b_ScifiPoint_fTime;   //!
   TBranch        *b_ScifiPoint_fLength;   //!
   TBranch        *b_ScifiPoint_fELoss;   //!
   TBranch        *b_ScifiPoint_fDetectorID;   //!
   TBranch        *b_ScifiPoint_fX;   //!
   TBranch        *b_ScifiPoint_fY;   //!
   TBranch        *b_ScifiPoint_fZ;   //!
   TBranch        *b_ScifiPoint_fPdgCode;   //!
   TBranch        *b_cbmroot_0_MuFilter_MuFilterPoint_;   //!
   TBranch        *b_MuFilterPoint_fUniqueID;   //!
   TBranch        *b_MuFilterPoint_fBits;   //!
 //  TBranch        *b_MuFilterPoint_fLink;   //!
   TBranch        *b_MuFilterPoint_fTrackID;   //!
   TBranch        *b_MuFilterPoint_fEventId;   //!
   TBranch        *b_MuFilterPoint_fPx;   //!
   TBranch        *b_MuFilterPoint_fPy;   //!
   TBranch        *b_MuFilterPoint_fPz;   //!
   TBranch        *b_MuFilterPoint_fTime;   //!
   TBranch        *b_MuFilterPoint_fLength;   //!
   TBranch        *b_MuFilterPoint_fELoss;   //!
   TBranch        *b_MuFilterPoint_fDetectorID;   //!
   TBranch        *b_MuFilterPoint_fX;   //!
   TBranch        *b_MuFilterPoint_fY;   //!
   TBranch        *b_MuFilterPoint_fZ;   //!
   TBranch        *b_MuFilterPoint_fPdgCode;   //!
   //TBranch        *b_cbmroot_0_Event_MCEventHeader_;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_TNamed_fUniqueID;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_TNamed_fBits;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_TNamed_fName;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_TNamed_fTitle;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fRunId;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fEventId;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fX;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fY;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fZ;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fT;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fB;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fNPrim;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fIsSet;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fRotX;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fRotY;   //!
   TBranch        *b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fRotZ;   //!

   SNDLHC_Reader(TTree *tree=0);
   Int_t GetMotherPdg(Int_t trackID);
   virtual ~SNDLHC_Reader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef SNDLHC_Reader_cxx
SNDLHC_Reader::SNDLHC_Reader(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("inECC_sndLHC.Genie-TGeant4.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("inECC_sndLHC.Genie-TGeant4.root");
      }
      f->GetObject("cbmsim",tree);

   }
   Init(tree);
}

SNDLHC_Reader::~SNDLHC_Reader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SNDLHC_Reader::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SNDLHC_Reader::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void SNDLHC_Reader::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   //MCEventHeader_ = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("MCTrack", &MCTrack_, &b_cbmroot_0_Stack_MCTrack_);
   fChain->SetBranchAddress("MCTrack.fUniqueID", MCTrack_fUniqueID, &b_MCTrack_fUniqueID);
   fChain->SetBranchAddress("MCTrack.fBits", MCTrack_fBits, &b_MCTrack_fBits);
   fChain->SetBranchAddress("MCTrack.fPdgCode", MCTrack_fPdgCode, &b_MCTrack_fPdgCode);
   fChain->SetBranchAddress("MCTrack.fMotherId", MCTrack_fMotherId, &b_MCTrack_fMotherId);
   fChain->SetBranchAddress("MCTrack.fPx", MCTrack_fPx, &b_MCTrack_fPx);
   fChain->SetBranchAddress("MCTrack.fPy", MCTrack_fPy, &b_MCTrack_fPy);
   fChain->SetBranchAddress("MCTrack.fPz", MCTrack_fPz, &b_MCTrack_fPz);
   fChain->SetBranchAddress("MCTrack.fM", MCTrack_fM, &b_MCTrack_fM);
   fChain->SetBranchAddress("MCTrack.fStartX", MCTrack_fStartX, &b_MCTrack_fStartX);
   fChain->SetBranchAddress("MCTrack.fStartY", MCTrack_fStartY, &b_MCTrack_fStartY);
   fChain->SetBranchAddress("MCTrack.fStartZ", MCTrack_fStartZ, &b_MCTrack_fStartZ);
   fChain->SetBranchAddress("MCTrack.fStartT", MCTrack_fStartT, &b_MCTrack_fStartT);
   fChain->SetBranchAddress("MCTrack.fW", MCTrack_fW, &b_MCTrack_fW);
   fChain->SetBranchAddress("MCTrack.fProcID", MCTrack_fProcID, &b_MCTrack_fProcID);
   fChain->SetBranchAddress("MCTrack.fNPoints", MCTrack_fNPoints, &b_MCTrack_fNPoints);
   fChain->SetBranchAddress("vetoPoint", &vetoPoint_, &b_cbmroot_0_veto_vetoPoint_);
   fChain->SetBranchAddress("vetoPoint.fUniqueID", &vetoPoint_fUniqueID, &b_vetoPoint_fUniqueID);
   fChain->SetBranchAddress("vetoPoint.fBits", &vetoPoint_fBits, &b_vetoPoint_fBits);
  // fChain->SetBranchAddress("vetoPoint.fLink", &vetoPoint_fLink, &b_vetoPoint_fLink);
   fChain->SetBranchAddress("vetoPoint.fTrackID", &vetoPoint_fTrackID, &b_vetoPoint_fTrackID);
   fChain->SetBranchAddress("vetoPoint.fEventId", &vetoPoint_fEventId, &b_vetoPoint_fEventId);
   fChain->SetBranchAddress("vetoPoint.fPx", &vetoPoint_fPx, &b_vetoPoint_fPx);
   fChain->SetBranchAddress("vetoPoint.fPy", &vetoPoint_fPy, &b_vetoPoint_fPy);
   fChain->SetBranchAddress("vetoPoint.fPz", &vetoPoint_fPz, &b_vetoPoint_fPz);
   fChain->SetBranchAddress("vetoPoint.fTime", &vetoPoint_fTime, &b_vetoPoint_fTime);
   fChain->SetBranchAddress("vetoPoint.fLength", &vetoPoint_fLength, &b_vetoPoint_fLength);
   fChain->SetBranchAddress("vetoPoint.fELoss", &vetoPoint_fELoss, &b_vetoPoint_fELoss);
   fChain->SetBranchAddress("vetoPoint.fDetectorID", &vetoPoint_fDetectorID, &b_vetoPoint_fDetectorID);
   fChain->SetBranchAddress("vetoPoint.fX", &vetoPoint_fX, &b_vetoPoint_fX);
   fChain->SetBranchAddress("vetoPoint.fY", &vetoPoint_fY, &b_vetoPoint_fY);
   fChain->SetBranchAddress("vetoPoint.fZ", &vetoPoint_fZ, &b_vetoPoint_fZ);
   fChain->SetBranchAddress("vetoPoint.fPdgCode", &vetoPoint_fPdgCode, &b_vetoPoint_fPdgCode);
   fChain->SetBranchAddress("vetoPoint.fLpos", &vetoPoint_fLpos, &b_vetoPoint_fLpos);
   fChain->SetBranchAddress("vetoPoint.fLmom", &vetoPoint_fLmom, &b_vetoPoint_fLmom);
   fChain->SetBranchAddress("EmulsionDetPoint", &EmulsionDetPoint_, &b_cbmroot_0_EmulsionDet_EmulsionDetPoint_);
   fChain->SetBranchAddress("EmulsionDetPoint.fUniqueID", &EmulsionDetPoint_fUniqueID, &b_EmulsionDetPoint_fUniqueID);
   fChain->SetBranchAddress("EmulsionDetPoint.fBits", &EmulsionDetPoint_fBits, &b_EmulsionDetPoint_fBits);
  // fChain->SetBranchAddress("EmulsionDetPoint.fLink", &EmulsionDetPoint_fLink, &b_EmulsionDetPoint_fLink);
   fChain->SetBranchAddress("EmulsionDetPoint.fTrackID", &EmulsionDetPoint_fTrackID, &b_EmulsionDetPoint_fTrackID);
   fChain->SetBranchAddress("EmulsionDetPoint.fEventId", &EmulsionDetPoint_fEventId, &b_EmulsionDetPoint_fEventId);
   fChain->SetBranchAddress("EmulsionDetPoint.fPx", &EmulsionDetPoint_fPx, &b_EmulsionDetPoint_fPx);
   fChain->SetBranchAddress("EmulsionDetPoint.fPy", &EmulsionDetPoint_fPy, &b_EmulsionDetPoint_fPy);
   fChain->SetBranchAddress("EmulsionDetPoint.fPz", &EmulsionDetPoint_fPz, &b_EmulsionDetPoint_fPz);
   fChain->SetBranchAddress("EmulsionDetPoint.fTime", &EmulsionDetPoint_fTime, &b_EmulsionDetPoint_fTime);
   fChain->SetBranchAddress("EmulsionDetPoint.fLength", &EmulsionDetPoint_fLength, &b_EmulsionDetPoint_fLength);
   fChain->SetBranchAddress("EmulsionDetPoint.fELoss", &EmulsionDetPoint_fELoss, &b_EmulsionDetPoint_fELoss);
   fChain->SetBranchAddress("EmulsionDetPoint.fDetectorID", &EmulsionDetPoint_fDetectorID, &b_EmulsionDetPoint_fDetectorID);
   fChain->SetBranchAddress("EmulsionDetPoint.fX", &EmulsionDetPoint_fX, &b_EmulsionDetPoint_fX);
   fChain->SetBranchAddress("EmulsionDetPoint.fY", &EmulsionDetPoint_fY, &b_EmulsionDetPoint_fY);
   fChain->SetBranchAddress("EmulsionDetPoint.fZ", &EmulsionDetPoint_fZ, &b_EmulsionDetPoint_fZ);
   fChain->SetBranchAddress("EmulsionDetPoint.fPdgCode", &EmulsionDetPoint_fPdgCode, &b_EmulsionDetPoint_fPdgCode);
   fChain->SetBranchAddress("ScifiPoint", &ScifiPoint_, &b_cbmroot_0_Scifi_ScifiPoint_);
   fChain->SetBranchAddress("ScifiPoint.fUniqueID", ScifiPoint_fUniqueID, &b_ScifiPoint_fUniqueID);
   fChain->SetBranchAddress("ScifiPoint.fBits", ScifiPoint_fBits, &b_ScifiPoint_fBits);
   //fChain->SetBranchAddress("ScifiPoint.fLink", ScifiPoint_fLink, &b_ScifiPoint_fLink);
   fChain->SetBranchAddress("ScifiPoint.fTrackID", ScifiPoint_fTrackID, &b_ScifiPoint_fTrackID);
   fChain->SetBranchAddress("ScifiPoint.fEventId", ScifiPoint_fEventId, &b_ScifiPoint_fEventId);
   fChain->SetBranchAddress("ScifiPoint.fPx", ScifiPoint_fPx, &b_ScifiPoint_fPx);
   fChain->SetBranchAddress("ScifiPoint.fPy", ScifiPoint_fPy, &b_ScifiPoint_fPy);
   fChain->SetBranchAddress("ScifiPoint.fPz", ScifiPoint_fPz, &b_ScifiPoint_fPz);
   fChain->SetBranchAddress("ScifiPoint.fTime", ScifiPoint_fTime, &b_ScifiPoint_fTime);
   fChain->SetBranchAddress("ScifiPoint.fLength", ScifiPoint_fLength, &b_ScifiPoint_fLength);
   fChain->SetBranchAddress("ScifiPoint.fELoss", ScifiPoint_fELoss, &b_ScifiPoint_fELoss);
   fChain->SetBranchAddress("ScifiPoint.fDetectorID", ScifiPoint_fDetectorID, &b_ScifiPoint_fDetectorID);
   fChain->SetBranchAddress("ScifiPoint.fX", ScifiPoint_fX, &b_ScifiPoint_fX);
   fChain->SetBranchAddress("ScifiPoint.fY", ScifiPoint_fY, &b_ScifiPoint_fY);
   fChain->SetBranchAddress("ScifiPoint.fZ", ScifiPoint_fZ, &b_ScifiPoint_fZ);
   fChain->SetBranchAddress("ScifiPoint.fPdgCode", ScifiPoint_fPdgCode, &b_ScifiPoint_fPdgCode);
   fChain->SetBranchAddress("MuFilterPoint", &MuFilterPoint_, &b_cbmroot_0_MuFilter_MuFilterPoint_);
   fChain->SetBranchAddress("MuFilterPoint.fUniqueID", MuFilterPoint_fUniqueID, &b_MuFilterPoint_fUniqueID);
   fChain->SetBranchAddress("MuFilterPoint.fBits", MuFilterPoint_fBits, &b_MuFilterPoint_fBits);
  // fChain->SetBranchAddress("MuFilterPoint.fLink", MuFilterPoint_fLink, &b_MuFilterPoint_fLink);
   fChain->SetBranchAddress("MuFilterPoint.fTrackID", MuFilterPoint_fTrackID, &b_MuFilterPoint_fTrackID);
   fChain->SetBranchAddress("MuFilterPoint.fEventId", MuFilterPoint_fEventId, &b_MuFilterPoint_fEventId);
   fChain->SetBranchAddress("MuFilterPoint.fPx", MuFilterPoint_fPx, &b_MuFilterPoint_fPx);
   fChain->SetBranchAddress("MuFilterPoint.fPy", MuFilterPoint_fPy, &b_MuFilterPoint_fPy);
   fChain->SetBranchAddress("MuFilterPoint.fPz", MuFilterPoint_fPz, &b_MuFilterPoint_fPz);
   fChain->SetBranchAddress("MuFilterPoint.fTime", MuFilterPoint_fTime, &b_MuFilterPoint_fTime);
   fChain->SetBranchAddress("MuFilterPoint.fLength", MuFilterPoint_fLength, &b_MuFilterPoint_fLength);
   fChain->SetBranchAddress("MuFilterPoint.fELoss", MuFilterPoint_fELoss, &b_MuFilterPoint_fELoss);
   fChain->SetBranchAddress("MuFilterPoint.fDetectorID", MuFilterPoint_fDetectorID, &b_MuFilterPoint_fDetectorID);
   fChain->SetBranchAddress("MuFilterPoint.fX", MuFilterPoint_fX, &b_MuFilterPoint_fX);
   fChain->SetBranchAddress("MuFilterPoint.fY", MuFilterPoint_fY, &b_MuFilterPoint_fY);
   fChain->SetBranchAddress("MuFilterPoint.fZ", MuFilterPoint_fZ, &b_MuFilterPoint_fZ);
   fChain->SetBranchAddress("MuFilterPoint.fPdgCode", MuFilterPoint_fPdgCode, &b_MuFilterPoint_fPdgCode);
   //fChain->SetBranchAddress("MCEventHeader.", &MCEventHeader_, &b_cbmroot_0_Event_MCEventHeader_);
   fChain->SetBranchAddress("MCEventHeader.TNamed.fUniqueID", &MCEventHeader_TNamed_fUniqueID, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_TNamed_fUniqueID);
   fChain->SetBranchAddress("MCEventHeader.TNamed.fBits", &MCEventHeader_TNamed_fBits, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_TNamed_fBits);
   fChain->SetBranchAddress("MCEventHeader.TNamed.fName", &MCEventHeader_TNamed_fName, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_TNamed_fName);
   fChain->SetBranchAddress("MCEventHeader.TNamed.fTitle", &MCEventHeader_TNamed_fTitle, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_TNamed_fTitle);
   fChain->SetBranchAddress("MCEventHeader.fRunId", &MCEventHeader_fRunId, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fRunId);
   fChain->SetBranchAddress("MCEventHeader.fEventId", &MCEventHeader_fEventId, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fEventId);
   fChain->SetBranchAddress("MCEventHeader.fX", &MCEventHeader_fX, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fX);
   fChain->SetBranchAddress("MCEventHeader.fY", &MCEventHeader_fY, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fY);
   fChain->SetBranchAddress("MCEventHeader.fZ", &MCEventHeader_fZ, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fZ);
   fChain->SetBranchAddress("MCEventHeader.fT", &MCEventHeader_fT, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fT);
   fChain->SetBranchAddress("MCEventHeader.fB", &MCEventHeader_fB, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fB);
   fChain->SetBranchAddress("MCEventHeader.fNPrim", &MCEventHeader_fNPrim, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fNPrim);
   fChain->SetBranchAddress("MCEventHeader.fIsSet", &MCEventHeader_fIsSet, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fIsSet);
   fChain->SetBranchAddress("MCEventHeader.fRotX", &MCEventHeader_fRotX, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fRotX);
   fChain->SetBranchAddress("MCEventHeader.fRotY", &MCEventHeader_fRotY, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fRotY);
   fChain->SetBranchAddress("MCEventHeader.fRotZ", &MCEventHeader_fRotZ, &b_MCEventHeader_cbmroot_0_Event_MCEventHeader_fRotZ);
   Notify();
}

Bool_t SNDLHC_Reader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void SNDLHC_Reader::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SNDLHC_Reader::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef SNDLHC_Reader_cxx
