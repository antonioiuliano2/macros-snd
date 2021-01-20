#!/usr/bin/env python
import ROOT,os,sys,time
import argparse
import genie_interface

defaultfiledir = '/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/Generate_GENIEinput_AnnaritaSND/NeutrinoFiles/'
defaultsplinedir = '/eos/user/a/aiuliano/public/sims_FairShip/sim_snd/Generate_GENIEinput_AnnaritaSND/SplinesTungstenTP/'
names = {14:'numu',12:'nue', 16:'nutau', -14:'anumu',-12:'anue', -16:'anutau'}
filenames = {14:'NeutMuon',12:'NeutElec',16:'NeutTau_filter',-14:'AntiNeutMuon',-12:'AntiNeutElec',-16:'AntiNeutTau_filter'}

def init(): #available options
    parser = argparse.ArgumentParser(description='Run GENIE neutrino simulation')
    subparsers = parser.add_subparsers()

    ap = subparsers.add_parser('sim', help="make genie simulation file")
    
    ap.add_argument( '--nupdg', type=str, dest='nupdg', default=None)
    ap.add_argument('-n', '--nevents', type=int, dest='nevents', default=1000)
    ap.add_argument('-f','--filedir', type=str, help="directory with neutrino fluxes", dest='filedir', default=defaultfiledir)
    ap.add_argument('-c','--crosssectiondir', type=str, help="directory with neutrino splines crosssection", dest='splinedir', default=defaultsplinedir)
    ap.add_argument('-o','--output', type=str, help="output directory", dest='outdir', default=None)
    ap.add_argument('-p', '--process', type=str, help='which interaction process',dest='process', default=None)
    ap.add_argument('-s', '--seed', type=int, dest='seed', default=65539) #default seed in $GENIE/src/Conventions/Controls.h
    ap.add_argument('-t', '--target', type=str, help="target material", dest='target', default='tungstenTP')

    ap1 = subparsers.add_parser('spline', help="make a new cross section spline file")
    ap1.add_argument('-t', '--target', type=str, help="target material", dest='target', default='tungstenTP')
    ap1.add_argument('-o', '--output',type=str, help="output directory", dest='outdir', default=None)
    args = parser.parse_args()
    return args

if __name__ == '__main__':
    args = init()
    nupdg = int(args.nupdg)

    FairShip = args.FairShip
    MakeSpline = args.MakeSpline

    print('Neutrino PDG code: ', nupdg)
    print('Number of events to generate: ', nevents)
    print('Process to simulate: ', args.process)
    print('Target type: ', args.target)
    print('Seed used in this generation: ', args.seed)

    if 'GALGCONF' not in os.environ:
        sys.exit('GALGCONF is not set to a conf folder: need to configure GENIE for SND high energies!')        

    if nupdg==None:
        print('Please specify the neutrino type!')
        sys.exit('Aborting code')
    
    if args.target=='tungstenTP':
        targetcode = '1000741840[0.95],1000280580[0.03],1000290630[0.02]'
    elif args.target=='tungstenEOI':
        targetcode = '1000741840[0.9],1000280580[0.1]'
    else:
        print('no other cross-sections available!')

    if (FairShip):

     nevents = int(args.nevents)
     if args.process==None:
        print('no process selected, generating with default GENIE processes')

     #Getting input flux and spline path
     inputfile = args.filedir+filenames[nupdg]+'.root'
     spline = args.splinedir+names[nupdg]+'_xsec_splines.xml'
     #setting path of outputfile
     outputfile = args.outdir+names[nupdg]+"_"+args.process+"_FairShip.root"
     #generating GENIE simulation
     genie_interface.generate_genie_events(nevents = nevents, nupdg = nupdg, emin = 0, emax = 5000, \
                                       targetcode = targetcode, inputflux = inputfile, \
                                        spline = spline, process = args.process, seed = args.seed)
     #converting GENIE simulation into FairShip compatible format
     genie_interface.make_ntuples("gntp.0.ghep.root", outputfile)

     #adding histograms
     genie_interface.add_hists(inputflux = inputfile, simfile = outputfile, nupdg = nupdg)

    elif (MakeSpline):
     #setting path of outputfile
     outputfile = args.outdir+names[nupdg]+'_xsec_splines.xml'
     #generating new set of splines, saving them in outputdir
     nupdglist = [nupdg]
     genie_interface.make_splines(nupdglist = nupdglist,targetcode = targetcode,emax = 5000, nknots = 100, outputfile = outputfile)
    
    else:
     print('Please choice if you want to generate events for FairShip (--FS) or a new set of splines (--MS)')
     sys.exit('Nothing done')
