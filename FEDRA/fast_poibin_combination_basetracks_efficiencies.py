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
     
     #get efficiency value
    
     geff = heff.CreateGraph()
     arr_eff = np.array(geff.GetY())
     #remove zeros from not existing plates
     arr_eff = arr_eff[arr_eff != 0]
     
     return arr_eff
    
#get eff array
arr_eff = GetEfficiencyArray("efficiency_plate_allquarters.root")
#calling fast Poisson Binomial Distribution module, https://github.com/privet-kitty/fast-poibin/
poibin = PoiBin(arr_eff)

#printing results
np.set_printoptions(precision=2) #for scientific notation
print("Probability of having exactly k base-tracks, starting from {}".format(min_nfilms))
#pmf is Probability Mass Function, i.e. the probability of having k successes over n trials
print(poibin.pmf[min_nfilms:]*100)
print("\n")
#cdf is Cumulative Distribution Function, i.e. the probability of having at maximum k successes
print("Probability of having at least {} base-tracks:\n{:.5}".format(min_nfilms, (1 -poibin.cdf[min_nfilms])*100))
print("\n")
#just printing computation time info
stopwatch.Stop()
stopwatch.Print()