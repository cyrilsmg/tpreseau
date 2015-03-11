/** fichier libnet.h **/

/******************************************************************/
/** Ce fichier decrit les structures et les constantes utilisees **/
/** par les fonctions reseau.                                    **/
/******************************************************************/

/**** Constantes ****/

/** Nombre maximum de connexions tamponnees pour le serveur **/
#include <sys/socket.h>
#define MAX_CONNEXION	32
#define MAX_TAMPON 64
#define BUF_SIZE 1024
/**** Fonctions ****/

int nomVersAdresse(char *hote,struct sockaddr_storage *padresse);
int socketVersNom(int ds,char *nom);
int connexionServeur(char *hote,char *service);
//int initialisationServeur(short int *port);
int read_fixed(int descripteur,unsigned char *array,int size);
int creationInterfaceVirtuelle(char *nom);
int boucleServeur(int ecoute,int (*traitement)(int));
int initialisationServeur(short int port);
int ecouterClient(int sock);
