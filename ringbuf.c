#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#define BUFFER_SIZE 10

struct message {
    int value; /* Value to be passed to consumer */
    int consumer_sleep; /* Time (in ms) for consumer to sleep */
    int line; /* Line number in input file */
    int print_code; /* Output code; see below */
    int quit; /* NZ if consumer should exit */
} typedef struct message message;

void Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void*(*start_routine)(void *), void *arg) {
  int errno;

  errnum = pthread_create(thread, attr, start_routine, arg);
  // Errno nonzero if the function returns an error
  if (errnum) {
    fprintf(stderr, "pthread_create error: %s\n", strerror(errnum));
    exit(-1);
  }
}

void Pthread_join(pthread_t thread, void** ret_val) {
  int errnum;

  errnum = pthread_join(thread, ret_val);
  // Errnum nonzero if the function returns an error
  if (errnum) {
    fprintf(stderr, "pthread_join error: %s\n", strerror(errnum));
    exit(-1);
  }
}

bool Scanf() {
  // do the scanning
  // if read end of file, return True
  // else, return false
}

void millisleep(int sleeptime) {
  // wrapper for nanosleep
  err = nanosleep(sleeptime*(10^6));

  if (err) {
    fprintf(stderr, "sleep error: %s\n", strerror(errno));
    exit(-1);
  }
}

void consumer() {

}

void producer() {
  int line_num = 0;
  bool eof = false;
  while (!eof){
    message  mes;
    // Read a line from text file

    mes.value;
    mes.consumer_sleep;
    int producer_sleep;
    mes.line;
    mes.print_code;

    eof = Scanf();
    // Reading/parse a line from stdin

    if (eof) {
      mes.quit = 1;
    }
    else {
      mes.quit = 0;
      millisleep(producer_sleep);
    }

  }
}

int main() {

  setlinebuf(stdout);

  // Create the buffer
  message buffer[BUFFER_SIZE];
  int buffer_size = 0;

  //Create the consumer thread
  pthread_t consumer_thread;
  Pthread_create(&consumer_thread, NULL, consumer, NULL); //Inputs to consumer?

  producer();

  Pthread_join(consumer_thread, NULL);

  //pthread_exit?
  return 0;
}
