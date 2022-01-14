#!/bin/bash
#*****************************************************************
# File:   analizadores.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   24/12/2021
# Coms:   script para lanzar los programas analizadores
#*****************************************************************
GESTOR_PORT=32030
IP_GESTOR=155.210.152.184
cd ..
if [[ $ARCH = "sunos-sun4" ]]
then
gmake analizadorTags
gmake analizadorRendimiento
else
make analizadorTags
make analizadorRendimiento
fi
cd exec/
echo "===================="
echo "COMPILACIÓN EXITOSA"
echo "===================="
./analizadorRendimiento ${GESTOR_PORT} ${IP_GESTOR}
./analizadorTags ${GESTOR_PORT} ${IP_GESTOR}
echo "===================="
echo "EJECUCIÓN FINALIZADA"
echo "===================="
