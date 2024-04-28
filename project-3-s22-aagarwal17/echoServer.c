/**
 * @file echoServer.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief This is my Assignment 3 Code (creating a multi-threaded echo server)
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "echoServer.h"

//Default Parameters and other important variables:
#define DEFAULT_PORT 8088
//#define DEFAULT_MESSAGE_LENGTH = 256 //THIS IS BUF SIZE!
#define DEFAULT_LOG_FILE "log.txt"
//#define DEFAULT_NUM_WORKERS 5 //REMOVE THIS
//#define DEFAULT_EXIT_NUM -1 //REMOVE THIS
//#define MAX_SIZE 1000 //REMOVE THIS
#define DEFAULT_LENGTH 256 //REMOVE THIS TOTALLY

//Global variables to hold true/false values for flag parser:
bool MessageLenSpecified = false;
bool PortNoSpecified = false;
bool cellNumSpecified = false;
bool workerThreadNumSpecified = false;
bool terminatingCharSpecified = false;

//Creating a variable to store the message length, if provided
int messageLength;
//Creating a variable to store the number of cells
int cellAmount;
//Creating a variable to store the number of worker threads
int workerThreadAmount;
//Creating a variable to store the terminating character;
char *terminatingCharacter;

//My locks and conditions variables to be used throughout the program
pthread_mutex_t job_queue_lock;
pthread_mutex_t log_queue_lock;
pthread_mutex_t log_lock;
pthread_cond_t cond1;
pthread_cond_t cond2;
pthread_cond_t cond3;
pthread_cond_t cond4;

//Defining our two queues necessary for this program:
Queue *job_queue;
Queue *log_queue;

/**
 * @brief my main function, which does the majority of the work of the main thread
 * 
 * @param argc number of arguments
 * @param argv an array of the arguments provided
 * @return int for indicating success or failure
 */
