#*****************************************************************
# File:   Makefile
# Author: Francisco Javier Pizarro Martínez
# Date:   1/12/2021
# Coms:   Makefile para el tp-6 de PSCD
# Options:all -> compila todo
#		  clean -> limpia todos los archivos generados al compilar
#		  streaming -> compila el programa de streaming
#		  filtro -> compila el programa de filtro
#		  masterWorker -> compila el programa masterWorker
#		  gestor -> compila el programa gestorDeColas
#*****************************************************************

#---------------------------------------------------------
CC=g++
STREAMING=streaming
MASTERWORKER=masterWorker
GESTOR=gestorDeColas
FILTRO=filtro/

SOCKET_DIR=Socket
SOCKET=${SOCKET_DIR}/Socket
MONITOR_DIR=Monitor
MONITOR =${MONITOR_DIR}/monitor
COLA_DIR=BoundedQueue
COLA=${COLA_DIR}/BoundedQueue

CPPFLAGS=-I. -I${SOCKET_DIR} -O2 -std=c++11 -lsockets # Flags compilacion
LDFLAGS=-pthread # Flags linkado threads

all: streaming masterWorker gestor
monitor: ${MONITOR}.o
#----------------------------------------------------------------------------
#Para gestionar opciones de compilación según la máquina: hendrix tiene sus manías
#Descomentar la siguiente línea para compilar en hendrix
#SOCKETSFLAGS=-lsocket -lnsl
#-----------------------------------------------------------
# Streaming
# Compilacion
${STREAMING}.o: ${STREAMING}.cpp 
	${CC} -c ${CPPFLAGS} ${STREAMING}.cpp

# Linkado
streaming: ${SOCKET}.o ${STREAMING}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${STREAMING}.o -o ${STREAMING} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# masterWorker
# Compilacion
${MASTERWORKER}.o: ${MASTERWORKER}.cpp 
	${CC} -c ${CPPFLAGS} ${MASTERWORKER}.cpp

# Linkado
masterWorker: ${SOCKET}.o ${MASTERWORKER}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${MASTERWORKER}.o -o ${MASTERWORKER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# gestorDeColas
# Compilacion
${GESTOR}.o: ${GESTOR}.cpp ${COLA}.hpp ${COLA}.cpp ${MONITOR}.hpp ${MONITOR}.cpp
	${CC} -c ${CPPFLAGS} ${GESTOR}.cpp

# Linkado
gestor: ${SOCKET}.o ${GESTOR}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${GESTOR}.o -o ${GESTOR} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# SOCKETS
# Compilacion libreria de Sockets
${SOCKET}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${SOCKET}.o
#-----------------------------------------------------------
# MONITOR
# Compilación del monitor

#-----------------------------------------------------------	
# LIMPIEZA
clean:
	$(RM) ${SOCKET}.o
	$(RM) ${MONITOR}.o
	$(RM) ${STREAMING} ${STREAMING}.o
	$(RM) ${MASTERWORKER} ${MASTERWORKER}.o
	$(RM) ${GESTOR} ${GESTOR}.o
	$(RM) ${FILTRO} -r