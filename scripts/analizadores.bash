#!/bin/bash
#*****************************************************************
# File:   analizadores.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   24/12/2021
# Coms:   script para lanzar los programas analizadores
#*****************************************************************
GESTOR_PORT=31053
IP_GESTOR=155.210.152.184
cd ..
if [[ $ARCH = "sunos-sun4" ]]
then
gmake -f Makefile_hendrix analizadorTags
gmake -f Makefile_hendrix analizadorRendimiento
else
make analizadorTags
make analizadorRendimiento
fi
echo "==================="
echo "COMPILACIÓN EXITOSA"
echo "==================="
./analizadorRendimiento ${GESTOR_PORT} ${IP_GESTOR} &
./analizadorTags ${GESTOR_PORT} ${IP_GESTOR}
