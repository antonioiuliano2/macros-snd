import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pandas as pd

micdf = pd.read_csv("/home/utente/cernbox/Synched/Archivio_cronologico/Settembre_2023/CERN_scanningtable.csv")

plt.figure()

DateWeek = pd.to_datetime(micdf["Week"])

plt.plot(DateWeek,micdf["NScans"],"bo",markersize=10)

plt.ylabel("NFilms Scanned x week",fontsize='x-large')

plt.ylim([0,15])

myFmt=mdates.DateFormatter('%m-%d')
plt.gca().xaxis.set_major_formatter(myFmt)

plt.show()
