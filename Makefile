#*****************************************************************
# File:   Makefile
# Authors:Francisco Javier Pizarro Martínez
#		  Axel Isaac Pazmiño Ortega
#	      Alicia Lázaro Huerta
# Date:   1/12/2021
# Coms:   Makefile para el tp-6 de PSCD
# Options:all -> compila todo
#		  clean -> limpia todos los archivos generados al compilar
#		  streaming -> compila el programa de streaming
#		  masterWorker -> compila el programa masterWorker
#		  gestorDeColas -> compila el programa gestorDeColas
#		  analizadorTags -> compila el programa analizadorTags
#		  analizadorRendimiento -> compila el programa analizadorRendimiento
#*****************************************************************
UNAME := $(shell uname)
ifeq ($(UNAME), Linux) # equipos del laboratorio L1.02
	SOCKETSFLAGS= -Wall
endif
ifeq ($(UNAME), SunOS) # hendrix
	SOCKETSFLAGS= -Wall -lsocket -lnsl -lrt
endif
#---------------------------------------------------------
CC=g++
STREAMING=streaming
MASTERWORKER=masterWorker
GESTOR=gestorDeColas
TAGS_ANALIZER=analizadorTags
QOS_ANALIZER=analizadorRendimiento
SRC=src/
EXEC=exec/
SOCKET_DIR=Socket
SOCKET=${SOCKET_DIR}/Socket
MONITOR_DIR=Monitor
MONITOR =${MONITOR_DIR}/monitor
COLA_DIR=BoundedQueue
COLA=${COLA_DIR}/BoundedQueue
LISTA_DIR=LinkedList
LISTA=${LISTA_DIR}/LinkedList
SEMAPHORE_DIR=Semaphore_V4
SEMAPHORE=${SEMAPHORE_DIR}/Semaphore_V4
MWPROCESADO=MWprocesado

CPPFLAGS=-I. -I${SOCKET_DIR} -O2 -std=c++11 -lsockets# Flags compilacion
LDFLAGS=-pthread # Flags linkado threads

BIN=bin
M1=${BIN}/maquina1/
M2=${BIN}/maquina2/
M3=${BIN}/maquina3/
M4=${BIN}/maquina4/
all: streaming masterWorker gestorDeColas analizadorTags analizadorRendimiento
streaming: ${EXEC}${STREAMING}
masterWorker: ${EXEC}${MASTERWORKER}
gestorDeColas: ${EXEC}${GESTOR}
analizadorTags: ${EXEC}${TAGS_ANALIZER}
analizadorRendimiento: ${EXEC}${QOS_ANALIZER}
#-----------------------------------------------------------
# Streaming
# Compilacion
${M1}${STREAMING}.o: ${SRC}${STREAMING}.cpp 
	${CC} -c ${CPPFLAGS} ${SRC}${STREAMING}.cpp -o ${M1}${STREAMING}.o

# Linkado
${EXEC}${STREAMING}: ${M1}${SOCKET_DIR}.o ${M1}${STREAMING}.o
	${CC} ${LDFLAGS} ${M1}${SOCKET_DIR}.o ${M1}${STREAMING}.o -o ${EXEC}${STREAMING} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# masterWorker
# Compilacion
${M3}${MASTERWORKER}.o: ${SRC}${MASTERWORKER}.cpp
	${CC} -c ${CPPFLAGS} ${SRC}${MASTERWORKER}.cpp -o ${M3}${MASTERWORKER}.o

# Linkado
${EXEC}${MASTERWORKER}: ${M3}${SOCKET_DIR}.o ${M3}${MWPROCESADO}.o ${M3}${MASTERWORKER}.o
	${CC} ${LDFLAGS} ${M3}${SOCKET_DIR}.o ${M3}${MWPROCESADO}.o ${M3}${MASTERWORKER}.o  -o ${EXEC}${MASTERWORKER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# gestorDeColas
# Compilacion
${M2}${GESTOR}.o: ${SRC}${GESTOR}.cpp ${COLA}.hpp ${COLA}.cpp ${SRC}${MONITOR}.hpp ${SRC}${MONITOR}.cpp
	${CC} -c ${CPPFLAGS} ${SRC}${GESTOR}.cpp -o ${M2}${GESTOR}.o

# Linkado
${EXEC}${GESTOR}: ${M2}${SOCKET_DIR}.o ${M2}${GESTOR}.o
	${CC} ${LDFLAGS} ${M2}${SOCKET_DIR}.o ${M2}${GESTOR}.o -o ${EXEC}${GESTOR} ${SOCKETSFLAGS}
#-----------------------------------------------------------
#-----------------------------------------------------------
# TAGS_ANALIZER
# Compilacion
${M4}${TAGS_ANALIZER}.o: ${SRC}${TAGS_ANALIZER}.cpp 
	${CC} -c ${CPPFLAGS} ${SRC}${TAGS_ANALIZER}.cpp -o ${M4}${TAGS_ANALIZER}.o

# Linkado
${EXEC}${TAGS_ANALIZER}: ${M4}${SOCKET_DIR}.o ${M4}${SEMAPHORE}.o ${M4}${TAGS_ANALIZER}.o
	${CC} ${LDFLAGS} ${M4}${SOCKET_DIR}.o ${M4}${SEMAPHORE_DIR}.o ${M4}${TAGS_ANALIZER}.o -o ${EXEC}${TAGS_ANALIZER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# QOS_ANALIZER
# Compilacion
${M4}${QOS_ANALIZER}.o: ${SRC}${QOS_ANALIZER}.cpp 
	${CC} -c ${CPPFLAGS} ${SRC}${QOS_ANALIZER}.cpp -o ${M4}${QOS_ANALIZER}.o

# Linkado
${EXEC}${QOS_ANALIZER}: ${M4}${SOCKET_DIR}.o ${M4}${QOS_ANALIZER}.o
	${CC} ${LDFLAGS} ${M4}${SOCKET_DIR}.o ${M4}${QOS_ANALIZER}.o -o ${EXEC}${QOS_ANALIZER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# SOCKETS
# Compilacion libreria de Sockets
${M1}${SOCKET_DIR}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${M1}${SOCKET_DIR}.o
${M2}${SOCKET_DIR}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${M2}${SOCKET_DIR}.o
${M3}${SOCKET_DIR}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${M3}${SOCKET_DIR}.o
${M4}${SOCKET_DIR}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${M4}${SOCKET_DIR}.o
#-----------------------------------------------------------	
# Procesado masterWorker
# Compilacion módulo auxiliar
${M3}${MWPROCESADO}.o: ${SRC}${MWPROCESADO}.hpp ${SRC}${MWPROCESADO}.cpp ${SRC}${LISTA}.hpp ${SRC}${LISTA}.cpp
	${CC} -c ${CPPFLAGS} ${SRC}${MWPROCESADO}.cpp -o ${M3}${MWPROCESADO}.o
#-----------------------------------------------------------	
# Semáforo
# Compilacion semáforo
${M4}${SEMAPHORE}.o: ${SEMAPHORE}.hpp ${SEMAPHORE}.cpp
	${CC} -c ${SEMAPHORE}.cpp -o ${M4}${SEMAPHORE_DIR}.o ${CPPFLAGS}
#-----------------------------------------------------------	
# LIMPIEZA
clean:
	$(RM) ${EXEC}*
	$(RM) ${M1}*
	$(RM) ${M2}*
	$(RM) ${M3}*
	$(RM) ${M4}*
