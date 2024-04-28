#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "8088"
#define EXIT_GETADDRINFO_FAILURE 1
#define EXIT_CONNECT_FAILURE 2

#define MAX_LINE 128
#define CONNECT_STR_MAX 256

ssize_t readLine(int fd, void *buffer, size_t n);
int getclientsd(char*,char*);

int main(int argc, char *argv[]) {
  size_t bytes_read;
  int sd;			/* socket descriptor */
  char line[MAX_LINE];
  int status;
  char *host;
  char *port;
  char connect_str[CONNECT_STR_MAX];

  if (argc != 2) {
    fprintf(stderr,"usage: %s host\n", argv[0]);
    exit(1);
  }

  host=argv[1];
  port = argv[2];
  sd=getclientsd(host, port);
  snprintf(connect_str, CONNECT_STR_MAX-1,
	    "GET / HTTP/1.1\r\nhost:%s\r\nConnection:close\r\n\r\n",
	    host);
  write(sd, connect_str, strlen(connect_str));

  while ((bytes_read=readLine(sd, line, MAX_LINE-1))>0) {
      printf("just read %s", line);
      write(sd, line, bytes_read);
  }

  close(sd);
  return 0;
}

int getclientsd(char *host, char *port) {
  int status;
  int sd;			/* socket descriptor */
  struct addrinfo hints, *p, *servinfo;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return EXIT_GETADDRINFO_FAILURE;
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sd = socket(p->ai_family, p->ai_socktype,
			 p->ai_protocol)) == -1) {
      continue;
    }

    if (connect(sd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sd);
      continue;
    }
    break;
  }

  if (!p) {
    fprintf(stderr, "failed to connect to %s\n", host);
    exit(EXIT_CONNECT_FAILURE);
  }

  freeaddrinfo(servinfo);
  return sd;
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

  if (n <= 0 || buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  buf = buffer;                       /* No pointer arithmetic on "void *" */

  totRead = 0;
  for (;;) {
    numRead = read(fd, &ch, 1);

    if (numRead == -1) {
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
