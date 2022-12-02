import ROOT


from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-n", dest="nSeg", help="nSeg", required=True)
options = parser.parse_args()

def loadHists(histFile, query=None):
    f = ROOT.TFile.Open(histFile)
    histList = {}
    keyList = f.GetListOfKeys()
    for key in keyList:
        #if query is not None and key.GetName() not in query:
        #    continue
        hist = f.Get(key.GetName())
        #hist.SetDirectory(ROOT.gROOT)
        hist.SetName(key.GetName())
        histList[key.GetName()] = hist
    if len(histList) == 0: raise Exception('ERROR: histList is empty!')
    f.Close()
    return histList

nseg = options.nSeg
path = '/home/scanner/sndlhc/RUN0/b000431/first_quadrant/b000431/resolutions'
entriesFile = path+'/checktrackslinearfits_'+str(nseg)+'.root'
positionxFile = path+'/polfit_x_'+str(nseg)+'.root'
positionyFile = path+'/polfit_y_'+str(nseg)+'.root'
anglexFile = path+'/gausfit_angx_'+str(nseg)+'.root'
angleyFile = path+'/gausfit_angy_'+str(nseg)+'.root'

fEntries = ROOT.TFile.Open(entriesFile)
entriesTree = fEntries.restree
entries = entriesTree.GetEntries()
print('Entries: ', entries)

positionx = loadHists(positionxFile)
positiony = loadHists(positionyFile)
anglex = loadHists(anglexFile)
angley = loadHists(angleyFile)

c = ROOT.TCanvas()
positionx['c1'].Draw()
positionx['c1'].Print(path+'/pos_reso_x_'+str(nseg)+'.png')

positiony['c1_n2'].Draw()
positiony['c1_n2'].Print(path+'/pos_reso_y_'+str(nseg)+'.png')

anglex['c1'].Draw()
anglex['c1'].Print(path+'/ang_reso_x_'+str(nseg)+'.png')

angley['c1_n2'].Draw()
angley['c1_n2'].Print(path+'/ang_reso_y_'+str(nseg)+'.png')


