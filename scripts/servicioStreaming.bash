#!/bin/bash
#*****************************************************************
# File:   servicioStreaming.bash
# Author: Axel IsaacOrtega Pazmiño Ortega y Alicia Lázaro Huerta
# Date:   19/12/2021
# Coms:   script para lanzar el servicio de streaming
#*****************************************************************
F_INPUT=tweets-sinProcesar.csv
F_OUTPUT=tweets-filtrados.csv
STREAM_PORT=32028
if [ ! -s "${F_OUTPUT}" ]
then 
./ejecutarFiltro.bash
fi
cd ..
if [[ $ARCH = "sunos-sun4" ]]
then
gmake streaming
else
make streaming
fi
cd exec/
echo "===================="
echo "COMPILACIÓN EXITOSA"
echo "===================="
if [ -s "${F_OUTPUT}" ]
then
./streaming ${STREAM_PORT}
echo "===================="
echo "EJECUCIÓN FINALIZADA"
echo "===================="
else
echo "INTRODUZCA " ${F_INPUT} " Y EJECUTE ESTE SCRIPT DE NUEVO"
fi