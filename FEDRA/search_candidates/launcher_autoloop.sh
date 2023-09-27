#!/bin/bash

root -l <<EOF
.L auto_loop_EDA_vertex.C
prepare_vertex_rec()
savepics()
nextentry()
savepics()
nextentry()
savepics()
.q
EOF