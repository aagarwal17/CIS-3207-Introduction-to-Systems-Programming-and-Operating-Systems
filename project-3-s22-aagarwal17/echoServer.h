/**
 * @file echoServer.h
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief This is the header file for my echoServer.c file. It contains the structs for the Queue and Node
 * @version 0.1
 * @date 2022-04-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <dirent.h>
#include <ctype.h>

// Node structure: 
typedef struct Node 
{
  struct sockaddr_in client;
  int client_socket;
  char *word;
  struct Node *next;
  //char time_str[BUF_LEN];
}Node;

// Queue structure:
typedef struct Queue 
{
  Node *front;
  int queue_size;
}Queue;

// Queue functions:
Queue *create_queue();
void del_queue(Queue *);
Node *create_node(struct sockaddr_in, char *, int);
void push(Queue *, struct sockaddr_in , char *, int);
Node *pop(Queue *);

// Function declarations for echo server:
//char **open_dictionary(char *);
int open_listenfd(int);
void *worker_thread(void *);
void *log_thread(void *);
ssize_t readLine(int, void *, size_t);

#endif
