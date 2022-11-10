import ROOT as r
import fedrarootlogon

TEST_POSD_path = "/home/scanner/sndlhc/TEST_POSD/b000001"

#couplesfile = r.TFile.Open(TEST_POSD_path+"p019/31.19.0.0.cp.root")
#couples = couplesfile.Get("couples")

couples = r.EdbCouplesTree()
couples.InitCouplesTree("couples",TEST_POSD_path+"/p001/1.1.0.0.cp.root","READ")
couples.eCut = "eCHI2P<2.4&&s.eW>20&&eN1<=1&&eN2<=1&&s1.eFlag>=0&&s2.eFlag>=0"
#couples used in track reconstruction
#how many entries above the cut?
lst = couples.InitCutList()
nlst =lst.GetN()

print('We have {} good couples'.format(nlst))


#couplesList = couples.InitCutList()

hxy = r.TH2D('hxy', '2d position distribution;x[mm];y[mm]', 190, 0, 190, 190, 0, 190)
htxty = r.TH2D('htxty','2D angular distribution;TX;TY',300,-1.5,1.5,300,-0.15,0.15)
htx = r.TH1D('htx','1D angular distribution;TX',300,-0.15,0.15)
hty = r.TH1D('hty','1D angular distribution;TY',300,-0.15,0.15)
htheta = r.TH1D('htheta','1D Theta distribution;#Theta[rad]',20,0.,0.2)

#nList = couplesCList.GetN()


for ilst in range(nlst):
    entr = lst.GetEntry(ilst)
    couples.GetEntry(entr)
    seg = couples.eS
    hxy.Fill(seg.X()/1000., seg.Y()/1000.)
    htxty.Fill(seg.TX(),seg.TY())
    htx.Fill(seg.TX())
    hty.Fill(seg.TY())
    htheta.Fill(r.TMath.ATan(seg.Theta()))
    #print(seg.X(), seg.Y(), seg.TX(), seg.TY())

cxy = r.TCanvas('cxy', 'xy position', 800, 800)
hxy.Draw('COLZ')
ctxty = r.TCanvas('ctxty', '2D angular', 800, 800)
htxty.Draw('COLZ')
ctheta = r.TCanvas('ctheta', 'theta angle', 800, 600)
htheta.Draw()
c1Dtxty = r.TCanvas('c1Dtxty', '1D angular', 800, 1200)
c1Dtxty.Divide(1, 2)
c1Dtxty.cd(1)
htx.Draw()
c1Dtxty.cd(2)
hty.Draw()
