import ROOT as r
import numpy as np

min_nfilms = 18

# note: with recent ROOT versions ROOT is even more Pythonic, see 
# https://indico.cern.ch/event/1642215/contributions/6927591/attachments/3221142/5739996/260217_ROOT_Status_and_Plans_SHiP.pdf
def GetEfficiencyArray(inputfile):
    '''open root file with hefficiency, get list of values'''

    with r.TFile.Open(inputfile) as ifile:
     heff = ifile["DATA_efficiency_total"]
     
     #get efficiency value
    
     geff = heff.CreateGraph()
     arr_eff = np.array(geff.GetY())
     #remove zeros from not existing plates
     arr_eff = arr_eff[arr_eff != 0]
     
     return arr_eff
 
def solve_with_constraints(n, k_target, p_list, max_fail=3):
    # state: (successes, consecutive_failures) -> probability
    dp = {(0, 0): 1.0}

    for i in range(n):
        new_dp = {}
        p = p_list[i]
        q = 1 - p
        
        for (k, f), prob in dp.items():
            # Scenario A: Success at trial i
            # Consecutive failures resets to 0
            state_s = (k + 1, 0)
            new_dp[state_s] = new_dp.get(state_s, 0) + prob * p
            
            # Scenario B: Failure at trial i
            # Only allowed if consecutive failures < max_fail
            if f < max_fail:
                state_f = (k, f + 1)
                new_dp[state_f] = new_dp.get(state_f, 0) + prob * q
        print("CHECK: ", new_dp)
        dp = new_dp

    # Sum all probabilities where total successes >= k_target
    total_prob = sum(prob for (k, f), prob in dp.items() if k >= k_target)
    return total_prob

# Example usage:
#get eff array
arr_eff = GetEfficiencyArray("DATA_efficiencyB21.root")
#p_list_57 = [0.7] * 57 # Replace with your actual list of 57 probabilities
result = solve_with_constraints(57, 40, arr_eff,3)
print(f"Probability: {result:.6f}")