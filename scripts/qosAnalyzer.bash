#!/bin/bash
#*****************************************************************
# File:   qosAnalyzer.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   14/01/2022
# Coms:   script para lanzar el analizador qos
#*****************************************************************
GESTOR_PORT=32030
IP_GESTOR=155.210.152.184
cd ..
if [[ $ARCH = "sunos-sun4" ]]
then
gmake analizadorRendimiento
else
make analizadorRendimiento
fi
cd exec/
echo "===================="
echo "COMPILACIÓN EXITOSA"
echo "===================="
./analizadorRendimiento ${GESTOR_PORT} ${IP_GESTOR}
echo "===================="
echo "EJECUCIÓN FINALIZADA"
echo "===================="
