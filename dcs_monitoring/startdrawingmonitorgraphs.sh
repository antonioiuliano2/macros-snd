#!/bin/bash
#update canvas with SMS graphs every hour
while true;
 do
  root -l -q drawmonitorgraphs.C\(\"$1\"\)
  echo "Updated graph file"
  sleep 3600
 done
