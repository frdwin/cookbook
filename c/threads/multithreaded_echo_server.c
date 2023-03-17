/*
 * Multithreaded echo server
 * Author: Frederico Winter
 * https://github.com/frdwin
 */

#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 5000
#define MAX_CONNECTIONS 10

void handle_error(char *msg);
void *handle_conn(void *fd);

struct client_connection {
  char *client_ip;
  int conn_fd;
};

pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

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
  if (listen(sock_fd, 10) == -1)
    handle_error("Could not listen for connection");

  // Array of opened threads and mutex
  pthread_t thread_ids[MAX_CONNECTIONS];
  int i = 0;

  // Accept connections
  while (true) {
    int conn_fd;
    struct sockaddr_in client_addr;
    size_t client_addr_len = sizeof(client_addr);

    if ((conn_fd = accept(sock_fd, (struct sockaddr *)&client_addr,
                          (socklen_t *)&client_addr_len)) == -1) {
      handle_error("Could not accept connection");
    }

    char *client_ip = inet_ntoa((struct in_addr)client_addr.sin_addr);
    printf("* Client connected: %s\n", client_ip);

    struct client_connection new_client = {client_ip, conn_fd};

    if (pthread_create(&thread_ids[i++], NULL, &handle_conn, &new_client) != 0)
      handle_error("Error creating thread");

    /*
     * When reached the max number
     * of connections, wait for the threads
     * to exit
     */
    if (i == MAX_CONNECTIONS) {
      i = 0;
      while (i < MAX_CONNECTIONS) {
        pthread_join(thread_ids[i++], NULL);
      }
      i = 0;
    }
  }

  // Shutdown socket
  shutdown(sock_fd, SHUT_RDWR);

  return 0;
}

void handle_error(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void *handle_conn(void *new_client) {
  int conn_fd = ((struct client_connection *)new_client)->conn_fd;
  char *client_ip = ((struct client_connection *)new_client)->client_ip;

  char buffer[1024];
  int bytes_received;
  while ((bytes_received = recv(conn_fd, buffer, sizeof(buffer), 0)) > 0) {
    pthread_mutex_lock(&mx);
    printf("+ Echoing message from %s (%d bytes).\n", client_ip,
           bytes_received);
    pthread_mutex_unlock(&mx);
    send(conn_fd, buffer, bytes_received, 0);
  }

  pthread_mutex_lock(&mx);
  printf("* Client disconnected: %s\n", client_ip);
  pthread_mutex_unlock(&mx);

  // Close connection
  close(conn_fd);

  pthread_exit(NULL);
}
