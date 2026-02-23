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
 
import pandas as pd

def compute_trial_evolution(n, k_target, p_list, max_fail=3):
    # state: (successes, current_consecutive_failures) -> probability
    dp = {(0, 0): 1.0}
    evolution_data = []

    for i in range(n):
        new_dp = {}
        p = p_list[i]
        q = 1 - p
        
        for (k, f), prob in dp.items():
            # Scenario: Success
            state_s = (k + 1, 0)
            new_dp[state_s] = new_dp.get(state_s, 0) + prob * p
            
            # Scenario: Failure (Only allowed if < max_fail)
            if f < max_fail:
                state_f = (k, f + 1)
                new_dp[state_f] = new_dp.get(state_f, 0) + prob * q
        
        dp = new_dp
        
        # Calculate stats for this trial
        current_alive_prob = sum(dp.values())
        avg_successes = sum(k * prob for (k, f), prob in dp.items()) / current_alive_prob
        
        evolution_data.append({
            "Trial": str(i + 1).zfill(2),
            "Survival_Prob": round(current_alive_prob, 4),
            "Avg_Successes": round(avg_successes, 2),
            "Prob_of_Dead_Paths": round(1 - current_alive_prob, 4)
        })

    # Final filter for k_target
    final_prob = sum(prob for (k, f), prob in dp.items() if k >= k_target)
    
    return pd.DataFrame(evolution_data), final_prob

# Example: 57 trials, target 40, varied p_list
arr_eff = GetEfficiencyArray("DATA_efficiencyB21.root")
print(arr_eff)
df_evolution, final_result = compute_trial_evolution(57, 40, arr_eff)
#df_evolution, final_result = compute_trial_evolution(57, 40, [0.75]*57)
print(df_evolution.head(57)) # Showing all steps
print(f"\nFinal Probability (>=40 successes & No 4-Fail Streaks): {final_result:.6f}")