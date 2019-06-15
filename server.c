//
// Created by Roger Valderrama on 2019-06-14.
//
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#define PORT 8080
#include "server.h"

// Base code from https://www.geeksforgeeks.org/socket-programming-cc/
// Original Author Akshat Sinha
// Modified Kimberly

int opt = (1);
int master_socket, addrlen, new_socket, client_socket[30],
    max_clients = 30, activity, i, valread, sd;
int max_sd;
struct sockaddr_in address;

char buffer[1025];

fd_set readfds;

char *message = "LOL";
/**
 * It sets up and start the server, accepting all incoming connections
 * @return
 */
void *setupStart() {
  for (i = 0; i < max_clients; i++) {
    client_socket[i] = 0;
  }

  if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                 sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Listener on port %d \n", PORT);

  if (listen(master_socket, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  addrlen = sizeof(address);
  puts("Waiting for connections ...");

  while (1 == 1) {
    FD_ZERO(&readfds);

    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    for (i = 0; i < max_clients; i++) {
      sd = client_socket[i];

      if (sd > 0)
        FD_SET(sd, &readfds);

      if (sd > max_sd)
        max_sd = sd;
    }

    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR)) {
      printf("select error");
    }

    if (FD_ISSET(master_socket, &readfds)) {
      if ((new_socket = accept(master_socket, (struct sockaddr *)&address,
                               (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      printf("New connection , socket fd is %d , ip is : %s , port : %d \n ",
             new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      if (send("", message, strlen(""), 0) != strlen("")) {
        perror("send");
      }

      puts("Welcome message sent successfully");

      for (i = 0; i < max_clients; i++) {
        if (client_socket[i] == 0) {
          client_socket[i] = new_socket;
          printf("Adding to list of sockets as %d\n", i);
          if (i == 0) {
            send(new_socket, "main", strlen("main"), 0);
          } else {
            send(client_socket[i], "listener", strlen("listener"), 0);
            send(client_socket[0], "get", strlen("get"), 0);
          }
          break;
        }
      }
    }

    for (i = 0; i < max_clients; i++) {
      sd = client_socket[i];

      if (FD_ISSET(sd, &readfds)) {

        if ((valread = read(sd, buffer, 1024)) == 0) {
          getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
          printf("Host disconnected , ip %s , port %d \n",
                 inet_ntoa(address.sin_addr), ntohs(address.sin_port));

          close(sd);
          client_socket[i] = 0;
        }

        else {
          buffer[valread] = '\0';
          send(sd, buffer, strlen(buffer), 0);
        }
      }
    }
  }
}
/**
 * Compare two strings
 * @param s string
 * @param p string
 * @return 0 if they are the same, 1 if does not.
 */
int stringEquals(char *s, char *p) {
  int i = 0;
  while (1) {
    if (p[i] == '\0' && s[i] == '\0') {
      return 0;
    }
    if (p[i] != s[i]) {
      return 1;
    }
    if (p[i] == '\0') {
      return 1;
    }
    if (s[i] == '\0') {
      return 1;
    }
    i++;
  }
}
/**
 * opens console and starts listening for inputs
 * @return
 */
void *startConsole() {
  printf("Choose a type of barrel, N for normal, C for falling and "
         "T for a delayed one. \n");
  while (1) {
    char str[100];
    while (1) {
      gets(str);
      if (!stringEquals(str, "T") || !stringEquals(str, "N") ||
          !stringEquals(str, "C")) {
        if (client_socket[0] != 0) {
          send(client_socket[0], str, strlen(str), 0);
        }
      } else if (client_socket[0] == 0) {
        printf("Error, there is no one connected");
      } else {
        printf("Bad input, only -> T N & C works\n");
      }
    }
  }
}

/**
 * Sends data to all users
 * @param characters data to send
 */
void sendtoAll(char *characters) {
  int i = 1;
  while (i < 30) {
    if (client_socket[i] != 0) {
      send(client_socket[i], characters, strlen(characters), 0);
    }
    i++;
  }
}

/**
 * Listens the first connection continuously
 * @return
 */
void *listenHost() {
  while (1) {
    char str[1025];
    while (1) {
      if (client_socket[0] != 0) {
        read(client_socket[0], str, 1025);
        printf(str);
        sendtoAll(str);
        memset(str, 0, 1024);
      } else if (client_socket[0] == 0) {
      }
    }
  }
}
