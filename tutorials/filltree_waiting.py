'''testing if we can access a file during writing
   Usage: launch 
   python -i filltree_waiting.py
   makefile()
   while it is still running, ON ANOTHER TERMINAL, launch
   python -i filltree_waiting.py
   demonstrator()
'''
import ROOT as r
import time
import sys

docreate = int(sys.argv[1])
#filepath accessed via xROOTD, for now in lxplus
filepath = "root:://eospublic.cern.ch//eos/experiment/sndlhc/emulsionData/testxrootdaccess/testfile.root" 

global inputfile

def makefile():
 #creating TFile and TTree
 inputfile = r.TFile.Open("/eos/experiment/sndlhc/emulsionData/testxrootdaccess/testfile.root" ,"RECREATE")
 inputtree = r.TNtuple("testtree","a test tree","x:y:z")

 nevents = 1000
 savinginterval = 10 #saving every tot events
 #starting loop to fill TTree entries
 for ievent in range(nevents):
    time.sleep(1.0) #wait 1.0 seconds between each event
    #some random variables
    x = r.gRandom.Uniform(0,1)
    y = r.gRandom.Uniform(-1,1)
    z = r.gRandom.Uniform(-1,1)
    inputtree.Fill(x,y,z)
    #once every time, saving an entry
    if (ievent % savinginterval == 0):
        print(" autosaving up to entry ",ievent)
        inputtree.AutoSave()
        inputfile.SaveSelf()# to avoid opening file continuously
 inputtree.Write()
 print("All entries done, closing file")
 inputfile.Close()

def demonstrator():
 '''checking the file'''
 inputfile.ReadKeys() #reading the keys
 inputtree = inputfile.Get("testtree")
 #how many entries do we have?
 print("Currently having {} entries".format(inputtree.GetEntries()))
 #making an histogram from some of our variables
 canvas = r.TCanvas()
 inputtree.Draw("y:x","","*")
 #checkfile = input("file opened, 0 to exit, another button to check it")
 inputtree.Delete() #need to delete the object, otherwise it will not be updated for the new readout
 return canvas

if (docreate):
   makefile()

else:
   inputfile = r.TFile.Open(filepath,"READ")
