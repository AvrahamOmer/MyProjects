#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>

void error (char *msg){
    perror(msg);
    exit(0);
}
void ready()
{
    printf(">");
    return;
}

void gradelist(int sockfd);

int main(int argc, char *argv[]){
    int fd[2];

    pipe(fd);
    if(fork() == 0){ //the son interpator procces
        close(fd[0]);
        char buffer_client_com[256];
        printf(">");
        while(true){
                bzero(buffer_client_com, 256);
                //sleep(1);
                //printf(">");
                if (fgets(buffer_client_com, sizeof buffer_client_com, stdin) != NULL) {
                    size_t len = strlen(buffer_client_com);
                    if (len > 0 && buffer_client_com[len-1] == '\n')
                        buffer_client_com[--len] = '\0';
                }
                write(fd[1], buffer_client_com, sizeof(buffer_client_com)); //write to pipe from buffer_com

                if(strcmp(buffer_client_com, "Exit") == 0)
                        break;
        }   
    }
    else{ // dad communication proccess
        close(fd[1]);
        char buffer_client_com1[256];
        char buffer_to_user[256];
        char buffer_test [256];


        // init client communication
        int sockfd, portno, n, n2;
        struct sockaddr_in serv_addr;
        struct hostent *server;
    
        if (argc < 3){
            fprintf(stderr,"usage %s hostname port\n", argv[0]);
            exit(0);
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            error("ERROR opening socket");
        }

        portno = atoi(argv[2]);
        server = gethostbyname(argv[1]);
        if (server == NULL){
            fprintf(stderr, "ERROR, no such host\n");
            exit(0);
        }

        bzero((char *)&serv_addr,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

        serv_addr.sin_port = htons(portno);

        if(connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
            error ("ERROR connecting");
        }

        while(true){

				bzero(buffer_client_com1, 256);
                bzero(buffer_to_user, 256);
                
                
                read(fd[0], buffer_client_com1, sizeof(buffer_client_com1)); // read from pipe to buffer_com

                //printf("the message : %s\n", buffer_client_com1);

                n = write(sockfd, buffer_client_com1, sizeof(buffer_client_com1)); // write to server

                if (n<0){
                    error("ERROR writing to socket");
                }
                //if(strcmp(buffer_client_com1, "Exit") == 0) // need more work
                  //  break;

                if(strcmp(buffer_client_com1, "GradeList") == 0){
                    gradelist(sockfd);
                    printf(">");
                    fflush(stdout);
                }

                else{

                    n = read(sockfd, buffer_to_user, sizeof(buffer_to_user)); // read from server need more work
                    if (n < 0){
                        error("ERROR reading from socket");
                    }
                    
						
                    printf("%s", buffer_to_user);
                    fflush(stdout);

                    if(strcmp(buffer_client_com1, "Exit") == 0) // need
                        break;

                    printf(">");
                    fflush(stdout);
                }

            }
        //printf ("%s\n", buffer_client_com);
        close(sockfd);
    }
    return 0;
}

void gradelist(int sockfd)
{
    int n;
    char buffer_grade_list[256];
    bzero(buffer_grade_list,256);

    while(true){
        bzero(buffer_grade_list,256);
        n = read(sockfd, buffer_grade_list, sizeof(buffer_grade_list)); // read from server need more work
            if (n < 0){
                error("ERROR reading from socket");}
        if(strcmp(buffer_grade_list, "Action not allowed\n") == 0)
        {
            printf("%s", buffer_grade_list);
            fflush(stdout);
            break;
        }
        if(strcmp(buffer_grade_list, "Finish") == 0)
            break;
        printf("%s", buffer_grade_list);
        fflush(stdout);
    }

    return;

}
