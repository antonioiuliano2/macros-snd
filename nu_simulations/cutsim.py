#script to cut cbmsim tree according to given selection
import ROOT as r
import sys

r.gInterpreter.ProcessLine('typedef double Double32_t') 
#target borders
targetxmin = -47.6
targetxmax = -8.0
targetymin = 15.5
targetymax = 55.1
maxentries = 100000 #MAXIMUM NUMBER OF ENTRIES, USUALLY AS IN THE OLD TREE

simfile = r.TFile.Open(sys.argv[1])
simtree = simfile.Get("cbmsim")
#cloning the tree, without any entry
copyfile = r.TFile.Open(("inECC_"+sys.argv[1]),"RECREATE")
copytree = simtree.CloneTree(0)

nevents = simtree.GetEntries()

eventlistfile = open("inECCevents.txt","w")
#loop into events
for ievent in range(nevents):
 if(ievent%10000 == 0):
  print("Reached event", ievent)
 simtree.GetEvent(ievent)
 tracks = simtree.MCTrack
 #are vertices within ECC?
 startx = tracks[0].GetStartX()
 starty = tracks[0].GetStartY()

 if (startx >= targetxmin) and (startx <= targetxmax) and (starty >= targetymin) and (starty <= targetymax):
  copytree.Fill()
  eventlistfile.write("{}\n".format(ievent))

 if (copytree.GetEntries() > maxentries):
  print("Gone above maximum number of entries",maxentries)
  break

copytree.AutoSave()
copytree.Write()
copyfile.Close()

eventlistfile.close()
