#!/bin/bash
#usage: source checklinkingdate platelast platefirst 
BRICKID=21
# Basic if statement
for i in $(seq $2 $1)

  do

  if [ $i -le 9 ]
      
      then
      
      echo ultimo linking piatto p00$i
      date -r p00$i/$BRICKID.$i.0.0.cp.root      
      
  else

      echo ultimo linking piatto p0$i
      date -r p0$i/$BRICKID.$i.0.0.cp.root

  fi
done
