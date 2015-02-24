/**** Fichier principal pour le repeteur virtuel ****/

/** Fichiers d'inclusion **/


#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "libnet.h"

/** Quelques constantes **/
#define MAX_LIGNE 1024
/** Variables globales **/

int tabport[MAX_CONNEXION];
/** Fonctions **/

int gestionClient(int s){

/* Obtient une structure de fichier */
FILE *dialogue=fdopen(s,"a+");
if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

/* Echo */
char ligne[MAX_LIGNE];
while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL)
  fprintf(dialogue,"> %s",ligne);

/* Termine la connexion */
fclose(dialogue);
return 0;
}

/* Fonction principale */
int main(int argc,char *argv[]){

SOCKET sock;
// Analyse des arguments
if(argc!=2){
  fprintf(stderr,"Syntaxe : switch <port>\n");
  exit(-1);
  }
short int port=atoi(argv[1]);
#ifdef DEBUG
fprintf(stdout,"Port : %d\n",port);
#endif

//Initialisation du serveur
if((sock = initialisationServeur(port)) == SOCKET_ERROR)
{
printf("L initialisation a foire\n");
return 1;
}
//serverLoop(sock, clientManagement);
boucleServeur(sock, ecouterClient);
return 0;

}
