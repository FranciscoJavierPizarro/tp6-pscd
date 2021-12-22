#include "MWprocesado.hpp"

//Comm: Procesa el bloque de entrada y genera los bloques de tareas
//Pre:  inTweets contiene un string formado por tantos tweets como TWEETS_FROM_STREAM
//      cada tweet tiene a su izquierda "$i" i € [0,TWEETS_FROM_STREAM) y a su derecha
//      "$j" j € (0,TWEETS_FROM_STREAM]
//Post: para cada i € [0, TWEETS_FROM_STREAM/TWEETS_TO_TASK), outTasks[i] contiene:
//      un string formado por tantos tweets como TWEETS_TO_TASK
//      cada tweet tiene a su izquierda "$i" i € [0,TWEETS_TO_TASK) y a su derecha
//      "$j" j € (0,TWEETS_TO_TASK], esto esta precedido por "PUBLISH_TAREAS,"
void createTasksBlock(string inTweets, string outTasks[TWEETS_FROM_STREAM/TWEETS_TO_TASK]) {
    for(int i = 0; i < TWEETS_FROM_STREAM/TWEETS_TO_TASK; i++) {
        outTasks[i] = "PUBLISH_TAREAS,";
    }
    int a, b;
    for(int i = 0; i < TWEETS_FROM_STREAM; i++) {
        a = (inTweets.find("$" + to_string(i) + " "))+("$" + to_string(i) + " ").length();
        b = (inTweets.find_first_of("$",a)); 
        outTasks[i/TWEETS_TO_TASK].append("$" + to_string((i) % TWEETS_TO_TASK)+ " " + inTweets.substr(a,b - a));
        if ((i + 1) % TWEETS_TO_TASK == 0) outTasks[i/TWEETS_TO_TASK].append("$" + to_string(TWEETS_TO_TASK) + " ");
    }
}

void proccessTaskBlock(string& inTaskBlock, string& performance, string& result) {
    float total = 0;
    float t1,t2;
    t1 = clock()/CLOCKS_PER_SEC;
    //RT: 11/10/2021 He mejorado bastante el vuelo #3D y ahora muestra la isla completa, y las 6 últimas coladas juntas días 6 al 11. Con datos del @CabLaPalma sobre un mapa del @IGNSpain. @lapalmaopendata @InnovaLaPalma #VigilanciaLaPalma #LaPalma #ErupciónLaPalma #VolcandeLaPalma https://t.co/3xLFCqIqCY
    
    //Los datos sin terminacion son todos, los que terminan por rt los rts 
    //y los que terminan por o los originales, los que terminan por H contienen un #
    //y los que terminan por M contienen una mencion
    
    //Analizador TAGs:
    //Cada variable almacena el número de tweets en cada tipo de dispostivo
    int webApp = 0,iphone = 0,android = 0,wordpress = 0, misc = 0;
    int webAppRT = 0,iphoneRT = 0,androidRT = 0,wordpressRT = 0, miscRT = 0;
    int webAppO = 0,iphoneO = 0,androidO = 0,wordpressO = 0, miscO = 0;
    int webAppH = 0,iphoneH = 0,androidH = 0,wordpressH = 0, miscH = 0;
    int webAppM = 0,iphoneM = 0,androidM = 0,wordpressM = 0, miscM = 0;
    //Contiene los dias y horas en formato "hora"h"dia"d"mes"m de los tweets separados por ;
    string diasHoras;
    string diasHorasRT;
    string diasHorasO;
    string diasHorasH;
    //Autores separados por ;
    //x_y_es;IGNSpain;
    string authors;
    string authorsRT;
    string authorsO;
    string authorsH;
    string authorsM;
    //TAGS separados por #
    //#ErupcionLaPalma&author#volcán&author
    string tags;
    string tagsRT;
    string tagsO;
    //Menciones separadas por @
    //@pepe&author@juan&author
    string mencion;
    string mencionRT;
    string mencionO;

    //VARIABLES AUXILIARES
    string tweet[TWEETS_TO_TASK];
    string campos[4];
    int a,b;
    //LEER ENTRADA
    for(int i = 0; i < TWEETS_TO_TASK; i++) {
        a = (inTaskBlock.find("$" + to_string(i) + " "))+("$" + to_string(i) + " ").length();
        b = inTaskBlock.find_first_of("$",a); 
        tweet[i] = inTaskBlock.substr(a,b - a);
    }

    //PROCESAR
    for(int i = 0; i < TWEETS_TO_TASK; i++) {
        a = 0;
        b = 0;
        for(int j = 0; j < 4; j++) {
            if(j > 0) a = (tweet[i].find_first_of(";",a))+ 1;
            b = tweet[i].find_first_of(";",a); 
            campos[j] = tweet[i].substr(a,b - a);
            cout << campos[j] << endl;
        }



        
    }




    result = "";
    result += "$0 " + to_string(webApp) + "/" + to_string(iphone) + "/" + to_string(android) + "/" + to_string(wordpress) + "/" + to_string(misc);
    result += "$1 " + to_string(webAppRT) + "/" + to_string(iphoneRT) + "/" + to_string(androidRT) + "/" + to_string(wordpressRT) + "/" + to_string(miscRT);
    result += "$2 " + to_string(webAppO) + "/" + to_string(iphoneO) + "/" + to_string(androidO) + "/" + to_string(wordpressO) + "/" + to_string(miscO);
    result += "$3 " + to_string(webAppH) + "/" + to_string(iphoneH) + "/" + to_string(androidH) + "/" + to_string(wordpressH) + "/" + to_string(miscH);
    result += "$4 " + to_string(webAppM) + "/" + to_string(iphoneM) + "/" + to_string(androidM) + "/" + to_string(wordpressM) + "/" + to_string(miscM);
    result += "$5 " + diasHoras + "/" + diasHorasRT + "/" + diasHorasO + "/" + diasHorasH; 

    result += "$6 " + authors + "/" + authorsRT + "/" + authorsO + "/" + authorsH + "/" + authorsM; 
    result += "$7 " + tags + "%" + tagsRT + "%" + tagsO; 
    result += "$8 " + mencion + "%" + mencionRT + "%" + mencionO;

    t2 = clock()/CLOCKS_PER_SEC;
    total += (t2-t1);
    performance = to_string(total);
}