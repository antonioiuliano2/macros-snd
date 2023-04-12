#!/bin/bash
#usage: source checkalignmentdate platelast platefirst. Check which plates have last (local) alignment before launching
BRICKID=21
# Basic if statemen
for i in $(seq $2 $1)

  do
    echo "alignment date for "$BRICKID.$i.0.0_$BRICKID.$(($i-1)).0.0.aff.par  
    date -r AFF/$BRICKID.$i.0.0_$BRICKID.$(($i-1)).0.0.aff.par      
      
done
