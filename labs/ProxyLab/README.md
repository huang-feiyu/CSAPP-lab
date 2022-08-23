# Proxy Lab

This lab ties together many of the concepts from the course, such as byte
ordering, caching, process control, signals, signal handling, file I/O,
concurrency, and synchronization.

[WriteUp](http://csapp.cs.cmu.edu/3e/proxylab.pdf)

## Prepare

We will implement the middleman between a web browser and an *end* server - Proxy Server.
It is that we need to implement a server to *client* and a client to *end* server.

## Part I

> Implementing a sequential web proxy

1. Listen for incoming connections on a port
2. When a connection is established, read the request from *client* and parse it
3. If the request is valid, establish a connection to the *end* server
4. Read *end* server's response and forward it to *client*

-- We gonna only handle GET requests, and forward via GET HTTP/1.0.

Take a look at [tiny](./tiny/tiny.c), just need to do a few things:
Get a request from *client* and parse it, build a request to *end* server, and
send it. Read from *end* server and send it to *client*.

Easy to get 40 scores.
