#ifndef CLIENT_UDP_H
#define CLIENT_UDP_H

#include <unistd.h>
#include <sys/types.h>
#ifdef __unix__
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SERVER_PORT 1234

void prepare_client(int *sock, struct sockaddr_in *client_addr);
void send_to_server(int sock, struct sockaddr_in serv_addr, int16_t id, int16_t keys);
int client_listen(int sock, int16_t *tab);

#endif
