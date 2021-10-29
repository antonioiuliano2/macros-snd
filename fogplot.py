import ROOT as r
import sys
#plofile plots, averaging over all points
botfog = r.TProfile("botfog","Fog plot of bot layer;z[#mum];fog(1/1000#mum^3)",15,150,300,0.,50.)
topfog = r.TProfile("topfog","Fog plot of top layer;z[#mum];fog(1/1000#mum^3)",15,400,550,0.,50.)

endoffile = False

filepath = sys.argv[1]

ntotview = 0

with open(filepath,"r") as fogfile:
 while (endoffile == False): #reading file continuously
  viewheader = fogfile.readline()
  try:
   nlinesview = int(viewheader.split()[0])
    
   for iline in range(nlinesview):
    viewdata = fogfile.readline().split()
    if (float(viewdata[0]) < 300.):
     botfog.Fill(float(viewdata[0]),float(viewdata[3])*1e+3)
    else:
     topfog.Fill(float(viewdata[0]),float(viewdata[3])*1e+3)
   ntotview = ntotview+1/2.
  except IndexError: #we have reached end of file
   endoffile = True

print("we have in total {} views".format(int(ntotview)))
cbot = r.TCanvas()

botfog.Draw("hist C*")


ctop = r.TCanvas()

topfog.Draw("hist C*")

#graphs for singlew view
botviewgraph = r.TGraph()
topviewgraph = r.TGraph()

cbotview = r.TCanvas()
ctopview = r.TCanvas()

def plotview(myview):
 ''' reading view number myview'''
 #clearing graphs
 botviewgraph.Clear()
 topviewgraph.Clear()

 with open(filepath,"r") as fogfile:

  iview = 0
  ipointbot = 0
  ipointtop = 0 
  while (iview <= myview):
   #reading view bot
   viewheader = fogfile.readline()

   nlinesview = int(viewheader.split()[0])
   for iline in range(nlinesview):
    viewdata = fogfile.readline().split()
    if (iview==myview): #plotting only points of view equal to the desired one myview
      botviewgraph.SetPoint(ipointbot, float(viewdata[0]),float(viewdata[3])*1e+3)
      ipointbot+=1
   #reading view top
   viewheader = fogfile.readline()

   nlinesview = int(viewheader.split()[0])
   for iline in range(nlinesview):
    viewdata = fogfile.readline().split()
    if (iview==myview): #plotting only points of view equal to the desired one myview
      topviewgraph.SetPoint(ipointtop, float(viewdata[0]),float(viewdata[3])*1e+3)
      ipointtop+=1
    
   iview += 1 

 cbotview.cd()
 botviewgraph.SetTitle("Fog plot of bot layer for view {};z[#mum];fog(1/1000#mum^3)".format(myview))
 botviewgraph.Draw("AP*")


 ctopview.cd()
 topviewgraph.SetTitle("Fog plot of top layer for view {};z[#mum];fog(1/1000#mum^3)".format(myview))
 topviewgraph.Draw("AP*")
 

plotview(1)
