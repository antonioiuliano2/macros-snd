#!/bin/bash/

#pdftk $1 dump_data output metadatapdf.txt #to create metadatapdf.txt

pdftk $1 update_info metadatapdf.txt output $1"_labelled.pdf"

mv $1"_labelled.pdf" $1