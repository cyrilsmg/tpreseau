/** fichier libnet.c **/

/************************************************************/
/** Ce fichier contient des fonctions reseau.              **/
/************************************************************/

/**** Fichiers d'inclusion ****/

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

/**** Constantes ****/

#define TAP_PRINCIPAL	"/dev/net/tun"

/**** Variables globales *****/

/**** Fonctions de gestion des sockets ****/

int initialisationServeur(short int port)
{
struct addrinfo precisions,*resultat;
int statut;
int s;
char service[BUF_SIZE];
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
statut=listen(s,port);
if(statut<0) return -1;
return s;
}

int boucleServeur(int sock,int (*traitement)(int))
{
struct pollfd descripteurs[MAX_CONNEXION+1];
int i;
int c=0;
for(i=0; i<MAX_CONNEXION; i++)
	{
	descripteurs[i].fd = -1;
	descripteurs[i].events = POLLIN;
	}
descripteurs[MAX_CONNEXION].fd = sock;
descripteurs[MAX_CONNEXION].events = POLLIN;
while(1)
	{
	printf("on attend que ca arrive\n");
	int nb = poll(descripteurs, MAX_CONNEXION+1, -1);
	if(nb<0)
		{
		printf("poll failed\n");
		exit(EXIT_FAILURE);
		}
	for(i=0; i<MAX_CONNEXION; i++)
		{
		if((descripteurs[i].revents&POLLIN)!=0)
			{
			printf("un client me parle\n");
			if(traitement(descripteurs[i].fd) <= 0)
				{
				printf("oh ! :( un client est parti\n");
				close(descripteurs[i].fd);
				descripteurs[i].fd = -1;
				}
			}
		}
		if((descripteurs[MAX_CONNEXION].revents&POLLIN)!=0)
			{
			printf("ouais ! Un nouveau client :D !\n");
			c=0;
			while(c<MAX_CONNEXION && (descripteurs[c].fd != -1))
			c++;
			if(c==MAX_CONNEXION)
				printf("Desole on est complet ce soir\n");
			else
				{
				if((descripteurs[c].fd = accept(descripteurs[MAX_CONNEXION].fd, NULL, NULL)) < 0)
				return -1;
				if(traitement(descripteurs[c].fd) <= 0)
					{
					printf("Un client est parti :'(\n");
					close(descripteurs[c].fd);
					descripteurs[c].fd = -1;
					}
				}
			}
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

int ecouterClient(int sock)
{
	char buffer[BUF_SIZE];
	int n = 0;
	n = recv(sock, buffer, BUF_SIZE - 1, 0);
	buffer[n] = 0;
	printf("Message recu 5 sur 5 : %s", buffer);
	return n;
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
