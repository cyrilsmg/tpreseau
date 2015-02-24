/**** Fichier principal pour le commutateur virtuel ****/

/** Fichiers d'inclusion **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <string.h>

#include "libnet.h"

/** Quelques constantes **/

/** Variables globales */

/** Fonctions **/



/* Fonction principale */
int main(int argc,char *argv[])
{
SOCKET sock;

/* Lecture des arguments de la commande */
if(argc!=3){
  fprintf(stderr,"Syntaxe : client <serveur> <port>\n");
  exit(-1);
  }

/* Connection au serveur */
char *serveur=argv[1];
char *port=argv[2];
#ifdef DEBUG
fprintf(stdout,"HUB sur %s port %s\n",serveur,port);
#endif

sock=connexionServeur(serveur,port);
if(sock<0){ fprintf(stderr,"Erreur de connexion au serveur\n"); exit(EXIT_FAILURE); }    

/* Boucle de communication avec le serveur */
struct pollfd descripteurs[2];
descripteurs[0].fd=sock;
descripteurs[0].events=POLLIN;
descripteurs[1].fd=0;
descripteurs[1].events=POLLIN;
while(1){
  char tampon[MAX_TAMPON];
  int nb=poll(descripteurs,2,-1);
  if(nb<0){ perror("main.poll"); exit(EXIT_FAILURE); }
  if((descripteurs[0].revents&POLLIN)!=0){
    int taille=read(sock,tampon,MAX_TAMPON);
    if(taille<=0) break;
    write(1,tampon,taille);
    }
  if((descripteurs[1].revents&POLLIN)!=0){
    int taille=read(0,tampon,MAX_TAMPON);
    if(taille<=0) break;
    write(sock,tampon,taille);
    }
  }

/* On termine la connexion */
shutdown(sock,SHUT_RDWR);

return 0;
}