int main(int argc, char **argv)
{
    //Creating a variable to store the port, if provided
    int port;

    //Local variable to be used for different flags
    int opt;

    while((opt = getopt(argc, argv, ":m:p:c:w:t:")) != -1)
    {
        switch(opt)
        {
            case 'm': //if -m inputted, user has specified a maximum length of message than be received
                MessageLenSpecified = true;
                messageLength = atoi(optarg);
                break;
            case 'p': //if -p inputted, user has specified a port number on which to listen for incoming connections
                PortNoSpecified = true;
                port = atoi(optarg);
                break;
            case 'c': //user must provide this flag, and it indicates the number of cells in the connection buffer/job queue
                cellNumSpecified = true;
                cellAmount = atoi(optarg);
                break;
            case 'w': //user must provide this flag, and it indicates the number of worker threads
                workerThreadNumSpecified = true;
                workerThreadAmount = atoi(optarg);
                break;
            case 't': //user must provide this flag, and it indicates the terminating character to end echoing for this client
                terminatingCharSpecified = true;
                terminatingCharacter = optarg;
                break;
            case ':': 
                printf("option needs a value\n"); 
                break; 
            case '?': //if some invalid flag is inputted, print out the following error type message
                fprintf(stderr, "Unknown Option: '-%c'.\n", optopt);
                break;
        }
    }

    //This optind is for the extra arguments which are not parsed
    for(; optind < argc; optind++)
    {     
        printf("extra arguments: %s\n", argv[optind]); 
    }

    if (MessageLenSpecified == false)
    {
        messageLength = DEFAULT_LENGTH;
    }
    if (PortNoSpecified == false)
    {
        port = DEFAULT_PORT;
    }
    if (cellNumSpecified == false)
    {
        printf("User must specify number of cells for the connection buffer\n");
        exit(1);
    }
    if (workerThreadNumSpecified == false)
    {
        printf("User must specify number of worker threads\n");
        exit(1); 
    }
    if (terminatingCharSpecified == false)
    {
        printf("User must specify a terminating character\n");
        exit(1);
    }
    //We must check if the port is a valid integer (in the possible range for a port):
    if (port < 1024 || port > 65535)
    {
        printf("Please enter a port number between 1024 and 65535.\n");
        exit(1);
    }
    //We should check that message length is a valid length:
    if (messageLength <= 0)
    {
        printf("Please enter a message length greater than 0\n");
        exit(1);
    }
    //We should check that number of worker threads is a valid number:
    if (workerThreadAmount <= 0)
    {
        printf("Please enter a worker thread amount greater than 0\n");
        exit(1);
    }
    //We should check that cell amount is a valid amount:
    if (cellAmount <= 0)
    {
        printf("Please enter a cell amount greater than 0\n");
        exit(1);
    }


    //Create the job queue and log queue:
    job_queue = create_queue();
    log_queue = create_queue();

    //Initializing the condition variables and the locks:
    pthread_mutex_init(&job_queue_lock, NULL);
    pthread_mutex_init(&log_queue_lock, NULL);
    pthread_mutex_init(&log_lock, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    pthread_cond_init(&cond3, NULL);
    pthread_cond_init(&cond4, NULL);

    //Creating the NUM_WORKER threads
    pthread_t workers[workerThreadAmount];
    for (int i = 0; i < workerThreadAmount; i++)
    {
        pthread_create(&workers[i], NULL, &worker_thread, NULL);
    }

    //Creating the log thread:
    pthread_t log;
    pthread_create(&log, NULL, &log_thread, NULL);

    //Setting up the socket:
    struct sockaddr_in client;
    socklen_t client_size = sizeof(struct sockaddr_in);  
    int connection_socket = open_listenfd(port);

    //Just making strings now to be used throughout program for success and full queue:
    char *success = "Connected to server. Type terminating character to exit: \n"; 
    //strcat(success, terminatingCharacter);
    //strcat(success, "to exit: \n");

    char *full = "Job buffer is full.\n";


    /*
    When the server starts, the main thread creates a fixed-sized data structure which will be used to store the socket descriptor information 
    of the clients that will connect to it. The number of elements in this data structure (shared buffer) is specified by a program input parameter 
    (‘size of the connection buffer’). The main thread creates a pool of worker threads (‘the number of threads’ specified as a program parameter), 
    and then the main thread immediately begins to behave in the following manner (to accept and distribute connection requests):

    while (true)
    {
        connected_socket = accept(listening_socket);
        add connected_socket information to the work buffer;
        signal any sleeping workers that there's a new socket in the buffer;
    }
    */

    while(1) //while true, accept the incoming connection
    {
        int client_socket = accept(connection_socket, (struct sockaddr*)&client, &client_size);

        //If error connecting to socket, print error message:
        if (client_socket == -1)
        {
            printf("Could not connect to socket %d.\n", client_socket);
            continue;
        }

        //We lock the job queue to check if it is full:
        pthread_mutex_lock(&job_queue_lock);
        if(job_queue->queue_size >= cellAmount) 
        {
            send(client_socket, full, strlen(full), 0);
            pthread_cond_wait(&cond2, &job_queue_lock);
        }

        //At this point, we have succesfully connected to a new client, so print message out to terminal
        printf("Connected to a new client! Client ID: %d\n", client_socket);
        send(client_socket, success, strlen(success), 0);

        //Now, we add the client/new job to the job_queue:
        push(job_queue, client, NULL, client_socket);
        //At this point, we can unlock the lock we had to check if the job queue was full:
        pthread_mutex_unlock(&job_queue_lock);
        //As described in the instructions, we should now signal any sleeping workers that there's a new socket in the buffer:
        pthread_cond_signal(&cond1);

        time_t t = time(NULL);
        char time_str[DEFAULT_LENGTH];
        ctime_r(&t, time_str);
        printf("The time of the request from the client is: %s", time_str);
    }

    return 0; //if we have made it to this point in the mains server thread, we were successful, so return 0 to indicate success
}

/**
 * @brief Function used above to create the socket descriptor, and binds the socket descriptor for the specified port.
 *        From textbook: "bind() associates the socket descriptor created with socket() to the port we want the server to listen on.
 *        Once the descriptor is bound, the listen() call prepares the socket so that we can call accept() on it and get a connection to the user."
 * @param port The port, either the default one or the user-specified one
 * @return int returns file descriptor or -1 on failure
 */
//FROM OUR TEXTBOOK!
int open_listenfd(int port)
{
    int listenfd, optval = 1;
	struct sockaddr_in serveraddr;

	// Creating a socket descriptor
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		return -1;
	}

	 // Eliminating "Address already in use" error from bind
	 if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) < 0)
     {
	 	return -1;
	 }

	 //Reset the serveraddr struct, setting all of it's bytes to zero.
	 //Some properties are then set for the struct.
	 //bind() is then called, associating the port number with the socket descriptor.
	 bzero((char *) &serveraddr, sizeof(serveraddr));
	 serveraddr.sin_family = AF_INET;
	 serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	 serveraddr.sin_port = htons((unsigned short)port);
	 if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
     {
	 	return -1;
	 }

	 //Prepare the socket to allow accept() calls. 
     //The value 20 is the backlog, this is the maximum number of connections that will be placed on queue until accept() is called again.
	 if (listen(listenfd, 20) < 0) 
     {
	 	return -1;
	 }

     //returning the listen file descriptor:
	 return listenfd;
}

