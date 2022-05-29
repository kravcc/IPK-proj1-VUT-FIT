// IPK Projekt č.1
// Autor: Marina Kravchuk (xkravc02)

#include <stdio.h>      
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdlib.h>      
#include <string.h>    
#include <unistd.h>     

#define RCVBUFSIZE 32   

void HandleTCPClient(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE];        
    int recvMsgSize;    
    char response[256];                
    size_t count = RCVBUFSIZE;
    ssize_t res;

    res = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0);
    if (!(strncmp(echoBuffer, "GET /hostname", 13)))
    {
        FILE *f= popen ("hostname", "r");
        char s[256]; 
        fgets (s, sizeof (s), f);
        strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n");
        strcat(response, s);
        pclose (f);
    }
    else if (!(strncmp(echoBuffer, "GET /cpu-name", 13)))
    {
        FILE *f= popen ("cat /proc/cpuinfo | grep 'model name' | uniq | cut -d: -f2 | cut -c 2-", "r");
        char s[256]; 
        fgets (s, sizeof (s), f);
        strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n");
        strcat(response, s);
        pclose (f);
    }
    else if (!(strncmp(echoBuffer, "GET /load", 9)))
    {
        FILE *f= popen ("cat /proc/stat | cut -c 6- | head -2", "r");
        char s[256]; 
        fgets (s, sizeof (s), f);
        unsigned long long int data[2][10];
        char toint[22] = "";
        for (int j = 0, i = 0, z = 0; s[j] != '\0'; j++, z++)
        {
            if (s[j] == ' ')
            {
                z = 0;
                data[0][i] = atoll(toint);
                i++;
                for (int k = 0; k < 22; k++)
                {
                    toint[k] = '\0';
                }
                j++;
            }
            
      		toint[z] = s[j];
        }
        data[0][9] = atoll(toint);
        fgets (s, sizeof (s), f);
        for (int j = 0, i = 0, z = 0; s[j] != '\0'; j++, z++)
        {
            if (s[j] == ' ')
            {
                z = 0;
                data[1][i] = atoll(toint);
                i++;
                for (int k = 0; k < 22; k++)
                {
                    toint[k] = '\0';
                }
                j++;
            }
            
      		toint[z] = s[j];
        }
        data[1][9] = atoll(toint);

        long long int idle[2];
        long long int total[2] = {0,0};
        for (int i = 0; i < 2; i++)
        {
            idle[i] = data[i][3] + data[i][4];
            for (int j = 0; j < 10; j++)
            {
                total[i] += data[i][j];
            }
            
        }
        double load = 100 * ((double)(total[1] - total[0]) - (double)(idle[1] - idle[0])) / (total[1] - total[0]);
        char str[6];
        snprintf (str, 6, "%.2f%%", load);
        strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n");
        strcat(response, str);
        strcat(response, "\n");
        pclose (f);
    }
    else
    {
        strcpy(response, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain;\r\n\r\n");
        strcat(response, "400 Bad Request");
    }
    
    send(clntSocket, response, strlen(response), 0);
}

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */
    if (echoServPort < 0 || echoServPort > 65535)
    {
        fprintf(stderr, "Incorrect port\n");
        exit(1);
    }
    /* Create socket for incoming connections */
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        fprintf(stderr, "socket() failed");
        exit(1);
    }  
    int reuse = 1;
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const char *)&reuse, sizeof(int));  
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    {
        fprintf(stderr, "bind() failed");
        exit(1);
    }
    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, 3) < 0)
    {
        fprintf(stderr, "listen() failed");
        exit(1);
    }
        

    for (;;)
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
        {
            fprintf(stderr, "accept() failed");
            exit(1);
        }
        /* clntSock is connected to a client! */

        HandleTCPClient(clntSock);
        close(clntSock);    /* Close client socket */
    }
    return 0;
}