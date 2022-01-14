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
if [ ! -e "exec/filtro" ]
then
flex src/filtro.l
mv lex.yy.c bin/maquina1/
gcc -lfl bin/maquina1/lex.yy.c -o exec/filtro
fi
if [ -e ${F_INPUT} ]
then
./exec/filtro <${F_INPUT} >exec/${F_OUTPUT}
echo "===================="
echo "  FILTRADO EXITOSO  "
echo "===================="
fi