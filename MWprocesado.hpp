#include <iostream>
using namespace std;
//CONSTANTES
const int N_WORKERS = 5;
const int TWEETS_FROM_STREAM = 25;
const int TWEETS_TO_TASK = 5;

//Comm: Procesa el bloque de entrada y genera los bloques de tareas
//Pre:  inTweets contiene un string formado por tantos tweets como TWEETS_FROM_STREAM
//      cada tweet tiene a su izquierda "$i" i € [0,TWEETS_FROM_STREAM) y a su derecha
//      "$j" j € (0,TWEETS_FROM_STREAM]
//Post: para cada i € [0, TWEETS_FROM_STREAM/TWEETS_TO_TASK), outTasks[i] contiene:
//      un string formado por tantos tweets como TWEETS_TO_TASK
//      cada tweet tiene a su izquierda "$i" i € [0,TWEETS_TO_TASK) y a su derecha
//      "$j" j € (0,TWEETS_TO_TASK], esto esta precedido por "PUBLISH_TAREAS,"
void createTasksBlock(string inTweets, string outTasks[TWEETS_FROM_STREAM/TWEETS_TO_TASK]);

void proccessTaskBlock(string& inTaskBlock, string& performance, string& tags);