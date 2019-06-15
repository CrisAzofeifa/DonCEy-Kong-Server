#include "server.h"
#include <pthread.h>
#include <stdio.h>

int main() {
  printf("Hello, World!\n");

  pthread_t thread_id;
  pthread_create(&thread_id, NULL, &startConsole, NULL);
  pthread_t thread_id2;
  pthread_create(&thread_id2, NULL, &setupStart, NULL);
  pthread_t thread_id3;
  pthread_create(&thread_id3, NULL, &listenHost, NULL);
  pthread_join(thread_id, NULL);
  pthread_join(thread_id2, NULL);
  pthread_join(thread_id3, NULL);

  return 0;
}