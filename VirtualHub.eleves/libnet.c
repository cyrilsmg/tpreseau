/** fichier libnet.c **/

/************************************************************/
/** Ce fichier contient des fonctions reseau.              **/
/************************************************************/

/**** Fichiers d'inclusion ****/

#include <unistd.h>
#include <string.h>
#include <fcntl.h>

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

/**** Constantes ****/

#define TAP_PRINCIPAL	"/dev/net/tun"

/**** Variables globales *****/

/**** Fonctions de gestion des sockets ****/

int initialisationServeur(char *service,int connexions)
{
struct addrinfo precisions,*resultat;
int statut;
int s;
/* Construction de la structure adresse */
memset(&precisions,0,sizeof precisions);
precisions.ai_family=AF_UNSPEC;
precisions.ai_socktype=SOCK_STREAM;
precisions.ai_flags=AI_PASSIVE;
statut=getaddrinfo(NULL,service,&precisions,&resultat);
if(statut<0){ perror("initialisationServeur.getaddrinfo"); exit(EXIT_FAILURE); }
/* Creation d'une socket */
s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
if(s<0){ perror("initialisationServeur.socket"); exit(EXIT_FAILURE); }
/* Options utiles */
int vrai=1;
if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
  perror("initialisationServeur.setsockopt (REUSEADDR)");
  exit(EXIT_FAILURE);
  }
if(setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai))<0){
  perror("initialisationServeur.setsockopt (NODELAY)");
  exit(EXIT_FAILURE);
  }
/* Specification de l'adresse de la socket */
statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
if(statut<0) return -1;
/* Liberation de la structure d'informations */
freeaddrinfo(resultat);
/* Taille de la queue d'attente */
statut=listen(s,connexions);
if(statut<0) return -1;
return s;
}

int boucleServeur(int ecoute,int (*traitement)(int))
{
//int dialogue;
while(1){
    /* Attente d'une connexion */
    if((dialogue=accept(ecoute,NULL,NULL))<0) return -1;
    /* Passage de la socket de dialogue a la fonction de traitement */
    if(traitement(dialogue)<0){ close(ecoute); return 0;}
    }
 
}


int connexionServeur(char *hote,char *service){
struct addrinfo precisions,*resultat;
int statut;
int s;
/* Creation de l'adresse de socket */
memset(&precisions,0,sizeof precisions);
precisions.ai_family=AF_UNSPEC;
precisions.ai_socktype=SOCK_STREAM;
statut=getaddrinfo(hote,service,&precisions,&resultat);
if(statut<0){ perror("connexionServeur.getaddrinfo"); exit(EXIT_FAILURE); }
/* Creation d'une socket */
s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
if(s<0){ perror("connexionServeur.socket"); exit(EXIT_FAILURE); }
/* Connection de la socket a l'hote */
if(connect(s,resultat->ai_addr,resultat->ai_addrlen)<0) return -1;
/* Liberation de la structure d'informations */
freeaddrinfo(resultat);
return s;
}


/**** Fonctions de gestion des interfaces virtuelles ****/

/** Ouverture d'une interface Ethernet virtuelle **/

int creationInterfaceVirtuelle(char *nom)
{
struct ifreq interface;
int fd,erreur;

/* Ouverture du peripherique principal */
if((fd=open(TAP_PRINCIPAL,O_RDWR))<0) return fd;

/* Preparation de la structure */
memset(&interface,0,sizeof(interface));
interface.ifr_flags=IFF_TAP|IFF_NO_PI;
if(nom!=NULL) strncpy(interface.ifr_name,nom,IFNAMSIZ);

/* Creation de l'interface */
if((erreur=ioctl(fd,TUNSETIFF,(void *)&interface))<0){ close(fd); return erreur; }

/* Recuperation du nom de l'interface */
if(nom!=NULL) strcpy(nom,interface.ifr_name);

return fd;
}
