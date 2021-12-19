#!/bin/bash
#*****************************************************************
# File:   gestorDeColas.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   19/12/2021
# Coms:   script para lanzar el programa masterWorker
#*****************************************************************
MW_PORT=4440
AC_PORT=4433
cd ..
make gestor
./masterWorker ${MW_PORT} ${AC_PORT} >salida.txt
