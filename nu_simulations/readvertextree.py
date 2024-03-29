import uproot, awkward
import pandas as pd
import numpy as np

def correctvertex(vtxinfo):
    '''vertex tracks have same MC mother'''
    #start loop over vertices
    samemother = []
    #using condition of same event instead of same mother for now
    for (ntracks, eventids, incoming) in zip(vtxinfo["n"], vtxinfo["MCEventID"], vtxinfo["incoming"]):
        goodvertex = True
        ndaughters = 0
        #loop over tracks, comparing mothers
        for itrk in range(ntracks):
            if (incoming[itrk] == 1): #track coming from the vertex
                ndaughters = ndaughters + 1
            for jtrk in range(itrk):
             if (eventids[jtrk] != eventids[itrk] and incoming[jtrk] == incoming[itrk]): #same direction, but different mother
                goodvertex = False
        #end of loop, storing result
        if(ndaughters<2):
            goodvertex = False
        samemother.append(goodvertex)
    #end of loop over vertices, returning list of results
    return samemother
#checking neutrino vertices
#at least a track with mother ID 0
def neutrinodaughters(vtxinfo,df):
    ''' checking if there is at least one neutrino daughter in the vertex'''
    aneutrinodaughter = []
    for (ntracks, incoming, eventID, eventmothers) in zip(vtxinfo["n"],vtxinfo["incoming"],vtxinfo["MCEventID"], vtxinfo["MCMotherID"]):
        hasnudau = 0 
        for itrk in range(ntracks):
            if(incoming[itrk] == 1 and eventmothers[itrk] == 0 and eventID[itrk] in df["Event"].values):
                hasnudau = hasnudau + 1
        aneutrinodaughter.append(hasnudau > 1) #at least two tracks from neutrino
    return aneutrinodaughter


def readvertextree(vertexfile,brickeventlist):
 '''main function, reading vertex tree'''


 df = pd.read_csv(brickeventlist)
 df = df.query("NBrick>0")
 #reading reconstructed vertices
 vtxfile = uproot.open(vertexfile)
 vtxtree = vtxfile["vtx"]
 vtxinfo = vtxtree.arrays(["n","MCTrackPdgCode","MCEventID","MCTrackID","MCMotherID","vx","vy","vz","vID","incoming"])

 #adding information about brickID
 brickinfo = vtxfile["brickinfo"]
 vtxinfo["brickID"] = brickinfo["brickID"].array()

 ndaughters = []
 #checking how many daughters
 for ntracks, incoming in zip(vtxinfo["n"],vtxinfo["incoming"]):
    nd = 0
    for itrk in range(ntracks):
        if(incoming[itrk]==1):
            nd = nd + 1
    ndaughters.append(nd)
 vtxinfo["ndaughters"] = ndaughters
 # we select only vertices with at least 2 daughters
 vtxinfo = vtxinfo[vtxinfo["ndaughters"]>=2]
 samemother = correctvertex(vtxinfo)
 vtxinfo["SameMother"] = samemother
 ngood = len(vtxinfo[samemother])
 nall = len(vtxinfo)
 print("{} out of {} vertices have same mother: fraction {}".format(ngood,nall,ngood/nall))


 aneutrinodaughter = neutrinodaughters(vtxinfo,df)
 nuvertices = vtxinfo[aneutrinodaughter]
 hasmuon = []
 for trackpdgs in nuvertices["MCTrackPdgCode"]:
     hasmuon.append(np.abs(13) in trackpdgs)
 nuvertices["hasmuon"] = hasmuon
 return nuvertices