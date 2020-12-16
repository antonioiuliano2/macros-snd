#!/usr/bin/env python 
import ROOT,os,sys,time
import argparse

filedir = '/afs/cern.ch/work/a/aiuliano/public/Generate_GENIEinput_AnnaritaSND/NeutrinoFiles/'
outdir = '/afs/cern.ch/work/a/aiuliano/public/Generate_GENIEinput_AnnaritaSND/GenieEvents/'
splinesdir = '/afs/cern.ch/work/a/aiuliano/public/Generate_GENIEinput_AnnaritaSND/SplinesTungsten/'
names = {'14':'numu', '12':'nue', '16':'nutau', '-14':'anumu','-12':'anue', '-16':'anutau'}
filenames = {'14':'NeutMuon','12':'NeutElec','16':'NeutTau_filter','-14':'AntiNeutMuon','-12':'AntiNeutElec','-16':'AntiNeutTau_filter'}
pDict = {'14':'1214','12':'1212','-14':'2214','-12':'2212','16':'1216','-16':'2216'}
pDict1 = {'14':'1014','12':'1012','-14':'2014','-12':'2012','16':'1016','-16':'2016'}

def init(): #available options
    ap = argparse.ArgumentParser(description='Run GENIE neutrino simulation')
    ap.add_argument( '--nupdg', type=str, dest='nupdg', default=None)
    ap.add_argument('-n', '--nevents', type=int, dest='nevents', default=1000)
    ap.add_argument('-p', '--process', type=str, help='which interaction process',dest='process', default=None)
    ap.add_argument('-s', '--seed', type=int, dest='seed', default=65539) #default seed in $GENIE/src/Conventions/Controls.h
    ap.add_argument('-t', '--target', type=str, help="target material", dest='target', default='tungstenTP')
    ap.add_argument('--FS', help="produce ntuples for FairShip", dest='FairShip', action="store_true")
    args = ap.parse_args()
    return args


def MakeNtuplesFairShip(nevents, nupdg, targetcode, process):
    inputfile = filedir+filenames[nupdg]+'.root'
    spline = splinesdir+names[nupdg]+'_xsec_splines.xml'
    print('Spline to use: ', spline)
    cmd = "gevgen -n "+str(nevents)+" -p "+ nupdg +" -t "+targetcode+" -e 0,5000 -f "+inputfile+","+pDict1[nupdg]+"  --cross-sections "+ spline + " --event-generator-list "+process
    print('Starting GENIE with the following command: ')
    print(cmd)
    #os.system(cmd + " > log_"+names[nupdg]+"_"+process+"_FairShip.txt")
    os.system(cmd)
    outfile = outdir+names[nupdg]+"_"+process+"_FairShip.root"
    print("Output file name: ", outfile)
    print("Converting file to standard ROOT tuple .. ")
    cmd2 = "gntpc -i gntp.0.ghep.root -f gst -o "+outfile
    #os.system(cmd2 + " > log_conversion_"+names[nupdg]+"_"+process+"_FairShip.txt")
    os.system(cmd2)
    inf = ROOT.TFile(inputfile)
    of = ROOT.TFile(outfile,"update")
    inf.Get(pDict[nupdg]).Write()
    of.Close()

    
def makeSplines(nupdg,targetcode,outdir):
    '''prepare splines'''
    outfile = outdir+names[nupdg]+"_xsec_splines.xml"
    cmd = "gmkspl -p "+nupdg+" -t "+targetcode+" -n 100 -e 5000 -o "+outfile
    os.system(cmd)    
    



if __name__ == '__main__':
    args = init()
    nupdg = args.nupdg
    nevents = args.nevents
    process = args.process
    target = args.target
    seed = args.seed
    FairShip = args.FairShip
    print('Neutrino PDG code: ', nupdg)
    print('Number of events to generate: ', nevents)
    print('Process to simulate: ', process)
    print('Target type: ', target)
    print('Seed used in this generation: ', seed)

    if nupdg==None:
        print('Please specify the neutrino type!')
        sys.exit('Aborting code')
    
    if target=='tungstenTP':
        targetcode = '1000741840[0.95],1000280580[0.03],1000290630[0.02]'
    elif target=='tungstenEOI':
        targetcode = '1000741840[0.9],1000280580[0.1]'
    else:
        print('no other cross-sections available!')
    
    if process==None:
        print('Please choice process from $GENIE/config/ListAssembler.xml')
        print('Example: CCDIS, CCQE, CC, NC, CCRES, NCRES, etc.')
        sys.exit('Aborting code')
    
    MakeNtuplesFairShip(nevents, nupdg, targetcode, process)





 