/**
 * @brief Create a queue object (defined in echoServer.h)
 * 
 * @return Queue* 
 */
Queue *create_queue()
{
    Queue *temp = (Queue *) malloc(sizeof(Queue));
    
    if (temp == NULL) //if error in allocating memory for queue, print out error message to terminal
    {
        printf("Unable to allocate memory for Queue. \n");
        exit(1);
    }

    temp->front = NULL; //we of course set the front to null to start
    temp->queue_size = 0; //we of course set the queue size to 0 to start
    return temp; //we trutn this queue we created
}

/**
 * @brief We also need a function to delete the queue once we are done with it:
 * 
 * @param queue the queue that we are freeing/removing
 */
void del_queue(Queue *queue)
{
    //We just need to call free to "delete" the queue
    free(queue ->front);
    free(queue);
}

/**
 * @brief Function to create our node object, as defined in echoServer.h
 * 
 * @param client the client
 * @param word the word/mesage
 * @param socket the socket
 * @return Node* we return the created node
 */
Node *create_node(struct sockaddr_in client, char *word, int socket) 
{
    //Allocating space for node:
    Node *temp = (Node *) malloc(sizeof(Node));
    if(temp == NULL) //as above for creating a queue, if error in allocating memory for node, print out error message to terminal
    {
        printf("Unable to allocate memory for Node.\n");
        exit(1);
    }
    temp->client = client; //node's client is set to the parameter client
    if (word == NULL)
    {
        temp->word = word;
    }
    else
    {
        //Allocating space for word:
        temp->word = malloc(sizeof(char *) * strlen(word) + 1);
        if(temp->word == NULL) //as above for creating a queue, if error in allocating memory for word, print out error message to terminal
        {
            printf("Unable to allocate memory for Node.\n");
            exit(1);
        }
        strcpy(temp->word, word); //otherwise we copy the inputted word into the node
    }

    temp->next = NULL; //when creating node, we set next to null
    temp->client_socket = socket; //set client_socket to the socket inputted as a parameter
    
    // time_t t = time(NULL);
    // char time_str[DEFAULT_LENGTH];
    // ctime_r(&t, time_str);
    // printf("The time of the request from the client is: %s", time_str);

    return temp; //return the created node
}

/**
 * @brief Pushing a created node struct onto the queue:

 * 
 * @param queue the queue
 * @param client the client
 * @param word the word/message
 * @param socket the socket
 */
void push(Queue *queue, struct sockaddr_in client, char *word, int socket) 
{
    //Creating a new node:
    Node *temp = create_node(client, word, socket);

    //For an empty queue:
    if(queue->queue_size == 0)
    {
        queue->front = temp; //setting beginning of queue to our created node if the queue_size is 0 (empty)
    }
    //Else, place the new node at the end of the queue for first in first out (FIFO), as instructed:
    else
    {
        Node * head = queue->front;
        while(head->next != NULL) //for iterating through queue
        {
            head = head->next;
        }
        head->next = temp; //once we have found an empty slot, we add our node to the queue
    }
    queue->queue_size++; //increase queue size by 1 because we just added a node to the queue
}

/**
 * @brief function for popping off the first node struct from the queue (to be done after we are done with that node)
 * 
 * @param queue the queue to remove the node from
 * @return Node* return the node removed
 */
Node *pop(Queue *queue)
{
    //if the queue is empty, return NULL:
    if (queue->front == NULL)
    {
        queue->queue_size = 0;
        return NULL;
    }

    //for when the queue is not empty, replace the first node and return it
    Node *temp = queue->front; //store the node in a temp variable
    queue->front = queue->front->next; //setting the front of the queue to the next element in the queue
    queue->queue_size--; //decreasing queue size by one since we remove one element from the queue
    free(queue->front); //free the node that we are trying to pop off
    return temp; //return the node that was popped off
}

