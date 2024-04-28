#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define BACKLOG 10
#define DEFAULT_PORT_STR "22222"

#define EXIT_USAGE_ERROR 1
#define EXIT_GETADDRINFO_ERROR 2
#define EXIT_BIND_FAILURE 3
#define EXIT_LISTEN_FAILURE 4

#define MAX_LINE 64

int getlistenfd(char*);
ssize_t readLine(int fd, void *buffer, size_t n);

int main(int argc, char **argv) {
  int listenfd,	       /* listen socket descriptor */
    connectedfd;       /* connected socket descriptor */
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size;
  char line[MAX_LINE];
  ssize_t bytes_read;
  char client_name[MAX_LINE];
  char client_port[MAX_LINE];
  char *port;

  if (argc<2) {
    port=DEFAULT_PORT_STR;
  } else {
    port=argv[1];
  }
  
  listenfd = getlistenfd(port);

  for (;;) {
    client_addr_size = sizeof(client_addr);
    if ((connectedfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_size)) == -1) {
      fprintf(stderr, "accept error\n");
      continue;
    }
    if (getnameinfo((struct sockaddr*)&client_addr, client_addr_size,
		    client_name, MAX_LINE, client_port, MAX_LINE, 0)!=0) {
      fprintf(stderr, "error getting name information about client\n");
    } else {
      printf("accepted connection from %s:%s\n", client_name, client_port);
    }
    while ((bytes_read=readLine(connectedfd, line, MAX_LINE-1))>0) {
      printf("just read %s", line);
      write(connectedfd, line, bytes_read);
    }
    printf("connection closed\n");
    close(connectedfd);
  }
  
  return 0;
}

/* given a port number or service as string, returns a
   descriptor that we can pass to accept() */
int getlistenfd(char *port) {
  int listenfd, status;
  struct addrinfo hints, *res, *p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM; /* TCP */
  hints.ai_family = AF_INET;	   /* IPv4 */

  if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
    exit(EXIT_GETADDRINFO_ERROR);
  }

  /* try to bind to the first available address/port in the list.
     if we fail, try the next one. */
  for (p = res; p != NULL; p = p->ai_next) {
    if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
      continue;
    }

    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
      break;
    }
  }
  freeaddrinfo(res);
  if (p==NULL) {
    fprintf(stderr, "bind error. quitting.\n");
    exit(EXIT_BIND_FAILURE);
  }

  if (listen(listenfd, BACKLOG)<0) {
    fprintf(stderr, "listen error. quitting.\n");
    close(listenfd);
    exit(EXIT_LISTEN_FAILURE);
  }
  return listenfd;
}


/* FROM KERRISK 

   Read characters from 'fd' until a newline is encountered. If a newline
   character is not encountered in the first (n - 1) bytes, then the excess
   characters are discarded. The returned string placed in 'buf' is
   null-terminated and includes the newline character if it was read in the
   first (n - 1) bytes. The function return value is the number of bytes
   placed in buffer (which includes the newline character if encountered,
   but excludes the terminating null byte). */
ssize_t readLine(int fd, void *buffer, size_t n) {
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

    } else if (numRead == 0) {      /* EOF */
      if (totRead == 0)           /* No bytes read; return 0 */
	return 0;
      else                        /* Some bytes read; add '\0' */
	break;

    } else {                        /* 'numRead' must be 1 if we get here */
      if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
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
