/* File:    filtro.l
 * Authors: Axel Isaac Pazmiño Ortega NIP: 817627 y Alicia Lázaro Huerta NIP: 820574
 * Date:    15/12/2021
 * Comms:   Compilar mediante: flex filtro.l
 *                    gcc -lfl lex.yy.c -o filtro
 * Ejecutar con: ./filtro <tweets-sinProcesar.csv> tweets-filtrados.csv
 * Filtra los tweets con estructura fecha;cliente;autor;tweet eliminando emoticonos o carácteres no esperados 
 * manteniendo intacto los tags
 */
%{
    #include <stdio.h>
    #include <string.h>
    #define BUFSIZE 1024
    char buffer[BUFSIZE];
    char tweet[BUFSIZE];
%}
FECHA [0-9 :-]
CLIENTE [[:alnum:] .]
AUTOR [[:alnum:]_]
ESPANYOL ("á"|"é"|"í"|"ó"|"ú"|"ñ"|"Á"|"É"|"Í"|"Ó"|"Ú"|"Ñ")
TWEET ([[:alnum:].,;:_#@/ -]|{ESPANYOL}) 
%%
{FECHA}+;{CLIENTE}+;{AUTOR}+; { 
    strcpy(buffer,yytext);
} 
{TWEET}* {strcat(tweet,yytext);}
[\n] { 
    if (strlen(tweet) > 0) {
        strcat(buffer,tweet);
        printf("%s\n",buffer);
        memset(buffer,0,BUFSIZE);
        memset(tweet,0,BUFSIZE);
    }
} 
. // elimina los carácteres no-válidos
%%
