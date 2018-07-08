#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 100000
#define url_length 40
#define portNum 80  //no need for different port numbers, site will redirect to correct port.
static int siteCounter;

typedef struct site
{
    char url[url_length];  // site address
//    int port;  // site port
} SITE;

int Socket_connect(char *host, in_port_t port);
SITE* FillSiteArray(SITE* siteArr, char* fname);
int CountWord(char buffer[BUFFER_SIZE], char* word);

//// program receives 2 arguments: 1.word to search. 2.data file. argv[0] is the name of the program ////
int main(int argc, char *argv[]){
    SITE* siteArr;
    siteArr = FillSiteArray(siteArr, argv[2]);
    int fd;
    char buffer[BUFFER_SIZE];

    int wordCounter = 0;
    for (int site = 0; site < siteCounter; ++site)
    {
        int sz = siteCounter;
        // fd = file-descriptor
        fd = Socket_connect(siteArr[site].url, portNum);

        write(fd, "GET /\r\n", strlen("GET /\r\n"));

        bzero(buffer, BUFFER_SIZE);

        // fill the buffer
        read(fd, buffer, BUFFER_SIZE - 1);

        wordCounter = CountWord(buffer, argv[1]);
        fprintf(stderr, "%s  ", siteArr[site].url);
        fprintf(stderr, "%i\n", wordCounter);

        bzero(buffer, BUFFER_SIZE);
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);

    return 0;
}

SITE* FillSiteArray(SITE* siteArr, char* fname)
{
    siteCounter = 0;
    int sizeArr = 10;
    siteArr = malloc(sizeArr*sizeof(SITE));
//    char strToCompare[5];

    FILE *dataFile = fopen(fname, "r");
    if (dataFile == NULL) {
        fprintf(stderr, "Failed opening the file. Exiting!\n");
        exit(1);
    }

    while (fscanf(dataFile, "%s\n", siteArr[siteCounter].url) == 1)
    {
        if(siteCounter == sizeArr)
        {
            siteArr = realloc(siteArr, siteCounter*2*sizeof(SITE));
            sizeArr *= 2;
        }

//        memcpy(strToCompare, siteArr[siteCounter].url, 5);

//        if (strcmp(strToCompare, "https") == 0)
//            siteArr[siteCounter].port = 443;
//        else if (strcmp(strToCompare, "http:") == 0)
//            siteArr[siteCounter].port = 80;

        siteCounter++;
    }

    fclose(dataFile);
    return siteArr;

}

int CountWord(char buffer[BUFFER_SIZE], char* word)
{
    int wordCounter = 0;
    char *tmp = buffer;
    while(tmp = strstr(tmp, word))
    {
        wordCounter++;
        tmp += strlen(word);
    }
    return wordCounter;
}


int Socket_connect(char *host, in_port_t port)
{
    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1, sock;

    if((hp = gethostbyname(host)) == NULL)
    {
        herror("gethostbyname");
        exit(1);
    }
    bcopy(hp->h_addr, &addr.sin_addr, (size_t) hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
    if(sock == -1)
    {
        perror("setsockopt");
        exit(1);
    }
    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("connect");
        exit(1);
    }
    return sock;
}