'''make virtual FEDRA tracks from csv of MonteCarlo segments 20 Oct 2021'''
import pandas as pd
import ROOT as r
import sys
import fedrarootlogon

dproc = r.EdbDataProc()

df = pd.read_csv(sys.argv[1])#"brick22_firstversion_Filips.csv"

simdf = df.query("MCTrack>=0") #I also exclude MCTrack -2

trackdf = simdf.groupby(["MCEvent","MCTrack"]).first()

ientry = 0
trarr = r.TObjArray() #array of tracks

trid = 0 #number of every track
#start loop over MC Tracks: building a FEDRA track for each of them
for eventID, trackID in trackdf.index:
 
 tr1 = r.EdbTrackP()

 trID = trid+1 #we need to consider different events
 
 tr1.SetM(0.139); 
 
 #selecting segments for this track
 segdf = simdf.query("MCEvent == {} and MCTrack == {}".format(eventID,trackID))
 iseg = 0
 for index,row in segdf.iterrows():
     #building segment object
     segment = r.EdbSegP(int(row["ID"]),row["x"],row["y"],row["TX"],row["TY"])
     segment.SetZ(row["z"])
     segment.SetPID(int(row["PID"]))
     segment.SetPlate(60-int(row["PID"])) 
     segment.SetMC(int(row["MCEvent"]),int(row["MCTrack"]))
     segment.SetAid(int(row["MCMotherID"]),0)
     segment.SetVid(int(row["Flag"]),0)
     if (iseg==0):
      #storing first segment information into track
      tr1.Set(int(row["ID"]),row["x"],row["y"],row["TX"],row["TY"],0,0)
      tr1.SetZ(row["z"])
      tr1.SetPID(int(row["PID"]))
      tr1.SetPlate(60-int(row["PID"])) 
      tr1.SetMC(int(row["MCEvent"]),int(row["MCTrack"]))
      tr1.SetAid(int(row["MCMotherID"]),0)
      tr1.SetVid(int(row["Flag"]),0)

     #adding segment to track
     tr1.AddSegment(segment)
     tr1.AddSegmentF(segment)
     # gAli->AddSegment(seg);
     iseg = iseg+1
     
 #end loop over segment, setting track counters and adding it to them
 tr1.SetID(int(trID))
 tr1.SetSegmentsTrack(int(trID))
 tr1.SetCounters()
 tr1.SetTrack(int(trID)) #providing trackid to eTrack so it will not be lost when trackID resets
 trarr.Add(tr1);     
 
 ientry = ientry + 1
#end track loop, buildingfile

dproc.MakeTracksTree(trarr,0,0,"FEDRAMCTracks.root")


    
