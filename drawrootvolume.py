import ROOT as r
import sys
#usage: python -i drawrootvolume.py geofile.root
r.TGeoManager.Import(sys.argv[1])
r.gGeoManager.GetTopVolume().Draw("ogl")#interesting volumes: Wall, volTarget, Cell...
r.gGeoManager.SetVisLevel(9);
