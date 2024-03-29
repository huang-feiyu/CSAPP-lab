/*
 * proxy.c - A simple proxy server that caches web pages, can only handle GET requests.
 */

#include <stdio.h>
#include <assert.h>
#include "csapp.h"

#define DEBUG

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

typedef struct {
    char *uri;
    char *obj;
} CacheLine;

typedef struct {
    int used_cnt;
    CacheLine *objects;
} Cache;

Cache cache;
int readcnt;
sem_t mutex, w;

static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void *Tdoit(void *vargp);
void doit(int fd);
void write_requesthdrs(rio_t *rio, char *req_str, char *hostname, char *path, char *port);
void parse_uri(char *uri, char *hostname, char *path, int *port);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void init_cache(void);
int reader(int fd, char *uri);
void writer(char *uri, char *buf);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    init_cache();

    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    /* 1. Listen for incoming connections on a port */
    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        // wait for a client connection request, store client address to clientaddr
        connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        // get the hostname and port of the client
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
#ifdef DEBUG
        printf("\nAccepted connection from (%s, %s)\n", hostname, port);
#endif
        Pthread_create(&tid, NULL, Tdoit, (void *) &connfd);
    }
}

/*
 * Tdoit - Thread routine for handling each client connection
 */
void *Tdoit(void *vargp) {
    int connfd = *((int *) vargp);
    Pthread_detach(pthread_self());
    doit(connfd);
    Close(connfd);
    return NULL;
}

/*
 * doit - handle one HTTP request/response transaction
 */
void doit(int fd) {
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], path[MAXLINE];
    int port;

    rio_t rio;

    /* 2.1: When a connection is established, read the request from *client*
     * Read request line and headers */
    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);
#ifdef DEBUG
    printf("\n==Request headers==\n%s", buf);
#endif
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET") != 0) {
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy Server does not implement this method");
        return;
    }

    /* 2.2: Parse the request line and headers
     * Parse URI from GET request */
    parse_uri(uri, hostname, path, &port);
    char port_str[6];
    sprintf(port_str, "%d", port);

    if (reader(fd, uri)) {
#ifdef DEBUG
        printf("Read from cache\n");
#endif
        return;
    }

    /* 3. If the request is valid, establish a connection to the *end* server */
    int endfd, n;
    rio_t rio_end;

    endfd = Open_clientfd(hostname, port_str);
    if (endfd < 0) {
        clienterror(fd, method, "404", "Not Found",
                    "Proxy Server could not find the requested page");
        return;
    }
    rio_readinitb(&rio_end, endfd);

    // build request header
    char new_request[MAXLINE];
    write_requesthdrs(&rio, new_request, hostname, path, port_str);

    /* 4. Read *end* server's response and forward it to *client* */
    size_t obj_size = 0;
    char *obj = (char *) Malloc(MAX_OBJECT_SIZE * sizeof(char));

    Rio_writen(endfd, new_request, strlen(new_request));
    while ((n = Rio_readlineb(&rio_end, buf, MAXLINE)) != 0) {
        Rio_writen(fd, buf, n);
        if (obj_size < MAX_OBJECT_SIZE) {
            memcpy(obj + obj_size, buf, n);
            obj_size += n;
        }
    }

    if (obj_size < MAX_OBJECT_SIZE) {
        writer(uri, obj);
#ifdef DEBUG
        printf("Write to cache");
#endif
    }

    Close(endfd);
}

/*
 * write_requesthdrs - build request header and send it to the server
 */
void write_requesthdrs(rio_t *rio, char *req_str, char *hostname, char *path, char *port) {
    sprintf(req_str, "GET %s HTTP/1.0\r\n", path);

    sprintf(req_str + strlen(req_str), "Host: %s:%s\r\n", hostname, port);
    sprintf(req_str + strlen(req_str), "%s", user_agent_hdr);
    sprintf(req_str + strlen(req_str), "Connection: close\r\n");
    sprintf(req_str + strlen(req_str), "Proxy-Connection: close\r\n");

    char buf[MAXLINE];
    while (Rio_readlineb(rio, buf, MAXLINE) > 0) {
        if (!strcmp(buf, "\r\n")) break;
        if (strstr(buf, "Host:") != NULL) continue;
        if (strstr(buf, "User-Agent:") != NULL) continue;
        if (strstr(buf, "Connection:") != NULL) continue;
        if (strstr(buf, "Proxy-Connection:") != NULL) continue;

        sprintf(req_str + strlen(req_str), "%s", buf);
    }

    sprintf(req_str + strlen(req_str), "\r\n"); // end of request header

#ifdef DEBUG
    printf("\n==Request headers==\n%s", req_str);
#endif
}

