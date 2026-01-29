import ROOT as r
import fedrarootlogon
import Fedra2sndsw as EmuConv

r.gGeoManager.Import("root:://eosuser.cern.ch//eos/user/a/aiulian/sim_snd/numu_sim_activeemu_withcrisfiles_25_July_2022/1/geofile_full.Genie-TGeant4.root")
trackfile = "root:://eosuser.cern.ch//eos/user/a/aiulian/sim_fedra/scanning_reference_numu_sim_activeemu_withcrisfiles_25_July_2022/b000031/b000031.0.0.0.trk.root"
#needed initializations for ReadTracksTree
dproc = r.EdbDataProc()
ali = r.EdbPVRec()
scancond = r.EdbScanCond()
ali.SetScanCond(scancond)

itrack = 0
brickID = 31

dproc.ReadTracksTree(ali,trackfile,"trid=={}".format(itrack))
#in this case, the list of tracks is only one track
track = ali.eTracks[0]


#read track, applying conversion

globaltrackarr = EmuConv.converttrack(track,brickID)

iseg = 0
for x,y,z,tx,ty,tz in globaltrackarr:
 segment = track.GetSegment(iseg)
 print("Original segment")
 print("MCEvent ", segment.MCEvt(), "MCtrack ",segment.MCTrack(), "Plate ",segment.Plate())
 print("positions: ", segment.X(), segment.Y(), segment.Z())
 print("angles: ",segment.TX(), segment.TY(), 1)
 print("Converted segment")
 print("positions: ",x,y,z)
 print("angles: ",tx/tz,ty/tz)

 iseg = iseg + 1
