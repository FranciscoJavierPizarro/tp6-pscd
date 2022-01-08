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
#		  gestor -> compila el programa gestorDeColas
#		  analizadorTags -> compila el programa analizadorTags
#		  analizadorRendimiento -> compila el programa analizadorRendimiento
#*****************************************************************

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


CPPFLAGS=-I. -I${SOCKET_DIR} -O2 -std=c++11 -lsockets # Flags compilacion
LDFLAGS=-pthread # Flags linkado threads

all: streaming masterWorker gestor analizadorTags analizadorRendimiento
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
masterWorker: ${SOCKET}.o ${MWPROCESADO}.o ${MASTERWORKER}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${MASTERWORKER}.o ${MWPROCESADO}.o -o ${MASTERWORKER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# gestorDeColas
# Compilacion
${GESTOR}.o: ${GESTOR}.cpp ${COLA}.hpp ${COLA}.cpp ${MONITOR}.hpp ${MONITOR}.cpp
	${CC} -c ${CPPFLAGS} ${GESTOR}.cpp

# Linkado
gestor: ${SOCKET}.o ${GESTOR}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${GESTOR}.o -o ${GESTOR} ${SOCKETSFLAGS}
#-----------------------------------------------------------
#-----------------------------------------------------------
# TAGS_ANALIZER
# Compilacion
${TAGS_ANALIZER}.o: ${TAGS_ANALIZER}.cpp 
	${CC} -c ${CPPFLAGS} ${TAGS_ANALIZER}.cpp

# Linkado
analizadorTags: ${SOCKET}.o ${TAGS_ANALIZER}.o ${SEMAPHORE}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${TAGS_ANALIZER}.o ${SEMAPHORE}.o -o ${TAGS_ANALIZER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# QOS_ANALIZER
# Compilacion
${QOS_ANALIZER}.o: ${QOS_ANALIZER}.cpp 
	${CC} -c ${CPPFLAGS} ${QOS_ANALIZER}.cpp

# Linkado
analizadorRendimiento: ${SOCKET}.o ${QOS_ANALIZER}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${QOS_ANALIZER}.o -o ${QOS_ANALIZER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# SOCKETS
# Compilacion libreria de Sockets
${SOCKET}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${SOCKET}.o
#-----------------------------------------------------------	
# Procesado masterWorker
# Compilacion módulo auxiliar
${MWPROCESADO}.o: ${MWPROCESADO}.hpp ${MWPROCESADO}.cpp ${LISTA}.hpp ${LISTA}.cpp
	${CC} -c ${CPPFLAGS} ${MWPROCESADO}.cpp -o ${MWPROCESADO}.o
#-----------------------------------------------------------	
# Semáforo
# Compilacion semáforo
${SEMAPHORE}.o: ${SEMAPHORE}.hpp ${SEMAPHORE}.cpp
	${CC} -c ${SEMAPHORE}.cpp -o ${SEMAPHORE}.o ${CPPFLAGS}
#-----------------------------------------------------------	
# LIMPIEZA
clean:
	$(RM) ${SOCKET}.o
	$(RM) ${MONITOR}.o
	$(RM) ${STREAMING} ${STREAMING}.o
	$(RM) ${MASTERWORKER} ${MASTERWORKER}.o ${MWPROCESADO}.o
	$(RM) ${GESTOR} ${GESTOR}.o
	$(RM) ${TAGS_ANALIZER} ${TAGS_ANALIZER}.o
	$(RM) ${QOS_ANALIZER} ${QOS_ANALIZER}.o
	$(RM) ${FILTRO} -r
	$(RM) ${SEMAPHORE}.o