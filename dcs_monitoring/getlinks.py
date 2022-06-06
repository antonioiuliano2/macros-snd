'''checks what files are available and print the link to them'''
import os

filelist = os.listdir("/eos/experiment/sndlhc/www/online/")


prefix = '<li> <a href="https://snd-lhc-monitoring.web.cern.ch/online/run.html?file='

postfix = '"> '
endstring =' </a>'
filelist = sorted(filelist, reverse = True)
for filename in filelist:
 if (filename[:10] == 'graphs_dcs'):
  print("\t"+prefix + filename + postfix + filename[-11:] + endstring)

