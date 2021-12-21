#!/bin/bash
#*****************************************************************
# File:   gestorDeColas.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   19/12/2021
# Coms:   script para lanzar el programa masterWorker
#*****************************************************************
MW_PORT=4444
AC_PORT=4443
cd ..
make gestor
./gestorDeColas ${MW_PORT} ${AC_PORT}
