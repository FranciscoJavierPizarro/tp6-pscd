#!/bin/bash
#*****************************************************************
# File:   masterWorker.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   19/12/2021
# Coms:   script para lanzar el programa masterWorker
#*****************************************************************
STREAM_PORT=32028
IP_STREAM=155.210.152.183
GESTOR_PORT=32029
IP_GESTOR=155.210.152.184
cd ..
if [[ $ARCH = "sunos-sun4" ]]
then
gmake -f Makefile_hendrix masterWorker
else
make masterWorker
fi
cd exec/
echo "===================="
echo "COMPILACIÓN EXITOSA"
echo "===================="
./masterWorker ${STREAM_PORT} ${IP_STREAM} ${GESTOR_PORT} ${IP_GESTOR}
echo "===================="
echo "EJECUCIÓN FINALIZADA"
echo "===================="