/*
 * parse_uri - parse URI into hostname, path and port
 */
void parse_uri(char *uri, char *hostname, char *path, int *port) {
#ifdef DEBUG
    printf("\n==Parse URI==\n");
#endif
    *port = 80;
    char *pos_hostname = strstr(uri, "//");
    pos_hostname = pos_hostname + 2;

    char *pos_port = strstr(pos_hostname, ":");
    if (pos_port != NULL) {
#ifdef DEBUG
        printf("An explicit port is specified\n");
#endif
        *pos_port = '\0';
        strncpy(hostname, pos_hostname, MAXLINE);
        sscanf(pos_port + 1, "%d%s", port, path);
        *pos_port = ':';
    } else {
#ifdef DEBUG
        printf("No explicit port\n");
#endif
        pos_port = strstr(pos_hostname, "/");
        if (pos_port == NULL) {
            strncpy(hostname, pos_hostname, MAXLINE);
            strcpy(path, "");
        } else {
            *pos_port = '\0';
            strncpy(hostname, pos_hostname, MAXLINE);
            *pos_port = '/';
            strncpy(path, pos_port, MAXLINE);
        }
    }
#ifdef DEBUG
    printf("hostname: %s\n", hostname);
    printf("path: %s\n", path);
    printf("port: %d\n", *port);
#endif
}

/*
 * clienterror - returns an error message to the client
 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Proxy Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Proxy server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}

/*
 * init_cache - Init global cache object
 */
void init_cache(void) {
    Sem_init(&mutex, 0, 1);
    Sem_init(&w, 0, 1);
    readcnt = 0;
    cache.objects = (CacheLine *) Malloc(10 * sizeof(CacheLine));
    cache.used_cnt = 0;

    assert(MAX_OBJECT_SIZE * 10 < MAX_CACHE_SIZE);
    for (int i = 0; i < 10; ++i) {
        cache.objects[i].uri = (char *) Malloc(MAXLINE * sizeof(char));
        cache.objects[i].obj = (char *) Malloc(MAX_OBJECT_SIZE * sizeof(char));
    }
}

/*
 * reader - reader to get object
 *          return 1 if object in cache, 0 otherwise
 */
int reader(int fd, char *uri) {
#ifdef DEBUG
    printf("\n==Reader==\n");
#endif
    int flag = 0; // whether the object in cache
    P(&mutex);
    readcnt++;
    if (readcnt == 1) { // First in
        P(&w);
    }
    V(&mutex);

    // Critical section: Reading happens
#ifdef DEBUG
    printf("Reader: Enter Critical section\n");
#endif
    for (int i = 0; i < 10; ++i) {
        if (!strcmp(cache.objects[i].uri, uri)) {
            // write to client file descriptor
            Rio_writen(fd, cache.objects[i].obj, MAXLINE);
            flag = 1;
            break;
        }
    }

    P(&mutex);
    readcnt--;
    if (readcnt == 0) { // Last out
        V(&w);
    }
    V(&mutex);
#ifdef DEBUG
    printf("Reader: End\n");
#endif
    return flag;
}

/*
 * writer - write to specific object
 */
void writer(char *uri, char *buf) {
#ifdef DEBUG
    printf("\n==Writer==\n");
#endif
    P(&w);

    // Critical section: Writing happens
#ifdef DEBUG
    printf("Writer: Enter Critical section\n");
#endif
    strcpy(cache.objects[cache.used_cnt].uri, uri);
    strcpy(cache.objects[cache.used_cnt].obj, buf);
    cache.used_cnt++;

    V(&w);
#ifdef DEBUG
    printf("Writer: End\n");
#endif
}
