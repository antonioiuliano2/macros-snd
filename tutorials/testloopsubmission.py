unit30_Nm = 470661
nRuns = 100
dN = int(unit30_Nm/nRuns)

S = 0

print ("Testing submission of {} events in {} + 1 runs".format(unit30_Nm, nRuns))

runcounter = 0
while S <  unit30_Nm:

   if (S > (unit30_Nm - dN)):
       dN = unit30_Nm - S

   cmd = "-n "+str(dN)+" -i "+str(min(S, unit30_Nm))

   S +=dN
   runcounter += 1

   print (cmd)

print("We would have submitted {} jobs".format(runcounter))
