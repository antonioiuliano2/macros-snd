#!/bin/bash
cp /home/simsndlhc/macros-snd/FEDRA/track.rootrc ./
cat track.rootrc
echo "track.rootrc has been copied from home/simsndlhc/macros-snd/FEDRA/"
read -r -s -p $"If it's ok for you press enter to continue, otherwise edit it before proceeding"
brickIDs=(11 12 13 14 21 22 23 24 31 32 33 34 41 42 43 44 51 52 53 54) #ehi, I have written an array in BASH!
for ibrick in $(seq 0 19)
 do
  echo "processing brick ${brickIDs[ibrick]}"
  cd b0000${brickIDs[ibrick]}
  echo "makescanset -set=${brickIDs[ibrick]}.0.0.0 -from_plate=60 -to_plate=1 -dz=-1315 -suff=cp.root" >> scanset.sh
  source scanset.sh
  cp ../track.rootrc ./
  ncouples=$(root -l -q ../GetEntries.C\(${brickIDs[ibrick]}\))
  if [[ "${ncouples: -1}" == "0" ]]; then
  	echo "No hits found in brick ${brickIDs[ibrick]}, skipping"
  	cd ..
  	continue
  fi
  emtra -set=${brickIDs[ibrick]}.0.0.0 -new -v=2
  ln -s b0000${brickIDs[ibrick]}.0.0.0.trk.root linked_tracks.root
  cp /home/simsndlhc/macros-snd/FEDRA/vertexing.C ./
  root -l -q vertexing.C
  cd ..
 done