/**
 * @brief main function for my worker threads. NUM_WORKERS represents the number of worker threads
 * 
 * @param args 
 * @return void* 
 */
void *worker_thread(void *args)
{
    //Messages to print out to the terminal
    char *prompt_message = "Enter a message to be echoed >> ";
    char *close_message = "ECHO SERVICE COMPLETE.\nConnection with the server is closed \n";
    char *error_message = "Unable to display the given message";

    /* PROVIDED DESIGN FOR WORKER THREAD:
    Each server worker thread's main loop is as follows:
    while (true) 
    {
        while (the work queue is NOT empty) 
        {
            remove a socket data element from the connection buffer
            notify that there's an empty spot in the connection buffer
            service the client
            close socket
        }
    }*/

    while(1) //while true
    {
        //Lock the job queue:
        pthread_mutex_lock(&job_queue_lock);
        if (job_queue->queue_size <= 0) //if queue empty, wait
        {
            pthread_cond_wait(&cond1, &job_queue_lock);
        }

        //Now Queue is not empty, so:
        
        //Pop the first job of the queue as we want to service this job now:
        Node *job = pop(job_queue);
        
        //We can now release the lock because we aren't accessing the job_queue at this point:
        pthread_mutex_unlock(&job_queue_lock);

        //Since we popped off an element from the queue, we know there is an empty spot in the connection buffer.
        //Thus, send a signal to notify that there is an empty slot in the job queue:
        pthread_cond_signal(&cond2);

        //For this client/node/cell, get the client socket to be able to service the client later:
        int client_socket = job->client_socket;

        //We use recv() and send() to be able to talk with the client:
        while(1)
        {

            char recv_buffer[DEFAULT_LENGTH] = "";
            //printf("the buffer val is %d", recv_buffer[1]);

            //Send the prompt to the client
            send(client_socket, prompt_message, strlen(prompt_message), 0);
            int bytes_returned = recv(client_socket, recv_buffer, messageLength, 0);
            //printf("the buffer val is %c", recv_buffer[0]);
            //printf("the terminating char is %c", terminatingCharacter[0]);

            time_t timeRequestSent = time(NULL);
            char time_strRequestSent[DEFAULT_LENGTH];
            ctime_r(&timeRequestSent, time_strRequestSent);
            time_strRequestSent[strcspn(time_strRequestSent, "\n")] = 0;


            //Checking if there was any error in the process, and sending the error prompt message if so:
            if (bytes_returned <= -1)
            {
                send(client_socket, error_message, strlen(error_message), 0);
                continue;
            }
            //If the client wishes to quit the program (inputs terminating character) //NEED TO EDIT THIS TO WORK IN MAIN FUNCTION --> CHANGE VALUE OF EXIT_NUM
            else if (recv_buffer[0] == terminatingCharacter[0])
            {
                time_t t = time(NULL);
                char time_str[DEFAULT_LENGTH];
                ctime_r(&t, time_str);

                char resultToClient[] = "ECHO SERVICE COMPLETE. Time of request from client is: "; //TO LOG FILE
                char resultToClientAppend[] = ". Client termination time is: ";

                strcat(resultToClient, time_strRequestSent);
                strcat(resultToClient, resultToClientAppend);
                strcat(resultToClient, time_str);

                send(client_socket, close_message, strlen(close_message), 0);

                struct sockaddr_in client = job->client;

                //No more than one worker thread at a time should manipulate the log queue at any one time.
                //This can be ensured through the proper use of mutual exclusion. Again, synchronization should be using locks and condition variables.
                //Thus, we lock the log buffer here:
                pthread_mutex_lock(&log_queue_lock);

                if (log_queue->queue_size >= cellAmount) //if our queue is full, wait until not full anymore
                {
                    pthread_cond_wait(&cond4, &log_queue_lock);
                }

                //Adding the message and result to the log buffer:
                push(log_queue, client, resultToClient, client_socket);

                //We are done working with the log queue, so we can release/unlock the lock on the log buffer:
                pthread_mutex_unlock(&log_queue_lock);

                //Sending the signal:
                pthread_cond_signal(&cond3);

                close(client_socket);

                break;
            }
            //Otherwise, we just want to echo what the user writes:
            else
            {

                recv_buffer[strlen(recv_buffer) - 1] = '\0';
                recv_buffer[bytes_returned - 2] = '\0';

                char theWord[DEFAULT_LENGTH];
                strncpy(theWord, recv_buffer, bytes_returned);


                char *resultToLog = " was just read. Success! Time of the request from client is: ";
                char *resultToLogAppend = ". Time echo completed is: ";

                char *resultToClient = " was just read. Success!\n";



                /*
                while ((bytes_read=readLine(connectedfd, line, MAX_LINE-1))>0) {
                    printf("just read %s", line);
                    write(connectedfd, line, bytes_read);
                */

                strcat(recv_buffer, resultToClient);
                printf("Letting Main Server know that %s", recv_buffer); //printing to main thread

                //Sending the message and result over to the client:
                send(client_socket, recv_buffer, strlen(recv_buffer), 0);


                strcat(theWord, resultToLog);
                strcat(theWord, time_strRequestSent);
                strcat(theWord, resultToLogAppend);

                time_t t = time(NULL);
                char time_str[DEFAULT_LENGTH];
                ctime_r(&t, time_str);

                strcat(theWord, time_str);

                struct sockaddr_in client = job->client;

                //No more than one worker thread at a time should manipulate the log queue at any one time.
                //This can be ensured through the proper use of mutual exclusion. Again, synchronization should be using locks and condition variables.
                //Thus, we lock the log buffer here:
                pthread_mutex_lock(&log_queue_lock);

                if (log_queue->queue_size >= cellAmount) //if our queue is full, wait until not full anymore
                {
                    pthread_cond_wait(&cond4, &log_queue_lock);
                }

                //Adding the message and result to the log buffer:
                push(log_queue, client, theWord, client_socket);

                //We are done working with the log queue, so we can release/unlock the lock on the log buffer:
                pthread_mutex_unlock(&log_queue_lock);

                //Sending the signal:
                pthread_cond_signal(&cond3);
            }
        }
    }
}

