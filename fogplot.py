import ROOT as r
import sys
#plofile plots, averaging over all points
r.gStyle.SetOptStat(0)
botfog = r.TProfile("botfog","Fog plot of bot layer;z[#mum];fog(1/1000#mum^3)",40,150,550,0.,50.)#19
topfog = r.TProfile("topfog","Fog plot of top layer;z[#mum];fog(1/1000#mum^3)",40,150,550,0.,50.)#19

#botfog = r.TProfile("botfog","Fog plot of bot layer;z[#mum];fog(1/1000#mum^3)",40,150,559,0.,50.)#49
#topfog = r.TProfile("topfog","Fog plot of top layer;z[#mum];fog(1/1000#mum^3)",40,150,559,0.,50.)#49

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

cfogall = r.TCanvas()
#cfogall.Divide(2,1)
#cfogall.cd(1)
botfog.Draw("hist C*")
#cfogall.cd(2)
topfog.Draw("hist C* SAME")

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
