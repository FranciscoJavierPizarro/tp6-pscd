#!/bin/bash
#*****************************************************************
# File:   masterWorker.bash
# Author: Francisco Javier Pizarro Martínez
# Date:   19/12/2021
# Coms:   script para lanzar el programa masterWorker
#*****************************************************************
STREAM_PORT=4000
IP_STREAM=127.0.0.1
GESTOR_PORT=4440
IP_GESTOR=127.0.0.1
cd ..
make masterWorker
./masterWorker ${STREAM_PORT} ${IP_STREAM} ${GESTOR_PORT} ${IP_GESTOR} >salida.txt
