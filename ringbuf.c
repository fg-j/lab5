#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#define BUFFER_SIZE 10

typedef struct message {
    int value; /* Value to be passed to consumer */
    int consumer_sleep; /* Time (in ms) for consumer to sleep */
    int line; /* Line number in input file */
    int print_code; /* Output code; see below */
    int quit; /* NZ if consumer should exit */
} message;

/********************/
/* Global Variables */
/********************/

message buffer[BUFFER_SIZE];
int bufferSize = 0;
int nextToFill = 0;
int nextToEmpty = 0;


// Create a mutex
pthread_mutex_t mutex;

// Create buffer conditions
pthread_cond_t buffFull; 
pthread_cond_t buffEmpty;

// declare timespec struct to use for sleeping
struct timespec sleepTime;

/********************************/

void unix_error(char *msg) /* Unix-style error */
{
  fprintf(stderr, "%s: %s\n", msg, strerror(errno)); 
}

void Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void*(*start_routine)(void *), void *arg) {

  int errnum = pthread_create(thread, attr, start_routine, arg);
  // Errno nonzero if the function returns an error
  if (errnum) {
    unix_error("Thread creation error");
  }
}

void Pthread_join(pthread_t thread, void** ret_val) {

  int errnum = pthread_join(thread, ret_val);
  // Errnum nonzero if the function returns an error
  if (errnum) {
    unix_error("Thread join error");
  }
}

void Pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr){
  int error = pthread_mutex_init(mutex, attr);
  if (error != 0) {
    unix_error("Mutex initialization error");
  }
}

void Pthread_mutex_lock(pthread_mutex_t* mutex){
  int error = pthread_mutex_lock(mutex);
  if (error != 0) {
    unix_error("Mutex lock error");
  }
}

void Pthread_mutex_unlock(pthread_mutex_t* mutex){
  int error = pthread_mutex_unlock(mutex);
  if (error != 0) {
    unix_error("Mutex unlock error");
  }
}

void Pthread_cond_init(pthread_cond_t* cond, pthread_condattr_t* cond_attr){
  int error = pthread_cond_init(cond, cond_attr);
  if (error != 0) {
    unix_error("Condition initialization error");
  }
}

void Pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex){
  int error = pthread_cond_wait(cond, mutex);
  if (error != 0) {
    unix_error("Condition wait error");
  }
}

void Pthread_cond_signal(pthread_cond_t* cond){
  int error = pthread_cond_signal(cond);
  if (error != 0) {
    unix_error("Condition signal error");
  }
}
/*
void Pthread_exit(void* return){

    pthread_exit();
  if (error != 0) {
    unix_error("Pthread exit error");
  }
}
*/
/*
bool Scanf() {
  // do the scanning
  // if read end of file, return True
  // else, return false
  int error = ;
  if (error != 0) {
    unix_error("Scanf error");
  }
}
*/
void millisleep(int sleeptime) {
  // wrapper for nanosleep
  long adjustedTime = sleeptime*(10^6);
  sleepTime.tv_nsec = adjustedTime;
  sleepTime.tv_sec = 0;

  int err = nanosleep(&sleepTime, NULL);

  if (err) {
    unix_error("Sleep error");
  }

}
/***********************/

void * consumer(void * returnval) {
  int lineNum = 0;
  int totalSum = 0;
  
  // Pick up the mutex
  Pthread_mutex_lock(&mutex);

  while(bufferSize == 0) {
    Pthread_cond_wait(&buffEmpty, &mutex); // puts down the lock until receives ping from producer
  }
  // We have the mutex -- read from the buffer
  message tempMes;
  tempMes = buffer[nextToEmpty];
  nextToEmpty = (nextToEmpty + 1) % BUFFER_SIZE;
  bufferSize--; 
  
  //drop the mutex
  Pthread_mutex_unlock(&mutex);

  // Send a message to the producer to check if the buffer is still full
  Pthread_cond_signal(&buffFull);

  // Check for quit message
  if (tempMes.quit) {
    printf("Final sum is %d\n", totalSum);
    pthread_exit(NULL);
  }
  
  millisleep(tempMes.consumer_sleep);
  totalSum += tempMes.value; // add value from buffer to running total
  if (tempMes.print_code == 2 || tempMes.print_code == 3) {
    printf("Consumed %d from input line %d; sum = %d\n", tempMes.value, tempMes.line, totalSum);
  }

  pthread_exit(NULL);
}

void producer() {
   
  int lineNum = 1;  
  int eof = 0;

  while (eof != EOF){
    message  mes;
    // Read a line from stdinput

    int producer_sleep;
    mes.line = lineNum;

    eof = scanf("%d %d %d %d", &(mes.value), &(producer_sleep), &(mes.consumer_sleep), &(mes.print_code));
    lineNum++;
    // Reading/parse a line from stdin

    if (eof == EOF) {
      mes.quit = 1;
    }
    else {
      mes.quit = 0;
      millisleep(producer_sleep);
    }

    // pick up the mutex
    Pthread_mutex_lock(&mutex);
    // Only want to write to buffer if it's not full
    while (bufferSize == BUFFER_SIZE){ // no room in the buffer
      Pthread_cond_wait(&buffFull, &mutex); // briefly puts down the mutex
    }
    // We have the mutex -- write to the buffer
    
    buffer[nextToFill] = mes;
    nextToFill = (nextToFill+1) % BUFFER_SIZE;
    bufferSize++;

    // Drop the mutex
    Pthread_mutex_unlock(&mutex);

    Pthread_cond_signal(&buffEmpty); // signal to consumer thread to check status of buffer again
    
    if (mes.print_code == 1 || mes.print_code == 3) {
      printf("Produced %d from input line %d\n", mes.value, mes.line);
    }

  }
}

/********************/

int main() {

  setlinebuf(stdout);

  //Initialize the mutex 
  Pthread_mutex_init(&mutex, NULL);
  
  //Initialize the buffer conditions
  Pthread_cond_init(&buffFull, NULL);
  Pthread_cond_init(&buffEmpty, NULL);


  //Create the consumer thread
  pthread_t consumer_thread;
  Pthread_create(&consumer_thread, NULL, &consumer, NULL); //Inputs to consumer?

  producer(&mutex, &buffFull, &buffEmpty);

  Pthread_join(consumer_thread, NULL);

  //pthread_exit?
  return 0;
}
