import uproot
import numpy as np
import matplotlib.pyplot as plt
import argparse #for arguments
#input parameters

def init(): #available options  

  ap = argparse.ArgumentParser(
      description='Predict number of interacting neutrinos in SND@LHC')
  ap.add_argument('-s','--xsecspline'    , type=str, help="File ROOT with Genie splines", dest='xsecfile', required = True)
  ap.add_argument('-f', '--inputfile', type=str, help="FLUKA distribution of incoming neutrinos", dest='nuspectrum', required = True)
  ap.add_argument('-n', '--nuflavour', type = str, help = "Neutrino flavour name (nu_e,nu_e_bar,nu_mu,nu_mu_bar,nu_tau,nu_tau_bar",required = True)
  args = ap.parse_args()
  return args


args = init() #getting options

#opening files
xsec = uproot.open(args.xsecfile)
input_file = uproot.open(args.nuspectrum)
if ('nu_tau' in args.nuflavour):
 norm = 31110650 #for tau
else:
 norm = 214285714 #for electron and muons

print("Predicting yield for ",args.nuflavour," with normalization ",norm)
#cross section

# elettronici
ccp = xsec[args.nuflavour+"_W184/dis_cc_p"]
ccn = xsec[args.nuflavour+"_W184/dis_cc_n"]

energies_ccp = ccp.member("fX")
xsecs_ccp = ccp.member("fY")
energies_ccn = ccn.member("fX")
xsecs_ccn = ccn.member("fY")

#Interpolo le sezioni d'urto con num punti nell'intervallo...

num = 100
min_int = 0.001
max_int = 5000
logspace = np.logspace(np.log10(min_int), np.log10(max_int), num, endpoint=True)
#logspace = np.linspace((min_int), (max_int), num, endpoint=True)

# interpolazione
int_ccp = np.interp(logspace, energies_ccp, xsecs_ccp * 10)
int_ccn = np.interp(logspace, energies_ccn, xsecs_ccn * 10)

# moltiplico le sezioni d'urto per 10 poiché le voglio in fb. Nel file sono in 10^-38 cm^2 = 10^-14 bn = 10 fb


# visualizzo l'interpolazione
plt.plot(energies_ccn, xsecs_ccn * 10, color="indianred", label="dis_ccn el")

plt.plot(
    logspace,
    int_ccn,
    color="grey",
    linestyle="-",
    drawstyle="steps-mid",
    label="dis_ccn el",
)

plt.plot(
    logspace,
    int_ccn,
    color="grey",
    linestyle="",
    drawstyle="steps-mid",
    label="dis_ccn el interpolazione",
    marker="o",
)
plt.title("dis cc interactions off neutrons")
plt.legend()
plt.xlim([100, 9000])
plt.ylabel("Cross section [fb]")
plt.xlabel("Energy [Gev]")
plt.xscale("log")
plt.show()

# elettronici
plt.plot(
    logspace,
    int_ccp,
    color="indianred",
    # markersize=1,marker='o',
    linestyle="-",
    drawstyle="steps-mid",
    label="dis_ccp el",
)
plt.plot(
    logspace,
    int_ccn,
    color="teal",
    # markersize=1,marker='o',
    linestyle="-",
    drawstyle="steps-mid",
    label="dis_ccn el",
)

plt.legend()
plt.xlabel('E [Gev]')
plt.xlim([100, 9000])
#plt.xlim([370,380])
#plt.ylim([0.01,0.5])
plt.xscale("log")


#spectrum nue
el = input_file["t"]
kin = el["Ekin"].array()
x, y = el["x"].array(), el["y"].array()
weight = el["weight"].array()

kin = kin[
    np.logical_and(
        np.logical_and(x < -8.0, x > -47.6),
        np.logical_and(y < 55.1, y > 15.5),
    )
]

x1 = x[
    np.logical_and(
        np.logical_and(x < -8.0, x > -47.6),
        np.logical_and(y < 55.1, y > 15.5),
    )
]

