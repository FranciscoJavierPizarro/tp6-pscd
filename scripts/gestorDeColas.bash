#!/bin/bash
#*****************************************************************
# File:   gestorDeColas.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   19/12/2021
# Coms:   script para lanzar el programa gestorDeColas
#*****************************************************************
MW_PORT=31052
AC_PORT=31053
cd ..
if [[ $ARCH = "sunos-sun4" ]]
then
gmake -f Makefile_hendrix gestor
else
make gestor
fi
echo "==================="
echo "COMPILACIÓN EXITOSA"
echo "==================="
./gestorDeColas ${MW_PORT} ${AC_PORT}
