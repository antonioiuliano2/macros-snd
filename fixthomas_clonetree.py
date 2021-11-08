# code provided da thomas to solve type problem
import ROOT

ROOT.gInterpreter.ProcessLine('typedef double Double32_t') 

import os

inputfile = ROOT.TFile.Open("sndLHC.Genie-TGeant4.root") #using GENIE simulation just as example

# inputfile = ROOT.TFile.Open('run1/sndLHC.Ntuple-TGeant4.root')

tree = inputfile.cbmsim

fout  = ROOT.TFile('selectEvents.root','recreate')

ctree = tree.CloneTree(0)

nEvents=0

#

for n in range(20):


     rc = tree.GetEvent(n)

     if tree.MCTrack.GetEntries()>5:   # or any other selection

        rc = ctree.Fill()

        nEvents+=1

#storing output of test file
fout.cd()
ctree.Write()
fout.Close()

