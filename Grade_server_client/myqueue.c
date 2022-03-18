#include "myqueue.h"
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


node_t* head = NULL;
node_t* tail = NULL;

/*struct student {
    int id;
    char pass[256];
    int grade;
    struct student *next;
} student;*/


void enqueue (int *client_socket){
	node_t *newnode = malloc(sizeof(node_t));
	newnode->client_socket = client_socket;
	newnode->next = NULL;
	if (tail == NULL){
		head = newnode;
	}
	else {
		tail->next = newnode;
	}
	tail = newnode;
}


int * dequeue(){
	if (head == NULL){
		return NULL;
	}
	else {
		int * result = head->client_socket;
		node_t * temp = head;
		head = head->next;
		if(head == NULL) {
			tail = NULL;
		}
		free (temp);
		return result;
	}
}

void close_queue(){
	
	char send_buffer[256];

	if (head == NULL)
		return;
	while (head != NULL){
		node_t * temp = head;
		head = head->next;
		int n = * temp->client_socket;
		sprintf (send_buffer,"Please end process"); // send to client
        int i = write(n, send_buffer, sizeof(send_buffer));//write to client     
		close(n);
		free(temp);
	}
	return;
}


void swap(struct student **a, struct student **b)
{
	struct student * temp = *a;
	*a = *b;
	*b = temp;

}

/* Bubble sort the given linked list */
void bubbleSort(struct student *start)
{
    int swapped, i;
    struct student *ptr1;
    struct student *lptr = NULL;
  
    /* Checking for empty list */
    if (start == NULL)
        return;
  
    do
    {
        swapped = 0;
        ptr1 = start;
  
        while (ptr1->next != lptr)
        {
            if (ptr1->id > ptr1->next->id)
            { 
                swap(&ptr1, &(ptr1->next));
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

void sortedInsert(struct student** head, struct student* newNode)
{
    struct student dummy;
    struct student* current = &dummy;
    dummy.next = *head;
 
    while (current->next != NULL && current->next->id < newNode->id) {
        current = current->next;
    }
 
    newNode->next = current->next;
    current->next = newNode;
    *head = dummy.next;
    return;
}
 
// Given a list, change it to be in sorted order (using `sortedInsert()`).
void insertSort(struct student** head)
{
    
    struct student* result = NULL;     // build the answer here
    struct student* current = *head;   // iterate over the original list
    struct student* next;
    //struct student* temp = *head;
 
    while (current != NULL)
    {
        // tricky: note the next pointer before we change it
        next = current->next;
 
        sortedInsert(&result, current);
        current = next;
    }
 
    *head = result;
    return;
}

