/**** Fichier principal pour le repeteur virtuel ****/

/** Fichiers d'inclusion **/

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

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

for(int i=0;i<MAX_CONNEXION;i++) tabport[i]=-1;

// Analyse des arguments
if(argc!=2){
  fprintf(stderr,"Syntaxe : switch <port>\n");
  exit(-1);
  }
short int port=atoi(argv[1]);
#ifdef DEBUG
fprintf(stdout,"Port : %d\n",port);
#endif

// Initialisation du serveur
    //int ts=initialisationServeur("4000",10);
// Traitement des connexions et des messages
    //boucleServeur(ts,gestionClient);
    
struct pollfd descripteurs[MAX_CONNEXION];
int compteur;
descripteurs[0].fd=s;
descripteurs[0].events=POLLIN;

while(1){
    compteur=1;
    char tampon[MAX_TAMPON];
    for(int k=0;k<MAX_CONNEXION;k++){
        if(tabPort[k].fd!=(-1)) {
            descripteurs[compteur].fd=tabport[k].fd;
            descripteurs[compteur].events=POLLIN;
            compteur++;
        }
    }
    int nb=poll(descripteurs,compteur,-1);
    if(nb<0){ perror("main.poll"); exit(EXIT_FAILURE); }
    if((descripteurs[0].revents&POLLIN)!=0){
      i=0;
      while(i<MAX_CONNEXIONS && (descripteurs[i].fd != SOCKET_ERROR))
      i++;
      if(i==MAX_CONNEXIONS)
      else{
        if((desc[i].fd = accept(descripteurs[MAX_CONNEXIONS].fd, NULL, NULL)) < 0)
          return -1;
        if(clientProcess(descripteurs[i].fd) <= 0){
          close(descripteurs[i].fd);
          descripteurs[i].fd = SOCKET_ERROR;
          }
      }      
    }
    for(int k=1;k<compteur;k++){
        if((descripteurs[k].revents&POLLIN)!=0){
          int taille=read(0,tampon,MAX_TAMPON);
          if(taille<=0) {
            for(int l=0;l<MAX_CONNEXION;l++){
                if(descripteurs[k].fd = tabport[l].fd) tabport[l].fd=-1;
            }
          }
          else write(s,tampon,taille);
        }
    }
} 
return 0;
}