/**
 * @brief main function for log threads. Writes the result to a log file
 * A second server thread will monitor a log queue and process entries by removing and writing them to a log file.
 * @param args 
 * @return void* 
 */
void *log_thread(void *args)
{
    //Outline in Instructions:
    /*
    A second server thread will monitor a log queue and process entries by removing and writing them to a log file.
    while (true) 
    {
        while (the log queue is NOT empty) 
        {
            remove an entry from the log
            write the entry to the log file
        }
    }
    */
    while(1) //while true
    {
        //Locking the log queue:
        pthread_mutex_lock(&log_queue_lock);

        //if log queue is empty, wait:
        if (log_queue->queue_size <= 0)
        {
            pthread_cond_wait(&cond3, &log_queue_lock);
        }

        //Now that log queue is not empty:
        //get the message:
        Node *node = pop(log_queue); //grabbing top node
        char *word = node->word;

        //Since we are done accessing the log queue, release the lock:
        pthread_mutex_unlock(&log_queue_lock);
        //Send signal:
        pthread_cond_signal(&cond4);

        //If no message is inputted, do nothing:
        if (word == NULL)
        {
            continue;
        }

        //Now, we lock the log file:
        pthread_mutex_lock(&log_lock);

        //Writing the results to the log file:
        FILE *log_file = fopen(DEFAULT_LOG_FILE, "a"); //'a' flag: Open a text file in append mode for reading or updating at the end of the file. fopen() creates the file if it does not exist.

        fprintf(log_file, "%s", word);
        fclose(log_file);

        //Since we are done accessing the log file, we release/unlock the lock:
        pthread_mutex_unlock(&log_lock);
    }
}

/* FROM KERRISK 

   Read characters from 'fd' until a newline is encountered. If a newline
   character is not encountered in the first (n - 1) bytes, then the excess
   characters are discarded. The returned string placed in 'buf' is
   null-terminated and includes the newline character if it was read in the
   first (n - 1) bytes. The function return value is the number of bytes
   placed in buffer (which includes the newline character if encountered,
   but excludes the terminating null byte). */
ssize_t readLine(int fd, void *buffer, size_t n) 
{
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) 
    {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) 
    {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) 
        {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */
        } 
        else if (numRead == 0) 
        {                               /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;
        } 
        else 
        {                               /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) 
            {                           /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }
    *buf = '\0';
    return totRead;
}


