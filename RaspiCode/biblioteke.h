#ifndef BIBLIOTEKE_H
#define BIBLIOTEKE_H

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <unistd.h>  
#include <fcntl.h>  
#include <errno.h>   
#include <termios.h> 
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 


typedef struct singlenode{
char Username[50];
char time[50];
struct singlenode* nextNode;
} node ;

typedef struct linkedlist
{
struct singlenode* head;
struct singlenode* tail;
unsigned int length;
}list;


typedef struct Userdata{
    char ID[5];
    char Name[20];
    char Password[20];
    char Entrance[20];
    }User;



int listlength;
void * listenClient();
void * listenArduino();
void set_database();
void set_server();
void open_Arduino();
int Checkentrance();
int assigning(void *, int, char **, char **);
void InsertLogin();
void setdefaults();
void * camera();
void updateClient();
node* make_node(char * a , char * b);
list* make_list();
void list_push(list* , node*);
int Checklogin(char * username , char * pass);

#endif