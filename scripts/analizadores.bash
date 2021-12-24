#!/bin/bash
#*****************************************************************
# File:   analizadores.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   24/12/2021
# Coms:   script para lanzar los programas analizadores
#*****************************************************************
GESTOR_PORT=4443
IP_GESTOR=127.0.0.1
cd ..
make analizadorTags
make analizadorRendimiento
./analizadorRendimiento ${GESTOR_PORT} ${IP_GESTOR} &
./analizadorTags ${GESTOR_PORT} ${IP_GESTOR}
