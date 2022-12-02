import matplotlib.pyplot as plt
import pandas as pd

micdf = pd.read_csv("/home/antonio/cernbox/Synched/Archivio_cronologico/Novembre_2022/scanning_table.csv")

plt.figure()

plt.plot(micdf["Week"],micdf["NScans"],"bo",markersize=10)

plt.ylabel("NFilms Scanned x week",fontsize='x-large')

plt.show()