y1 = y[
    np.logical_and(
        np.logical_and(x < -8.0, x > -47.6),
        np.logical_and(y < 55.1, y > 15.5),
    )
]

weight = weight[
    np.logical_and(
        np.logical_and(x < -8.0, x > -47.6),
        np.logical_and(y < 55.1, y > 15.5),
    )
]

# spettro elettronici

values = plt.hist(
    kin, bins=logspace, weights=weight * norm,
    color="white", edgecolor="steelblue", histtype="step", linestyle="--", label="anti-e", log=True,
)

weight_1 = weight[weight == 1]
kin_1 = kin[weight == 1]
weight_not1 = weight[weight != 1]
kin_not1 = kin[weight != 1]

plt.xlabel("E [Gev]")
plt.ylabel("Counts")
# plt.xscale('linear')
plt.xscale("log")
plt.yscale("log")

plt.xlim([5, 10000])
#plt.xlim([370,380])
#plt.ylim([10 ** 8, 3 * 10 ** 10])
#plt.yscale('linear')
# plt.ylim([10**4,10**12])
# plt.xlim([50,1000])
plt.grid("True")

plt.title("Spettro")
plt.show()

#average values and number of neutrinos

# calcolo il valore medio in questo modo: il numero di neutrini compresi nel range energetico [a,b] fa da peso
# per l'energia (a+b)/2



en_spectrum = (values[1][0:-1] + values[1][1:]) / 2
averagE = (en_spectrum * values[0]).sum() / values[0].sum()
print( 'E media al bersaglio' , averagE )
print( 'Numero al bersaglio' , values[0].sum() )


# length = 295 mm = 29,5 cm
# Area = (39.6 cm)^2 = 1568.16 cm^2
# Area = 1474.56 cm^2 = 1474.56 * 10**24 fb
# density = 19,3 g/cm^3
# L_int = [Spettro/(8*10**13)*]/*(1/1568.16)*(29.5)*(6.022*10**23)
# A = 184
# Mass = 800 kg

#const = 8 * (10 ** 13)
#const=1

# sono i due modi di calcolare il fattore, con la massa o con la densità
# verificavo che fossero identici i risultati
const2 = norm * 830 * (1/184) * (1/1474.56) * (6.022 * 10**23) *(10**-39) * 1000
const3 = norm * (1/184) * (6.022*10**23) * (10**-39) * 5.9 * 19.3 * 5


#print(const2,const3)
#const2 = 800*(1/184)*(1/1474.56)*(6.022*10**23)*(10**-24)

logspace2= (logspace[0:-1] + logspace[1:])/2
logspace3= logspace[1:] - logspace[0:-1]

n, bins, patches = plt.hist(
    kin,
    bins=logspace2,
    weights=weight * const2,
    color="white",
    edgecolor="steelblue",
    histtype="step",
    linestyle="-",
    label="e",
    log=True,
)

plt.xlabel("E [Gev]")
plt.ylabel("L_int [fb^-1]")
plt.xscale("log")
plt.grid("True")

#plt.xlim([370, 380])
#plt.yscale('linear')
#plt.xscale('linear')
#plt.ylim(0.00015,0.00018)

plt.legend()
plt.show()

#start actual convolution

def convolve(xsec, values): 
    print((np.array(xsec) * np.array(values)).sum())
    return (np.array(xsec) * np.array(values))

print('\nNumero interazioni con protoni')
conv_p = convolve(int_ccp[1: -1], n)

print('\nNumero interazioni con neturoni')
conv_n = convolve(int_ccn[1: -1], n)

plt.plot(logspace[1: -1], 
         conv_p + conv_n,                 # sommo i contributi delle interazioni da protoni e neutroni
         drawstyle = 'steps-mid', color = 'indianred', linestyle = '-',
         label = '$\N{greek small letter nu}_e$')


# valori medi energia cinetica

kin_mean = ( logspace[1: -1] * (conv_p + conv_n) ).sum()/(conv_p + conv_n).sum()
print('energia media     neutrini interagenti', kin_mean)

