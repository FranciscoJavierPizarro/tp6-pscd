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
#		  filtro -> compila el programa de filtro
#		  masterWorker -> compila el programa masterWorker
#		  gestorDeColas -> compila el programa gestorDeColas
#		  analizadorTags -> compila el programa analizadorTags
#		  analizadorRendimiento -> compila el programa analizadorRendimiento
#*****************************************************************
SOCKETSFLAGS= -Wall

#---------------------------------------------------------
CC=g++
STREAMING=streaming
MASTERWORKER=masterWorker
GESTOR=gestorDeColas
FILTRO=filtro/
TAGS_ANALIZER=analizadorTags
QOS_ANALIZER=analizadorRendimiento

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
#-----------------------------------------------------------
# Streaming
# Compilacion
${M1}${STREAMING}.o: ${STREAMING}.cpp 
	${CC} -c ${CPPFLAGS} ${STREAMING}.cpp -o ${M1}${STREAMING}.o

# Linkado
streaming: ${M1}${SOCKET_DIR}.o ${M1}${STREAMING}.o
	${CC} ${LDFLAGS} ${M1}${SOCKET_DIR}.o ${M1}${STREAMING}.o -o ${STREAMING} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# masterWorker
# Compilacion
${M3}${MASTERWORKER}.o: ${MASTERWORKER}.cpp
	${CC} -c ${CPPFLAGS} ${MASTERWORKER}.cpp -o ${M3}${MASTERWORKER}.o

# Linkado
masterWorker: ${M3}${SOCKET_DIR}.o ${M3}${MWPROCESADO}.o ${M3}${MASTERWORKER}.o
	${CC} ${LDFLAGS} ${M3}${SOCKET_DIR}.o ${M3}${MWPROCESADO}.o ${M3}${MASTERWORKER}.o  -o ${MASTERWORKER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# gestorDeColas
# Compilacion
${M2}${GESTOR}.o: ${GESTOR}.cpp ${COLA}.hpp ${COLA}.cpp ${MONITOR}.hpp ${MONITOR}.cpp
	${CC} -c ${CPPFLAGS} ${GESTOR}.cpp -o ${M2}${GESTOR}.o

# Linkado
gestorDeColas: ${M2}${SOCKET_DIR}.o ${M2}${GESTOR}.o
	${CC} ${LDFLAGS} ${M2}${SOCKET_DIR}.o ${M2}${GESTOR}.o -o ${GESTOR} ${SOCKETSFLAGS}
#-----------------------------------------------------------
#-----------------------------------------------------------
# TAGS_ANALIZER
# Compilacion
${M4}${TAGS_ANALIZER}.o: ${TAGS_ANALIZER}.cpp 
	${CC} -c ${CPPFLAGS} ${TAGS_ANALIZER}.cpp -o ${M4}${TAGS_ANALIZER}.o

# Linkado
analizadorTags: ${M4}${SOCKET_DIR}.o ${M4}${SEMAPHORE}.o ${M4}${TAGS_ANALIZER}.o
	${CC} ${LDFLAGS} ${M4}${SOCKET_DIR}.o ${M4}${SEMAPHORE_DIR}.o ${M4}${TAGS_ANALIZER}.o -o ${TAGS_ANALIZER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# QOS_ANALIZER
# Compilacion
${M4}${QOS_ANALIZER}.o: ${QOS_ANALIZER}.cpp 
	${CC} -c ${CPPFLAGS} ${QOS_ANALIZER}.cpp -o ${M4}${QOS_ANALIZER}.o

# Linkado
analizadorRendimiento: ${M4}${SOCKET_DIR}.o ${M4}${QOS_ANALIZER}.o
	${CC} ${LDFLAGS} ${M4}${SOCKET_DIR}.o ${M4}${QOS_ANALIZER}.o -o ${QOS_ANALIZER} ${SOCKETSFLAGS}
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
${M3}${MWPROCESADO}.o: ${MWPROCESADO}.hpp ${MWPROCESADO}.cpp ${LISTA}.hpp ${LISTA}.cpp
	${CC} -c ${CPPFLAGS} ${MWPROCESADO}.cpp -o ${M3}${MWPROCESADO}.o
#-----------------------------------------------------------	
# Semáforo
# Compilacion semáforo
${M4}${SEMAPHORE}.o: ${SEMAPHORE}.hpp ${SEMAPHORE}.cpp
	${CC} -c ${SEMAPHORE}.cpp -o ${M4}${SEMAPHORE_DIR}.o ${CPPFLAGS}
#-----------------------------------------------------------	
# LIMPIEZA
clean:
	$(RM) ${STREAMING}
	$(RM) ${MASTERWORKER}
	$(RM) ${GESTOR}
	$(RM) ${TAGS_ANALIZER}
	$(RM) ${QOS_ANALIZER}
	$(RM) ${M1}*.o
	$(RM) ${M2}*.o
	$(RM) ${M3}*.o
	$(RM) ${M4}*.o
	$(RM) -R ${FILTRO}
