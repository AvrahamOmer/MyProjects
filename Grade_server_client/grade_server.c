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
#include <pthread.h>
#include <signal.h>
#include "myqueue.h"

#define THREAD_POOL_SIZE 5

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

void error (char *msg){
	perror(msg);
	exit(1);
}


//Student struct functions :

struct student * allocate_new_student(int student_id, char password[256], struct student *next_student);
struct student *push_student(int student_id, char password[256], struct student *next_student);
struct student * stu_read_from_file_to_struct();


// Assistant struct functions :
struct assistant * allocate_new_assistant(int assistant_id, char password[256], struct assistant * next_assistant);
struct assistant * push_assistant(int assistant_id, char password[256],struct assistant * next_assistant);
struct assistant * assi_read_from_file_to_struct();

struct student* head_stu;
struct assistant* head_ass;

struct student* Find_stu(int id);
struct assistant* Find_ass(int id);
bool check_login(struct student* curr_stu, struct assistant* curr_ass);
void send_to_client(char* str, int sockclient);
void * handle (void* p_sockfd);
void * thread_function(void*arg);
void deleteList_ass(struct assistant** head_ref);
void deleteList_stu(struct student** head_ref);
void sigint_handler(int signum);

int mainsocket;

int main(int argc,char *argv[]){
    // the read from file part
    head_stu = stu_read_from_file_to_struct();
    head_ass = assi_read_from_file_to_struct();

    // the socket part
    int newsockfd, portno, clilen;

    struct sockaddr_in serv_addr, cli_addr;

    for(int i =0; i<THREAD_POOL_SIZE;i++){
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }


    if (argc<2) {
        fprintf (stderr,"ERROR,no port provided\n");
        exit (1);
    }

    mainsocket = socket(AF_INET, SOCK_STREAM,0);
    if (mainsocket<0)
        error ("ERROR opening socket");
    bzero ((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi (argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr;
    serv_addr.sin_port = htons(portno);

    if (bind(mainsocket,(struct sockaddr *)&serv_addr,sizeof (serv_addr))<0)
        error ("ERROR on binding");

    listen (mainsocket,5);

    signal(SIGINT,sigint_handler);

    while(true){
        clilen = sizeof (cli_addr);
        newsockfd = accept(mainsocket, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd<0)
            error ("ERROR on accept");
        int* pclient = malloc(sizeof(int));
        *pclient = newsockfd;
        pthread_mutex_lock(&mutex);
        enqueue(pclient);
        pthread_cond_signal(&condition_var);
        pthread_mutex_unlock(&mutex); 
    }
    close (mainsocket);

    return 0;

}

void sigint_handler(int signum)
{
    deleteList_stu(&head_stu);
    if(head_stu != NULL)
    /*{
        printf("ERROR - didnt free students\n");
    }*/

    deleteList_ass(&head_ass);
    if(head_ass != NULL)
    /*{
        printf("ERROR- didnt free assistants\n");
    }*/
	close_queue();
    close(mainsocket);
    fflush(stdout);
    exit(0);
}

void deleteList_stu(struct student** head_ref)
{
   /* deref head_ref to get the real head */
   struct student* current = *head_ref;
   struct student* next;
 
   while (current != NULL)
   {
       next = current->next;
       free(current);
       current = next;
   }
   
   /* deref head_ref to affect the real head back
      in the caller. */
   *head_ref = NULL;
}

void deleteList_ass(struct assistant** head_ref)
{
   /* deref head_ref to get the real head */
   struct assistant* current = *head_ref;
   struct assistant* next;
 
   while (current != NULL)
   {
       next = current->next;
       free(current);
       current = next;
   }
   
   /* deref head_ref to affect the real head back
      in the caller. */
   *head_ref = NULL;
}



struct student * allocate_new_student(int student_id, char password[256], struct student *next_student){
    struct student *head = malloc(sizeof(student));
    if (head == NULL){
        perror ("new student is null");
        return (NULL); 
    }
    head->id = student_id;
    strcpy(head->pass, password);
    head->next = next_student;
    head->grade = 0;
    return head;
}


struct student *push_student(int student_id, char password[256], struct student *next_student){
    return allocate_new_student(student_id, password, next_student);
}

struct student * stu_read_from_file_to_struct()
{
    int id_num = 0;
    char id[10] = {0};
    char pass[256] = {0};
    FILE * fptr;
    fptr = fopen("students.txt", "r");

    if(fptr == NULL){
        perror("not open the file");
        exit(1);
    }

    fgets(id, 10, fptr);
    fseek(fptr, 1, SEEK_CUR);
    fscanf(fptr, "%s", pass);
    id_num = atoi(id);

    struct student * head = push_student(id_num, pass, NULL);

    while(fgetc(fptr) != EOF){
        bzero(id, 9);
        bzero(pass, 255);
        id_num = 0;

        fgets(id, 10, fptr);
        fseek(fptr, 1, SEEK_CUR);
        fscanf(fptr, "%s", pass);
        id_num = atoi(id);

        head = push_student(id_num, pass, head);
    }
    fclose(fptr);


    return (head) ;

}


// Assistants struct functions:

struct assistant * allocate_new_assistant(int assistant_id, char password[256], struct assistant * next_assistant){
    struct assistant *head = malloc(sizeof(assistant));
    if (head == NULL){
        perror ("new assistant is null");
        return (NULL); 
    }
    head->id = assistant_id;
    strcpy(head->pass, password);
    head->next = next_assistant;
    return head;
}

struct assistant * push_assistant(int assistant_id, char password[256],struct assistant * next_assistant){
    return allocate_new_assistant (assistant_id ,password , next_assistant);
}

struct assistant * assi_read_from_file_to_struct()
{
    int id_num = 0;
    char id[10] = {0};
    char pass[256] = {0};
    FILE * fptr;
    fptr = fopen("assistants.txt", "r");

    if(fptr == NULL){
        perror("not open the file");
        exit(1);
    }

    fgets(id, 10, fptr);
    fseek(fptr, 1, SEEK_CUR);
    fscanf(fptr, "%s", pass);
    id_num = atoi(id);

    struct assistant * head = push_assistant(id_num, pass, NULL);

    while(fgetc(fptr) != EOF){
        bzero(id, 9);
        bzero(pass, 255);
        id_num = 0;

        fgets(id, 10, fptr);
        fseek(fptr, 1, SEEK_CUR);
        fscanf(fptr, "%s", pass);
        id_num = atoi(id);

        head = push_assistant(id_num, pass, head);
    }
    fclose(fptr);


    return (head) ;

}

// handle functions :
struct student* head_stu;
struct assistant* head_ass;


struct student* Find_stu(int id)
{
    struct student * temp_head = head_stu; 
	while (temp_head != NULL){
		if (id == temp_head->id){
            return temp_head;
		}
	    temp_head = temp_head->next;
	}
    return NULL;
}

struct assistant* Find_ass(int id)
{
    struct assistant * temp_head = head_ass; 
	while (temp_head != NULL){ 
		if (id == temp_head->id){
            return temp_head;
		}
	    temp_head = temp_head->next;
	}
    return NULL;  
}

bool check_login(struct student* curr_stu, struct assistant* curr_ass)
{
    if(curr_stu == NULL && curr_ass == NULL)
        return false;
    return true;
}

void send_to_client(char* str, int sockclient)
{
    int i = -1;
    i = write(sockclient, str, sizeof(str)); // write to client
    if (i<0)
        error("ERROR writing to socket");
    return;
}


void * handle (void* p_sockfd)
{
    int sockfd = *(int*)p_sockfd;
    free (p_sockfd); 

    char buffer[256];
    char send_buffer[256];
    char command[256];
    int id;
    char pass[256];
    int n = -1;
    int i = -1;
    struct student* curr_stu = NULL;
    struct assistant* curr_ass = NULL;
    
    while(n != 5)
    {
        bzero(buffer, 256);
        bzero(send_buffer,256);
        bzero(command, 256);
        bzero(pass, 256);
        id = -1;
        n = -1;
        i = -1;
		
		i = read(sockfd, buffer, sizeof(buffer));
		if (i==0)
			return;			
        else if (i<0)
            error("error reading from socket");
        sscanf(buffer, "%s %d %s", command, &id, pass);
        //printf("%s   %d   %s\n", command, id, pass);// to check if the read is ok

        if (strcmp(command, "Login") == 0)
            n = 0;
        else if (strcmp(command, "ReadGrade") == 0)
            n = 1;
        else if (strcmp(command, "UpdateGrade") == 0)
            n = 2;
        else if (strcmp(command, "GradeList") == 0)
            n = 3;
        else if (strcmp(command, "Logout") == 0)
            n = 4;
        else if (strcmp(command, "Exit") == 0)
            n = 5;
        
        //printf("%d\n", n); //to check the n
        struct student * temp_head_stu;
        switch(n)
        {
            case 0: //Login
                //struct student * temp_head_stu;
                temp_head_stu = Find_stu(id);

                if (curr_stu != NULL || curr_ass != NULL){
                    sprintf (send_buffer,"Action not allowed\n"); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;   
                }

                temp_head_stu = Find_stu(id);

                if((temp_head_stu != NULL) && (strcmp(pass, temp_head_stu->pass) == 0))
                {
                    sprintf (send_buffer,"Welcome Student %d\n",temp_head_stu->id); // send to client
                    curr_stu = temp_head_stu;
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;
                }

                struct assistant * temp_head_ass = Find_ass(id);

                if((temp_head_ass != NULL) && (strcmp(pass, temp_head_ass->pass)==0))
                {
                    sprintf (send_buffer,"Welcome TA %d\n",temp_head_ass->id); // send to client
                    curr_ass = temp_head_ass;
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;
                }

                sprintf(send_buffer,"Wrong user information\n"); //send to client
                i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                if (i<0)
                    error("ERROR writing to socket");
                break;
                
            case 1: // ReadGrade

                if(!check_login(curr_stu, curr_ass))
                {
                    sprintf(send_buffer,"Not logged in\n"); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;
                }

                if (curr_stu != NULL)
                {
                    if(id < 0)
                    {
                        sprintf(send_buffer, "%d\n", curr_stu->grade);
                        i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                        if (i<0)
                            error("ERROR writing to socket");
                    }
                    else
                    {
                        sprintf(send_buffer, "Action not allowed\n"); // send to client
                        i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                        if (i<0)
                            error("ERROR writing to socket");
                    }
                    
                }

                else if (curr_ass != NULL)
                {
                    if(id > 0)
                    {
                        struct student * temp_head = Find_stu(id);
                        if(temp_head == NULL)
                        {
                            sprintf(send_buffer, "Invalid id\n"); // send to client
                            i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                            if (i<0)
                                error("ERROR writing to socket");
                        }
                        else
                        {
                            sprintf(send_buffer, "%d\n", temp_head->grade);// send to client
                            i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                            if (i<0)
                                error("ERROR writing to socket");
                        }
                    }
                    else
                    {
                        sprintf(send_buffer, "Missing argument\n"); // send to client
                        i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                        if (i<0)
                            error("ERROR writing to socket");
                    }
                }

                break;
            
            case 2: //UpdateGrade

                if(!check_login(curr_stu, curr_ass))
                {
                    sprintf(send_buffer, "Not logged in\n"); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;
                }               

                if (curr_stu != NULL)
                {
                    sprintf(send_buffer, "Action not allowed\n"); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;
                }

                int grade = atoi(pass);
                if(id > 0)
                {
                    struct student * temp_head = Find_stu(id);
                    if(temp_head == NULL)
                    {
                        struct student* new_stu = allocate_new_student(id, "", NULL);
                        new_stu->grade = grade;
                        sortedInsert(&head_stu, new_stu);
                    }
                    else // guess that client enter a grade
                    {
                        temp_head->grade = grade;
                    }
                }
                //sprintf(send_buffer, "\n"); // send to client
                i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                if (i<0)
                    error("ERROR writing to socket");
                break;

            case 3: //GradeList

                if(!check_login(curr_stu, curr_ass))
                {
                    sprintf(send_buffer, "Not logged in\n"); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;
                }               

                if (curr_stu != NULL)
                {
                    sprintf(send_buffer, "Action not allowed\n"); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;
                }               
                
                struct student * temp_head = head_stu;
                insertSort(&head_stu);
                //bubbleSort(head_stu);
                //head_stu = SortedMerge(&head_stu);
                while(temp_head != NULL) // think how to do this one
                {
                    bzero(send_buffer,256);
                    sprintf(send_buffer, "%d: %d\n", temp_head->id, temp_head->grade); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    temp_head = temp_head->next;
                }
                bzero(send_buffer,256);
                sprintf(send_buffer, "Finish"); // send to client
                i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                if (i<0)
                        error("ERROR writing to socket");
                break;


            case 4: // Logout

                if(!check_login(curr_stu, curr_ass))
                {
                    sprintf(send_buffer, "Not logged in\n"); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                    break;
                }               
                
                if(curr_stu != NULL)
                {
                    sprintf(send_buffer, "Good bye %d\n", curr_stu->id); // send to client
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                }
                else
                {
                    sprintf(send_buffer, "Good bye %d\n", curr_ass->id); // send to client 
                    i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                    if (i<0)
                        error("ERROR writing to socket");
                }
                
                curr_ass = NULL;
                curr_stu = NULL;
                break;
            
            case 5: //Exit

                curr_ass = NULL;
                curr_stu = NULL;
                close(sockfd);
                break;
            
            default:

                sprintf(send_buffer, "Wrong input\n"); // pass to the client through socket
                i = write(sockfd, send_buffer, sizeof(send_buffer)); // write to client
                if (i<0)
                    error("ERROR writing to socket");

        }
    }
}

void * thread_function(void*arg){
    while(true)
    {
        int *pclient;
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&condition_var, &mutex);
        pclient = dequeue();
        pthread_mutex_unlock(&mutex);
        if(pclient != NULL){
            handle(pclient);
        }
    }
}
