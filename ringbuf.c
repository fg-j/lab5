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

message buffer[BUFFER_SIZE];
int buffer_size = 0;


/********************************/

void unix_error(char *msg) /* Unix-style error */
{
  fprintf(stderr, "%s: %s\n", msg, strerror_r(errno)); // use thread-safe version of strerror
  exit(0);

}

void Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void*(*start_routine)(void *), void *arg) {
  int errno;

  errnum = pthread_create(thread, attr, start_routine, arg);
  // Errno nonzero if the function returns an error
  if (errnum) {
    unix_error("Thread creation error");
    exit(-1);
  }
}

void Pthread_join(pthread_t thread, void** ret_val) {
  int errnum;

  errnum = pthread_join(thread, ret_val);
  // Errnum nonzero if the function returns an error
  if (errnum) {
    unix_error("Thread join error");
    exit(-1);
  }
}

void Pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr){
  error = pthread_mutex_init(&mutex, NULL);
  if (error != 0) {
    unix_error("Mutex initialization error");
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
    unix_error("Sleep error");
    exit(-1);
  }
}

void consumer(pthread_mutex_t* mutex) {

}

void producer(pthread_mutex_t* mutex) {
   
  int line_num = 0;
  bool eof = false;
  while (!eof){
    message  mes;
    // Read a line from stdinput

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

    // pick up the mutex
    pthread_mutex_lock(&mutex);
    // Only want to write to buffer if it's not full
    while (buffer_size == BUFFER_SIZE){ // no room in the buffer
      pthread_cond_wait(&cond, &mutex); // briefly puts down the mutex
    }
    // We have the mutex -- write to the buffer
    





    // Drop the mutex
    pthread_mutex_unlock(&mutex);


  }
}

int main() {

  setlinebuf(stdout);

  // Create the buffer


  // Create a mutex
  pthread_mutex_t mutex;
  Pthread_mutex_init(&mutex, NULL);

  //Create the consumer thread
  pthread_t consumer_thread;
  Pthread_create(&consumer_thread, NULL, consumer, NULL); //Inputs to consumer?

  producer();

  Pthread_join(consumer_thread, NULL);

  //pthread_exit?
  return 0;
}
