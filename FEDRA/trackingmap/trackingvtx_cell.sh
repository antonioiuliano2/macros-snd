#!/bin/bash

brickID=21

#finally doing cell by cell tracking with realign
#edit track.rootrc before launching

var1=$1
xbin=$((var1 / 19))
ybin=$((var1 % 19))

#starting from 1 to 19, not 0 to 18
xname=$((xbin + 1))
yname=$((ybin + 1))


echo "Starting tracking without realign"

emtra -set=$brickID.0.$xname.$yname -new -v=2 -ix=$xbin -iy=$ybin

mkdir trackfiles/rootfiles/57_1/cell_$xname_$yname

cp b0000$brickID.0.$xname.$yname.trk.root trackfiles/rootfiles/57_1/cell_$xname_$yname/b0000$brickID.0.$xname.$yname.trk.root 
rm b0000$brickID.0.$xname.$yname.trk.root

cp b0000$brickID.0.$xname.$yname.set.root trackfiles/parfiles/57_1/cell_$name_$yname/b0000$brickID.0.$xname.$yname.set.root
rm b0000$brickID.0.$xname.$yname.set.root 

cd trackfiles/rootfiles/57_1/cell_$name_$yname/
ln -s b0000$brickID.0.$xname.$yname.trk.root linked_tracks.root


root -l -q ~/sndlhc/macros-snd/FEDRA/vertexing.C 
