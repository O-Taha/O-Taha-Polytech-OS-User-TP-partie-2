/*****
* Exemple de client UDP
* socket en mode non connecte
*****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define LBUF 512

/* parametres :
        P[1] = nom de la machine serveur
        P[2] = port
        P[3] = message
*/
int main(int N, char*P[])
{
int sid, n;
struct hostent *h;
struct sockaddr_in Sock;
socklen_t ls;
char buf[LBUF+1];
char sent[64];

    /* creation du socket */
    if ((sid=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0) {
        perror("socket");
        return(2);
    }
    /* recuperation adresse du serveur */
    if (!(h=gethostbyname("127.0.0.1"))) {
        perror("IP");
        return(3);
    }
    
    bzero(&Sock,sizeof(Sock));
    Sock.sin_family = AF_INET;
    bcopy(h->h_addr,&Sock.sin_addr,h->h_length);
    Sock.sin_port = htons(9998);
    for(;;) {
        scanf("%s", sent);
        if (sendto(sid, sent,strlen(sent),0,(struct sockaddr *)&Sock,
                            sizeof(Sock))==-1) {
            perror("sendto");
            return(4);
        }
        printf("Envoi OK !\n");
    }

    /* réception accusé */
    ls = sizeof(Sock);

    n = recvfrom(sid,buf,LBUF,0,(struct sockaddr *)&Sock,&ls);

    if (n == -1) {
        perror("recvfrom");
        return 5;
    }

    buf[n] = '\0';

    printf("Accuse du serveur : %s\n",buf);
    return 0;
}

