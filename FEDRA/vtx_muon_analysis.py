import ROOT as r
import numpy as np
import fedrarootlogon
from time import time
import os.path
from array import array


from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-b", dest="brickID", required=True, default=None, type=int)
options = parser.parse_args()

brickID = options.brickID
from_plate = 60

zmin = -77585.00


path = '/home/simsndlhc/Simulations_sndlhc/sim_fedra/muonbkg_1E5cm2'
out_dir = path

#histo setup
h_n = r.TH1D('n','Multiplicity;multiplicity', 50, 0, 50)
h_flag = r.TH1D('flag','Flag;flag', 6, 0, 6)
h_vz = r.TH1D('vz','Vertex z position;vz[um]', 400, -80000, 5000)
h_vxy = r.TH2D('vxy', 'Vertex xy map;vx[um];vy[um]', 195, 0, 195, 190, 0, 195)
h_n0 = r.TH1D('n0', 'Multiplicity;multiplicity', 47, 3, 50)
h_nseg = r.TH1D('nseg', 'Number of segments;nseg', 56, 4, 60)
h_npl = r.TH1D('npl', ' Number of crossing films;npl', 56, 4, 60)
h_ff = r.TH1D('ff', 'Fill Factor;FF', 22, 0, 1.05)
h_ip = r.TH1D('ip', 'Impact parameter;ip[um]', 100, 0, 300)
h_meanff = r.TH1D('meanff', 'Mean Fill Factor;FF', 22, 0, 1.05)
h_meanip = r.TH1D('meanip', 'Mean impact parameter;ip[um]', 100, 0, 300)
h_prob = r.TH1D('prob', 'Probability;prob', 30, 0, 1.02)
h_maxape = r.TH1D('maxape', 'Max aperture;max_ape', 50, 0, 2.5)
h_meanape = r.TH1D('meanape', 'Mean aperture;mean_ape', 50, 0, 2.5)
h_meanphi = r.TH1D('meanphi', 'Mean phi;mean_phi', 80, 0, 4)
h_maxdphi = r.TH1D('maxdphi', 'Max phi diff;max_dphi', 40, 0, 4)

