/*****
* Exemple de serveur UDP
* socket en mode non connecte
*****/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 9998
#define LBUF 512

char buf[LBUF+1];
struct sockaddr_in SockConf;
char table[255][45];

typedef struct {
    char type;          // '1’ message broadcast ou ‘2’ accuse de reception
    char beuip[5];      // exactement 'B','E','U','I','P', **sans \0**
    char expediteur[32];// chaîne C terminée par '\0'
} Message;

char * addrip(unsigned long A) {
static char b[16];
  sprintf(b,"%u.%u.%u.%u",
        (unsigned int)(A>>24&0xFF),
        (unsigned int)(A>>16&0xFF),
        (unsigned int)(A>>8&0xFF),
        (unsigned int)(A&0xFF));
  return b;
}

int main(int N, char*P[])
{
int sid, n;
struct sockaddr_in Sock;
struct sockaddr_in Broad;
socklen_t ls;

if (N != 2) {
  fprintf(stderr,"Utilisation : %s pseudo\n", P[0]);
  return 1;
}

/* creation du message broadcast */
Message msg;
msg.type = '1'; // broadcast
memcpy(msg.beuip, "BEUIP", 5); // copie binaire, pas de '\0'
strncpy(msg.expediteur, P[1], 31);
msg.expediteur[31] = '\0'; // assurer terminaison

/* creation du socket */
if ((sid = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0) {
  perror("socket");
  return 2;
}

/* autoriser broadcast */
int opt = 1;
setsockopt(sid, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

/* configuration serveur */
bzero(&SockConf,sizeof(SockConf));
SockConf.sin_family = AF_INET;
SockConf.sin_port = htons(PORT);
SockConf.sin_addr.s_addr = INADDR_ANY;

if (bind(sid,(struct sockaddr*)&SockConf,sizeof(SockConf)) == -1) {
  perror("bind");
  return 3;
}

/* adresse broadcast */
bzero(&Broad,sizeof(Broad));
Broad.sin_family = AF_INET;
Broad.sin_port = htons(PORT);
Broad.sin_addr.s_addr = inet_addr("192.168.88.255");

/* envoi broadcast */
sendto(sid,&msg,sizeof(msg),0,(struct sockaddr*)&Broad,sizeof(Broad));

printf("Serveur lance sur port %d avec pseudo %s\n",PORT,P[1]);

int nb_user = 0;
for (;;) {

  ls = sizeof(Sock);

  /* attente message */
  if ((n=recvfrom(sid,(void*)buf,LBUF,0,(struct sockaddr *)&Sock,&ls))
    == -1)  perror("recvfrom");
  else {
    buf[n] = '\0';
    printf ("recu de %s : <%s>\n",addrip(ntohl(Sock.sin_addr.s_addr)), buf);
  }
  Message *m = (Message*)buf;
  
  if (memcmp(m->beuip, "BEUIP", 5) == 0) {
    switch (m->type) {
      case '1':
        char user_entry[100]; // tampon temporaire pour construire la chaîne
        snprintf(user_entry, sizeof(user_entry), " %s (%s)", m->expediteur, addrip(ntohl(Sock.sin_addr.s_addr)));

        // Vérifie si l'utilisateur est déjà dans la table
        int exists = 0;
        for (int i = 0; i < nb_user; i++) {
            if (strcmp(table[i], user_entry) == 0) {
                exists = 1;
                break;
            }
        }

        if (!exists) {  // Ajoute seulement s'il n'existe pas
            printf("Broadcast de %s (%s)\n", m->expediteur, addrip(ntohl(Sock.sin_addr.s_addr)));
            strcpy(table[nb_user], user_entry);
            nb_user++;
        }
        break;
      
      case '3':
        for (int i = 0 ; i < nb_user ; i++) printf("- %s\n", table[i]);
        break;
      
      case '4': // pseudo destinataire + message
        char pseudo[32];
        char message[32];

        // Extraire pseudo et message depuis "pseudo message"
        sscanf(m->expediteur, "%[^_]%s", pseudo, message);
        pseudo[31] = '\0';
        message[31] = '\0';

        // Chercher l'utilisateur dans la table
        char ip_str[32];
        int found = 0;

        for (int i = 0; i < nb_user; i++) {
            char stored_pseudo[32];
            char stored_ip[32];

            // Extraire pseudo et IP depuis "pseudo (IP)"
            sscanf(table[i], "%s (%[^)])", stored_pseudo, stored_ip);
            if (strcmp(stored_pseudo, pseudo) == 0) {
                strcpy(ip_str, stored_ip);
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("Utilisateur %s non trouvé\n", pseudo);
            break;
        }

        // Construire le message à envoyer
        msg.type = '9';
        memcpy(msg.beuip, "BEUIP", 5);

        strncpy(msg.expediteur, message, 31);
        msg.expediteur[31] = '\0';
        

        // Construire l'adresse destination
        struct sockaddr_in dest;
        dest.sin_family = AF_INET;
        dest.sin_port = Sock.sin_port; // même port
        dest.sin_addr.s_addr = inet_addr(ip_str);

        // Envoi
        sendto(sid, &msg, sizeof(msg), 0,(struct sockaddr*)&dest, sizeof(dest));

        break;
  
    }
  }
}


return 0;
}