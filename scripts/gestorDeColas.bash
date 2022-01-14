#!/bin/bash
#*****************************************************************
# File:   gestorDeColas.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   19/12/2021
# Coms:   script para lanzar el programa gestorDeColas
#*****************************************************************
MW_PORT=32029
AC_PORT=32030
cd ..
if [[ $ARCH = "sunos-sun4" ]]
then
gmake gestorDeColas
else
make gestorDeColas
fi
cd exec/
echo "===================="
echo "COMPILACIÓN EXITOSA"
echo "===================="
./gestorDeColas ${MW_PORT} ${AC_PORT}
echo "===================="
echo "EJECUCIÓN FINALIZADA"
echo "===================="