N=40
M=60
start_time = time()
#reading vertices
vtx_file = path+'/b{:06d}/vertextree.root'.format(brickID)
sim_file = path+'/sndLHC.Ntuple-TGeant4-1E5cm2.root'
if os.path.isfile(vtx_file):
    #save vertices in root file
    outputFile = r.TFile(out_dir+"/vertex_muon_{}.root".format(brickID),"RECREATE")	
    outputTree = r.TTree("vtx","Tree of vertices")
    
    _brickID = array('i', [0])
    _vID = array('i', [0])
    _flag = array('i', [0])
    _vx = array('f', [0])
    _vy = array('f', [0])
    _vz = array('f', [0])
    _ntrks = array('i', [0])
    _nsegtot = array('i', [0])
    _nseg = array('i', N*[0])
    _npl = array('i', N*[0])
    _fillfact_t = array('f', N*[0])
    _fillfact = array('f', [0])
    _meanIP = array('f', [0])
    _ip = array('f', N*[0])
    _prob = array('f', [0])
    _maxaperture = array('f', [0])
    _meanaperture = array('f', [0])
    _maxdphi = array('f', [0])
    _meanphi = array('f', [0])
    _tTX = array('f', N*[0])
    _tTY = array('f', N*[0])
    _tID = array('i', N*[0])
    _tMCID = array('i', N*[0])
    _sChi2 = array('f', N*M*[0])
    _sTX = array('f', N*M*[0])
    _sTY = array('f', N*M*[0])
    _sPID = array('i', N*M*[0])
    _sID = array('i', N*M*[0])
    _MCEvt = array('i', [0])
    _evtPDG = array('f', [0])
    _trPDG = array('f', [0])
    _motherPDG = array('f', [0])
    _signal = array('i', [0])

    outputTree.Branch("brickID", _brickID, "brickID/I")
    outputTree.Branch("vID", _vID, "vID/I")
    outputTree.Branch("flag", _flag, "flag/I")
    outputTree.Branch("vx", _vx, "vx/F")
    outputTree.Branch("vy", _vy, "vy/F")
    outputTree.Branch("vz", _vz, "vz/F")
    outputTree.Branch("ntrks", _ntrks, "ntrks/I")
    outputTree.Branch("nsegtot", _nsegtot, "nsegtot/I")
    outputTree.Branch("nseg", _nseg, "nseg[ntrks]/I")
    outputTree.Branch("npl", _npl, "npl[ntrks]/I")
    outputTree.Branch("fillfact", _fillfact, "fillfact/F")
    outputTree.Branch("fillfact_t", _fillfact_t, "fillfact_t[ntrks]/F")
    outputTree.Branch("meanIP", _meanIP, "meanIP/F")
    outputTree.Branch("ip", _ip, "ip[ntrks]/F")
    outputTree.Branch("tTX", _tTX, "tTX[ntrks]/F")
    outputTree.Branch("tTY", _tTY, "tTY[ntrks]/F")
    outputTree.Branch("tID", _tID, "tID[ntrks]/I")
    outputTree.Branch("tMCID", _tMCID, "tMCID[ntrks]/I")
    outputTree.Branch("prob", _prob, "prob/F")
    outputTree.Branch("maxaperture", _maxaperture, "maxaperture/F")
    outputTree.Branch("meanaperture", _meanaperture, "meanaperture/F")
    outputTree.Branch("maxdphi", _maxdphi, "maxdphi/F")
    outputTree.Branch("meanphi", _meanphi, "meanphi/F")
    outputTree.Branch("sChi2", _sChi2, "sChi2[nsegtot]/F")
    outputTree.Branch("sTX", _sTX, "sTX[nsegtot]/F")
    outputTree.Branch("sTY", _sTY, "sTY[nsegtot]/F")
    outputTree.Branch("sPID", _sPID, "sPID[nsegtot]/I")
    outputTree.Branch("sID", _sID, "sID[nsegtot]/I")
    outputTree.Branch("MCEvt", _MCEvt, "MCEvt/I")
    outputTree.Branch("evtPDG", _evtPDG, "evtPDG/F")
    outputTree.Branch("trPDG", _trPDG, "trPDG/F")
    outputTree.Branch("motherPDG", _motherPDG, "motherPDG/F")
    outputTree.Branch("signal", _signal, "signal/I")

    print("opening file: ",vtx_file)
    dproc = r.EdbDataProc()
    gAli = dproc.PVR()
    scancond = r.EdbScanCond()
    scancond.SetSigma0(4,4,0.005,0.005) #change sigma0
    scancond.SetDegrad(4) #change angular degradation
    gAli.SetScanCond(scancond)
    vertexrec = r.EdbVertexRec()
    vertexrec.SetPVRec(gAli)
    vertexrec.eDZmax=3000.
    vertexrec.eProbMin=0.0001
    vertexrec.eImpMax=15.
    vertexrec.eUseMom=False
    vertexrec.eUseSegPar=True
    vertexrec.eQualityMode=0
    proc = r.EdbDataProc()
    dproc.ReadVertexTree(vertexrec, vtx_file, "1")
    vertices = gAli.eVTX

    f = r.TFile.Open(vtx_file)
    vtxtree = f.vtx

    ### VERTICES LOOP ###
    for ivtx, vtx in enumerate(vertices):
        vtxtree.GetEntry(ivtx)
        nu_vtx=0
        vx = vtx.VX()
        vy = vtx.VY()
        h_vxy.Fill(vx, vy)
        vz = vtx.VZ()
        flag = vtx.Flag()
        ntrks = vtx.N()
        h_vz.Fill(vz)
        if vz < zmin: continue
        if vz > 0: continue
        h_flag.Fill(flag)
        #if flag != 0 and flag != 3: continue
        h_n.Fill(ntrks)
        #if ntrks < 3: continue

        # for itrack in range(ntrks):
        #     if(vtxtree.MCMotherID[itrack] == 0):
        #         nu_vtx += 1
        # if nu_vtx<0.5*ntrks: continue
        
        h_n0.Fill(ntrks)
        nplList = []
        nsegList = []
        apeList = []
        phiList = []
        TXList = []
        TYList = []
        ffList = []
        ipList = []
        segidx = 0

        ### TRACKS LOOP ###
        DictTrackPdg = {}
        DictTrackEvt = {}
        for itrack in range(ntrks):
            track = vtx.GetTrack(itrack)
            trackID = track.MCTrack()
            h_ip.Fill(vtx.GetVTa(itrack).Imp())         
            ipList.append(vtx.GetVTa(itrack).Imp())
            nseg = track.N()
            npl = track.Npl()
            nfirst = track.GetSegmentFirst().Plate()
            nava = from_plate - nfirst + 1 
            FF = float(nseg)/float(nava)
            phi = track.Phi()
            nsegList.append(nseg)
            nplList.append(npl)
            ffList.append(FF)
            TXList.append(track.TX())
            TYList.append(track.TY())
            phiList.append(phi)
            h_nseg.Fill(nseg)
            h_npl.Fill(npl)
            h_ff.Fill(FF)
            _nseg[itrack] = nseg
            _npl[itrack] = npl
            _fillfact_t[itrack] = FF
            _ip[itrack] = vtx.GetVTa(itrack).Imp()
            _tTX[itrack] = track.TX()
            _tTY[itrack] = track.TY()
            _tID[itrack] = track.Track()
            _tMCID[itrack] = trackID
            tEvt = track.MCEvt()
            if tEvt in DictTrackEvt:
                DictTrackEvt[tEvt] += 1
            else:
                DictTrackEvt[tEvt] = 1

            ### SEGMENTS LOOP ###
            DictSegPDG = {}
            for iseg in range(nseg):
                couple = track.GetSegment(iseg)
                _sChi2[segidx] = couple.Chi2()
                _sTX[segidx] = couple.TX()
                _sTY[segidx] = couple.TY()
                _sPID[segidx] = couple.PID()
                _sID[segidx] = couple.ID()
                segidx+=1
                sPDG = couple.Vid(0)
                if sPDG in DictSegPDG:
                    DictSegPDG[sPDG] += 1
                else:
                    DictSegPDG[sPDG] = 1

            trackPDG = max(DictSegPDG, key=DictSegPDG.get)
            DictTrackPdg[trackID] = trackPDG
            # DictTrackPdg[itrack] = vtxtree.MCTrackPdgCode[itrack]
            for jtrack in range(itrack+1, ntrks):
                t2 = vtx.GetTrack(jtrack)
                tx= track.TX() - t2.TX()
                ty= track.TY() - t2.TY()
                apeList.append(r.TMath.Sqrt( tx*tx+ty*ty ))
                
        h_prob.Fill(vtx.V().prob())
        h_maxape.Fill(vtx.MaxAperture())
        h_meanff.Fill(np.mean(ffList))
        h_meanip.Fill(np.mean(ipList))
        h_meanape.Fill(np.mean(apeList))
        h_meanphi.Fill(np.mean(phiList))

        arrPhi = np.array(phiList)
        arrTX = np.array(TXList)
        arrTY = np.array(TYList)
        dPhiList = {}
        for itrack in range(ntrks):
            track = vtx.GetTrack(itrack)
            trackID = track.MCTrack()
            aor = r.TMath.ATan2(np.sum(arrTY) - arrTY[itrack], np.sum(arrTX) - arrTX[itrack])
            # aor_X = (np.sum(arrTX) - arrTX[itrack]) / (len(arrTX) - 1)
            # aor_Y = (np.sum(arrTY) - arrTY[itrack]) / (len(arrTY) - 1)
            # difference = r.TMath.ACos(arrTX[itrack]*aor_X+arrTY[itrack]*aor_Y)
            difference = r.TMath.Abs(arrPhi[itrack] - aor)
            if difference > r.TMath.Pi():
                difference = 2*r.TMath.Pi() - difference
            dPhiList[trackID] = difference
        track_maxdphi = max(dPhiList, key=dPhiList.get)
        maxdphi = dPhiList[track_maxdphi]
        h_maxdphi.Fill(maxdphi)
        fsim = r.TFile.Open(sim_file)
        cbmsim = fsim.cbmsim
        eventID = max(DictTrackEvt, key=DictTrackEvt.get)
        cbmsim.GetEntry(eventID)

        _brickID[0] = brickID
        _trPDG[0] = DictTrackPdg[track_maxdphi]
        _MCEvt[0] = eventID
        _evtPDG[0] = cbmsim.MCTrack[0].GetPdgCode()
        if track_maxdphi < len(cbmsim.MCTrack):
            motherID = cbmsim.MCTrack[track_maxdphi].GetMotherId()
            if motherID > 0:
                _motherPDG[0] = cbmsim.MCTrack[motherID].GetPdgCode()
            else:
                _motherPDG[0] = -1

        else:
            _motherPDG[0] = 10
        _vx[0]=vx
        _vy[0]=vy
        _vz[0]=vz
        _flag[0]=flag
        _vID[0] = vtx.ID()
        _ntrks[0] = ntrks
        _nsegtot[0] = segidx
        _fillfact[0] = np.mean(ffList)
        _meanIP[0] = np.mean(ipList)
        _prob[0] = vtx.V().prob()
        _maxaperture[0] = vtx.MaxAperture()
        _maxdphi[0] = maxdphi
        _meanphi[0] = np.mean(phiList)
        _meanaperture[0] = np.mean(apeList)
        _signal[0] = 0
        outputTree.Fill()
        
        fsim.Close()
    del gAli

    #write output files
    outputFile.cd()
    outputTree.Write()
    outputFile.Close()

    histoFile = r.TFile(out_dir+"/hist_out_{}.root".format(brickID), "RECREATE")
    h_n.Write()
    h_flag.Write()
    h_vxy.Write()
    h_vz.Write()
    h_n0.Write()
    h_nseg.Write()
    h_npl.Write()
    h_ff.Write()
    h_ip.Write()
    h_meanff.Write()
    h_meanip.Write()
    h_prob.Write()
    h_maxape.Write()
    h_meanape.Write()
    h_maxdphi.Write()
    h_meanphi.Write()
    histoFile.Write()
    histoFile.Close()

    end_time = time()-start_time
    print("TOTAL ELAPSED TIME ", end_time)

