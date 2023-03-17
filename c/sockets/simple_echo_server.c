/*
 * Simple echo server
 * One client only
 * Author: Frederico Winter
 * https://github.com/frdwin
 */

#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 5000
#define MAX_CONNECTIONS 10

void handle_error(char *msg);

int main(void) {
  int sock_fd;
  struct sockaddr_in addr;
  int addr_len = sizeof(addr);

  // Open socket
  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    handle_error("Failed opening socket");

  // Bind the address
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(SERVER_PORT);
  if (bind(sock_fd, (struct sockaddr *)&addr, addr_len) == -1)
    handle_error("Failed binding address");

  // Listen for connection
  if (listen(sock_fd, MAX_CONNECTIONS) == -1)
    handle_error("Could not listen for connection");

  // Accept connection
  int conn_fd;
  struct sockaddr_in client_addr;
  size_t client_addr_len = sizeof(client_addr);

  if ((conn_fd = accept(sock_fd, (struct sockaddr *)&client_addr,
                        (socklen_t *)&client_addr_len)) < 0) {
    handle_error("Could not accept connection");
  }

  printf("Client connected: %s\n",
         inet_ntoa((struct in_addr)client_addr.sin_addr));

  char buffer[1024];
  int bytes_received;
  while ((bytes_received = recv(conn_fd, buffer, sizeof(buffer), 0)) > 0) {
    printf("Received message. Sending echo: %d bytes\n", bytes_received);
    send(conn_fd, buffer, bytes_received, 0);
  }

  // Close connection
  close(conn_fd);
  shutdown(sock_fd, SHUT_RDWR);

  return 0;
}

void handle_error(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
