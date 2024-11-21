import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pandas as pd

plt.rcParams.update({'font.size':15})

#micdf = pd.read_csv("/home/utente/cernbox/Synched/Archivio_cronologico/Ottobre_2023/Bologna_scanningtable.csv")
#micdf = pd.read_csv("/home/utente/cernbox/Synched/Archivio_cronologico/Settembre_2023/CERN_scanningtable.csv")
#plt.figure()

#DateWeek = pd.to_datetime(micdf["Week"])

#plt.plot(DateWeek,micdf["NScans"],"bo",markersize=10)

#plt.ylabel("NFilms Scanned x week",fontsize='x-large')

#plt.ylim([0,15])

#myFmt=mdates.DateFormatter('%m-%d')
#plt.gca().xaxis.set_major_formatter(myFmt)
#assignment of runs
labplandf = pd.read_excel("/home/utente/cernbox/Synched/Archivio_cronologico/Novembre_2023/EMULSION SCANNING.xlsx",indexcol=1,usecols="A:F",nrows=5,engine="openpyxl",header=0)
runplandf = pd.read_excel("/home/utente/cernbox/Synched/Archivio_cronologico/Novembre_2023/EMULSION SCANNING.xlsx",indexcol=1,usecols="A:H",nrows=4,skiprows=7,engine="openpyxl",header=0)
#labplandf = pd.read_csv("/home/utente/cernbox/Synched/Archivio_cronologico/Ottobre_2023/Scanning_Assignment_lab.csv",index_col=False)
#setting the index (used for the axis)
runplandf=runplandf.set_index("Unnamed: 0").transpose()
labplandf=labplandf.set_index("Unnamed: 0").transpose()
#we know how many we still need to scan
runplandf.loc["RUN4","ASSIGNED"] =  runplandf.loc["RUN3","ASSIGNED"]
runplandf.loc["RUN5","ASSIGNED"] =  runplandf.loc["RUN3","ASSIGNED"]

ax = runplandf.iloc[1:,:-2].plot(kind="barh") #horizontal bars
#adding the numbers on the bars
for container in ax.containers:
    ax.bar_label(container)

ax1 = labplandf.iloc[1:,:-2].plot(kind="barh")

for container in ax1.containers:
    ax1.bar_label(container)
#display the results
plt.show()