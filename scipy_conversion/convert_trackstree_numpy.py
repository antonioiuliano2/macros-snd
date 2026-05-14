import ROOT as r
import pandas as pd
import numpy as np

# Define helper function in C++ to extract the updated RDataFrame with the new columns
r.gInterpreter.Declare("""
ROOT::RDF::RNode PreprocessTracks(ROOT::RDataFrame df) {
    auto getFirst = [](const ROOT::RVec<int>& vec, const ROOT::RVec<int>& mcTrack) {
        ROOT::RVec<int> res;
        //start loop over the vec to copy values
        for (size_t i = 0; i < vec.size(); i += 2) {
            res.push_back(vec[i]);

            if (i < vec.size() -1) {
            //the second variable is always 0, we need to know if the MCTrack is the same or it changed
             if (mcTrack[i+1] == mcTrack[i]) res.push_back(vec[i]);
             else {
              //the particle changed, we need to check if the next one is the same as the current one
              if (i < vec.size() - 2){

               if (mcTrack[i+1] == mcTrack[i+2]) res.push_back(vec[i+2]);
               else res.push_back(vec[i]);

              } //end if we have arrived to the second to last element
              else res.push_back(vec[i]);
             }

            } //end if we have arrived to the last element
         } //end for loop over vector
        return res;
    };
    // Here in C++ s.eVid and s.eAid can be recognized
    return df.Define("s_PdgCode", getFirst, {"s.eVid[2]","s.eMCTrack"}).Define("s_MotherId", getFirst, {"s.eAid[2]","s.eMCTrack"});
}
""")

# In Python
df_raw = r.RDataFrame("tracks", "b000021.0.0.97.trk.root")

df_processed = r.PreprocessTracks(df_raw)

#convert into a pandas dataframe
df = pd.DataFrame(df_processed.AsNumpy(columns = ["s.eID","s.ePID","s.eX","s.eY","s.eZ","s.eTX","s.eTY","s.eMCTrack","s.eP","s.eFlag","s_PdgCode","s_MotherId","trid"]))
#renameing them to remove the s.e prefix
df.columns = ["ID","PID","X","Y","Z","TX","TY","MCTrack","P","Flag","PdgCode","MotherId","trid"]
#explode into 1D (i.e. each row is a segment, not a track)
#everything except trid, which is already one entry per track
df_exploded = df.explode(["ID","PID","X","Y","Z","TX","TY","MCTrack","P","Flag","PdgCode","MotherId"])
