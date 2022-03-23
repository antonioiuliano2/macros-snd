import math
#doing it myself
E0 = 13.6 #MeV
x = 1 #mm
x0 = 3.5 #W radiation length

p = 4000 #MeV energy of assumed cosmics
m = 106  #muon mass

E = math.sqrt(p*p+m*m)
beta = p/E
z = 1 #muon charge in e unit

prefactor = E0/(beta * p) * z * math.sqrt(x/x0)

infactor = 1 + 0.038 * math.log(x*z*z/(x0 * beta*beta))

theta0 = prefactor * infactor

print("Estimated sigma theta0 manually with formula: ",theta0)

#employing FEDRA function
import ROOT
import fedrarootlogon

momestimator = ROOT.EdbMomentumEstimator()
dtx = 0 #other uncertainty for quadratic sum
#it does: return new TF1(name,Form("sqrt(214.3296*x/%f*((1+0.038*log(x/(%f)))**2)/([0])**2+%e)",x0,x0,dtx));
fedrasigmafunc = momestimator.MCSErrorFunction("fedrasigmafunc",x0,dtx) #just as normal, but with 14.64 instead of 13.6
fedrasigmafunc.SetParameter(0,p)
print("Estimated sigma theta0 with FEDRA function: ",fedrasigmafunc.Eval(x))


