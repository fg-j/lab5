/********************************************/
/* FLORA GALLINA-JONES AND SASHA FRIEDRICH  */
/********************************************/

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
pthread_cond_t buffCheck; 

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

void millisleep(int sleeptime) {
  // wrapper for nanosleep
  long adjustedTime = sleeptime*(10^6);
  long maxTime = 999999999;
  while (adjustedTime > maxTime) {
    sleepTime.tv_nsec = maxTime;
    sleepTime.tv_sec = 0;

    adjustedTime -= maxTime;

    int err = nanosleep(&sleepTime, NULL);

    if (err) {
      unix_error("Sleep error");
    }

  }

  sleepTime.tv_nsec = adjustedTime;
  sleepTime.tv_sec = 0;
  int err = nanosleep(&sleepTime, NULL);

  if (err) {
    unix_error("Sleep error");
  }
}

/***********************/

void * consumer(void * returnval) {
  //printf("consumer called \n");
  int totalSum = 0;
  
  while (1) {
    // Pick up the mutex
    Pthread_mutex_lock(&mutex);
    // printf("consumer grabs mutex \n");
    
    while(bufferSize == 0) {
      Pthread_cond_wait(&buffCheck, &mutex); // puts down the lock until receives ping from producer
    }
    // printf("Consumer buffer size before read: %d \n", bufferSize);
    // We have the mutex -- read from the buffer
    message tempMes;
    tempMes = buffer[nextToEmpty];
    nextToEmpty = (nextToEmpty + 1) % BUFFER_SIZE;
    bufferSize--;
    // printf("Consumer buffer size after read: %d \n", bufferSize); 
    
    //drop the mutex
    Pthread_mutex_unlock(&mutex);
    // printf("consumer dropped mutex \n");

    // Send a message to the producer to check if the buffer is still full
    Pthread_cond_signal(&buffCheck);

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
  }
}

void producer() {
  // printf("producer called \n");
   
  int lineNum = 1;  
  int eof = 0;

  while (eof != EOF){
    // printf("producer entered scanf loop \n");
    message  mes;
    // Read a line from stdinput

    int producer_sleep;
    mes.line = lineNum;

    int tempVal;
    int tempConSleep;
    int tempPrintCode;
    // mes.value = 1;
    // producer_sleep = 1;
    // mes.consumer_sleep = 1;
    // mes.print_code = 3;

    //eof = scanf("%d", &(mes.value));
    eof = scanf("%d %d %d %d", &tempVal, &producer_sleep, &tempConSleep, &tempPrintCode);
    // printf("successful scanf \n");
    lineNum++;

    mes.value = tempVal;
    mes.consumer_sleep = tempConSleep;
    mes.print_code = tempPrintCode;
    // Reading/parse a line from stdin

    if (eof == EOF) {
      mes.quit = 1;
    }
    else {
      mes.quit = 0;
      millisleep(producer_sleep);
      // printf("producer finished sleep \n");
    }

    // pick up the mutex
    Pthread_mutex_lock(&mutex);
    // printf("producer grabs mutex \n");
    // Only want to write to buffer if it's not full
    while (bufferSize == BUFFER_SIZE){ // no room in the buffer
      Pthread_cond_wait(&buffCheck, &mutex); // briefly puts down the mutex
    }
    // printf("Producer buffer size before add: %d \n", bufferSize);
    // We have the mutex -- write to the buffer
    
    buffer[nextToFill] = mes;
    nextToFill = (nextToFill+1) % BUFFER_SIZE;
    bufferSize++;
    // printf("Producer buffer size after add: %d \n", bufferSize);

    // Drop the mutex
    Pthread_mutex_unlock(&mutex);
    // printf("producer dropped mutex \n");

    Pthread_cond_signal(&buffCheck); // signal to consumer thread to check status of buffer again
    
    if (mes.print_code == 1 || mes.print_code == 3) {
      printf("Produced %d from input line %d\n", mes.value, mes.line);
    }

  }
}

/********************/

int main() {

  setlinebuf(stdout);

  // printf("Started run \n");
  //Initialize the mutex
  Pthread_mutex_init(&mutex, NULL);
  // printf("Initialized mutex \n");
  
  //Initialize the buffer conditions
  Pthread_cond_init(&buffCheck, NULL);
  // printf("condition init\n");


  //Create the consumer thread
  pthread_t consumer_thread;
  Pthread_create(&consumer_thread, NULL, &consumer, NULL); //Inputs to consumer?
  // printf("thread created\n");

  producer();

  Pthread_join(consumer_thread, NULL);

  //pthread_exit?
  return 0;
}
