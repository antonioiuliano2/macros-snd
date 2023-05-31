executable = condortraking_realign.sh
arguments = $(ClusterID) $(ProcId) $(CELL)
output = output/trackingrealign.$(ClusterId).$(ProcId).out
error = error/trackingrealign.$(ClusterId).$(ProcId).err
log = log/trackingrealign.$(ClusterId).log
+JobFlavour = "workday"
notification = Never
queue CELL from seq 1 360 |