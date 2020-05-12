#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

/*****************************************************************************/
/* MAINTAINABILITY                                                           */
/* How can we enable the user to modify the port number (and if you're keen, */
/* the buffer size) without having to recompile the program?                 */
/*****************************************************************************/

#define PORT    56789           /* default port number for the echo service */
#define BSIZE   11              /* artificially short buffer size           */
#define BACKLOG 10              /* arbitrary backlog                        */

#define PROMPT  "Networks Practical Echo Server\n"
#define QUIT    ".\r\n"
#define ENDLN   '\n'

#define TRUE    (0==0)

/*****************************************************************************/
/* MAINTAINABILITY                                                           */
/* How will the user know what this function does?                           */
/*****************************************************************************/

int main(int argc, char *argv[]) {

    int sock;                   /* file descriptor for the server socket */
    struct sockaddr_in server;

    char buf[BSIZE];
    
    /* 1. Create socket*/

    /**
     * socket(int domain, int type, int protocol)
     *  - domain: protocol family of socket (probably want PF_INET which is IPv4)
     *  - type: type of socket to initialize (probably want SOCK_STREAM)
     *  - protocol: actual transport protocol (want 0, this specifies which
     *    protocol out of all protocols in the domain to use, and 0 uses default)
     */

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                /*************************************************************/
                /* ARGUMENTS look at "man 2 socket" for information          */
                /*************************************************************/
        perror("cannot open a socket");
        exit(EXIT_FAILURE);
    };

    /* 2. Bind an address at the socket*/

    server.sin_family = AF_INET;             /* ARGUMENT: see "man 2 bind"   */
    // listen on all interfaces
    server.sin_addr.s_addr = htonl(INADDR_ANY);  /* see "man 7 ip" */
    // To be made configurable
    server.sin_port = htons(PORT);   

    if (bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    };

    /* 3. Accept connections*/

    /**
     * listen takes a socketfd and a backlog
     *  - socketfd: self-explanatory
     *  - backlog is an integer specifying how many requests to queue at once
     */

    if (listen(sock, BACKLOG) < 0) {               /* ARGUMENT: see "man 2 listen" */

        perror("listen");
        exit(EXIT_FAILURE);
    };

    while (TRUE) {

    /* 4. Wait for client requests*/
       
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int stream = accept(sock, (struct sockaddr *) &client, &client_len);
        // int stream = connect(sock, (struct sockaddr *) &client, &client_len);

        int size;

        /*********************************************************************/
        /* ERROR HANDLING                                                    */
        /*      is "stream" a valid socket descriptor here?                  */
        /*      can anything go wrong in an of the code that follows?        */
        /*********************************************************************/

        // handle accept error
        if (stream < 0) {
            perror("accept failed");
            close(stream);
            exit(EXIT_FAILURE);
        }

        // send a welcome message
        if (send(stream, PROMPT, strlen(PROMPT), 0) == -1) {
            perror("send prompt");
            close(stream);
        }
        
        /* ARGUMENTS: see "man 2 send"  */

        // receive input
        // TODO: Handle errors
        size = recv(stream, &buf, BSIZE, 0);

        // null termination for strings
        buf[size] = '\0';          
        fprintf(stderr, "Text received: %s\n", buf);

        // echo input
        send(stream, &buf, strlen(buf), 0);

        close(stream);
    
    }; /* while(TRUE) */

    close(sock);

}; /* main */
