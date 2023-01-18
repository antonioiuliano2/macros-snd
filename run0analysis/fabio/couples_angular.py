import ROOT as r
import numpy as np
import fedrarootlogon

from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-q", "--quadrant", dest="quadrant", type=int,required=True)
parser.add_argument("-p", "--plate", dest="plate", type=int,required=True)
options = parser.parse_args()

quadDict = {1: 'first', 2: 'second', 3: 'third', 4: 'fourth'}
if options.quadrant>0:
 quadrant = quadDict[options.quadrant]+'_quadrant/b000431/' 
else:
 quadrant = ''
if options.plate>9:
 plate = str(options.plate)
else:
 plate = '0'+str(options.plate)

path = '/home/scanner/sndlhc/RUN0/b000431/'+str(quadrant)+'p0'+plate
cpFile = path+'/431.31.0.0.cp.root'
r.gStyle.SetOptStat(0)
f = r.TFile.Open(cpFile)
couples = f.couples


c = r.TCanvas('c','couples angular distribution', 800, 800)
h = r.TH2D('h', 'h; tan#theta_{xz}; tan#theta_{yz}', 100, -0.1, 0.1, 100, -0.1, 0.1)
couples.Draw("s.eTY:s.eTX>>h", "eCHI2P<2.4&&s.eW>20&&eN1<=1&&eN2<=1&&s1.eFlag>=0&&s2.eFlag>=0", "COLZ")
c.SetLogz()
c.Update()
c.Print(path+'/angle_cp_zoom.png')
