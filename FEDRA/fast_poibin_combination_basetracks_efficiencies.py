import ROOT as r
import numpy as np
from fast_poibin import PoiBin

min_nfilms = 18

stopwatch = r.TStopwatch()
# note: with recent ROOT versions ROOT is even more Pythonic, see 
# https://indico.cern.ch/event/1642215/contributions/6927591/attachments/3221142/5739996/260217_ROOT_Status_and_Plans_SHiP.pdf
def GetEfficiencyArray(inputfile):
    '''open root file with hefficiency, get list of values'''

    with r.TFile.Open("efficiency_plate_allquarters.root") as ifile:
     heff = ifile["hexpected_clone"]

     c0 = r.TCanvas()
     heff.Draw() #Draw to create the graph
     c0.Update()
     #get efficiency value
    
     geff = heff.GetPaintedGraph()
     print(geff)
     arr_eff = np.array(geff.GetY())
     #remove zeros from not existing plates
     arr_eff = arr_eff[arr_eff != 0]
     
     return arr_eff
    
    

arr_eff = GetEfficiencyArray("efficiency_plate_allquarters.root")
poibin = PoiBin(arr_eff)

print(poibin.pmf)

stopwatch.Stop()
stopwatch.Print()