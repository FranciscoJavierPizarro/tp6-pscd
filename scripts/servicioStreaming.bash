#!/bin/bash
#*****************************************************************
# File:   servicioStreaming.bash
# Author: Axel IsaacOrtega Pazmiño Ortega y Alicia Lázaro Huerta
# Date:   19/12/2021
# Coms:   script para lanzar el servicio de streaming
#*****************************************************************
F_INPUT=tweets-sinProcesar.csv
F_OUTPUT=tweets-filtrados.csv
STREAM_PORT=31051
cd ..
if [[F_OUTPUT -ne]]
then
mkdir filtro
flex filtro.l
mv lex.yy.c filtro/
cd filtro
gcc -lfl lex.yy.c -o filtro
./filtro <../${F_INPUT} >../${F_OUTPUT}
cd ..
fi
echo "==================="
echo " FILTRADO EXITOSO"
echo "==================="
if [[ $ARCH = "sunos-sun4" ]]
then
gmake -f Makefile_hendrix streaming
else
make streaming
fi
echo "==================="
echo "COMPILACIÓN EXITOSA"
echo "==================="
./streaming ${STREAM_PORT}
