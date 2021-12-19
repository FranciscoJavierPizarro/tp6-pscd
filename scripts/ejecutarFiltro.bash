#!/bin/bash
#*****************************************************************
# File:   ejecutarFiltro.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   17/12/2021
# Coms:   script de ejecución del proceso filtro del tp6 de PSCD
#*****************************************************************
F_INPUT=tweets-sinProcesar.csv
F_OUTPUT=tweets-filtrados.csv
cd ..
mkdir filtro
flex filtro.l
mv lex.yy.c filtro/
cd filtro
gcc -lfl lex.yy.c -o filtro
./filtro <../${F_INPUT} >../${F_OUTPUT}