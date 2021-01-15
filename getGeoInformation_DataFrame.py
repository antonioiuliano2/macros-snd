#!/usr/bin/env python 
#prints z-coordinators of SHiP detector volumes
#WARNING: printing the entire geometry takes a lot of time
#24-02-2015 comments to EvH

from __future__ import print_function, division
from builtins import range
import operator, sys
from argparse import ArgumentParser
from array import array
import os,ROOT
import numpy as np
import pandas as pd

pd.options.display.max_rows = 9999 #we want to display EVERYTHING

#list to store information
names = []
dx = []
dy = []
dz = []
zC = []
xstart = []
xend = []
ystart = []
yend = []
zstart = []
zend = []
material = []
cubicmeter = []
weight = []


def local2Global(n):
    Info={}
    nav = ROOT.gGeoManager.GetCurrentNavigator()
    nav.cd(n)
    Info['node'] = nav.GetCurrentNode()
    Info['path'] = n
    tmp = Info['node'].GetVolume().GetShape()
    Info['material'] = Info['node'].GetVolume().GetMaterial().GetName()
    if options.moreInfo:
     x = ROOT.gGeoManager.GetVerboseLevel()
     ROOT.gGeoManager.SetVerboseLevel(0)
     Info['weight']=Info['node'].GetVolume().Weight() # kg
     Info['cubicmeter']=Info['node'].GetVolume().Capacity()/1000000. # 
     ROOT.gGeoManager.SetVerboseLevel(x)
    o = [tmp.GetOrigin()[0],tmp.GetOrigin()[1],tmp.GetOrigin()[2]]
    Info['locorign'] = o
    local = array('d',o)
    globOrigin  = array('d',[0,0,0])
    nav.LocalToMaster(local,globOrigin)
    Info['origin'] = globOrigin
    shifts = [ [-tmp.GetDX()+o[0],o[1],o[2]],
               [tmp.GetDX()+o[0],o[1],o[2]],
               [o[0],-tmp.GetDY()+o[1],o[2]],
               [o[0],tmp.GetDY()+o[1],o[2]],
               [o[0],o[1],-tmp.GetDZ()+o[2]],[o[0],o[1],tmp.GetDZ()+o[2]]
             ]
    shifted = []
    for s in shifts:
     local = array('d',s)
     glob  = array('d',[0,0,0])
     nav.LocalToMaster(local,glob)
     shifted.append([glob[0],glob[1],glob[2]])
    Info['boundingbox']={}
    for j in range(3):
     jmin = 1E30
     jmax = -1E30
     for s in shifted:
       if s[j]<jmin: jmin = s[j]
       if s[j]>jmax: jmax = s[j]
     Info['boundingbox'][j]=[jmin,jmax]
    return Info

def print_info(path, node, level, currentlevel, print_sub_det_info=False):
  sub_nodes = {}
  fullInfo = {}

  for subnode in node.GetNodes():
    name = subnode.GetName()
    fullInfo[name] = local2Global(path + '/' + name)
    sub_nodes[name] = fullInfo[name]['origin'][2]

  for name, _ in sorted(list(sub_nodes.items()), key=operator.itemgetter(1)):
    names.append(name)
    boundingbox = fullInfo[name]['boundingbox']

    dx.append(abs(boundingbox[0][0]-boundingbox[0][1])/2.)
    xstart.append(boundingbox[0][0])
    xend.append(boundingbox[0][1])

    dy.append(abs(boundingbox[1][0]-boundingbox[1][1])/2.)
    ystart.append(boundingbox[1][0])
    yend.append(boundingbox[1][1])

    dz.append(abs(boundingbox[2][0]-boundingbox[2][1])/2.)
    zstart.append(boundingbox[2][0])
    zend.append(boundingbox[2][1])
    
    zcenter = fullInfo[name]['origin'][2]
    zC.append(zcenter)
    
    material.append(fullInfo[name]['material'])

    if options.moreInfo:
      cubicmeter.append(fullInfo[name]['cubicmeter'])
      weight.append(fullInfo[name]['weight'])

    if options.volume in ["", name]:
      print_sub_det_info = True

    if print_sub_det_info and currentlevel < level and fullInfo[name]['node'].GetNodes():
      print_info(fullInfo[name]['path'], fullInfo[name]['node'], level, currentlevel + 1,
                 print_sub_det_info)

    if currentlevel == 0:
      print_sub_det_info = False


parser = ArgumentParser()
parser.add_argument("-g", "--geometry", dest="geometry", help="input geometry file",
                    required=True)
parser.add_argument("-l", "--level", dest="level", help="max subnode level", default=0)
parser.add_argument("-v", "--volume", dest="volume", help="name of volume to expand", default="")
parser.add_argument("-X", "--moreInfo", dest="moreInfo", help="print weight and capacity", default=False)

options = parser.parse_args()
fname = options.geometry
if fname.startswith('/eos/'):
    fname = os.environ['EOSSHIP'] + fname
fgeom = ROOT.TFile.Open(fname)
fGeo = fgeom.FAIRGeom
top = fGeo.GetTopVolume()


currentlevel = 0
print_info("", top, int(options.level), currentlevel)
#building dataframe
labels = ['name','z(midpoint)','dz','zstart','zend','dx','xstart','xend','dy','ystart','yend','material']
geometrydata = {'name' : np.array(names), 'z(midpoint)': np.array(zC),
                'dx' : np.array(dx), 'xstart' : np.array(xstart), 'xend' : np.array(xend),
                'dy' : np.array(dy), 'ystart' : np.array(ystart), 'yend' : np.array(yend),
                'dz' : np.array(dz), 'zstart' : np.array(zstart), 'zend' : np.array(zend),
                'material' : np.array(material)
}
df = pd.DataFrame(geometrydata,columns = labels)

if options.moreInfo:
  extralabels = ['cubicmeter','weight']
  extradata = {'cubicmeter':cubicmeter,'weight':weight}
  extradf = pd.DataFrame(extradata, columns = extralabels)
  df = pd.concat([df,extradf],axis = 1)

print(df)
