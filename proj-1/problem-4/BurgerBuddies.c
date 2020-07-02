#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>


/*

This is a program to solve the Burger Buddies Problem, which schedules
the work among cooks,cashiers and customers.

*/


#define COOK_TIME 1
#define CUSTOMER_TIME 5 //the range of time during which all customers come in 


int cooks,cashiers,customers,rack_size;
//int num_rack=0; // this data is used to debug, while it can be commented in the program
int customer_served=0;

//three sequences used to create thread.
pthread_t *cook_thread;
pthread_t *cashier_thread ;
pthread_t *customer_thread ;

//two mutex-clocks
//pthread_mutex_t access_rack;// this sem is used to debug, while it can be commented in the program
pthread_mutex_t access_cus;


sem_t av_rack;  //available numbers of rack
sem_t custom_number;
sem_t av_cashier;   //available cashiers that can accept order
sem_t custom_eating; 
sem_t occupied_rack; 
sem_t finish; // be used to kill the cooks and cashiers 


void * cook_func(void *param){

    int cook_id=*(int*)param;

    while(1){
        
        //if rack is full the cook will not start to make burger.
        sem_wait(&av_rack);
        sem_post(&av_rack);
        
        // printf("Cooks[%d] is busy making a burger.\n",cook_id);
        //make burger
        sleep(COOK_TIME);

        //in case that there is empty space before cooking while no space after that
        sem_wait(&av_rack);
               //pthread_mutex_lock(&access_rack);
                // num_rack++;
                //printf("num_rack after push is: %d\n",num_rack);
                sem_post(&occupied_rack);
            //pthread_mutex_unlock(&access_rack);
            printf("Cooks[%d] make a burger.\n",cook_id);

    }

}


void *cashier_func(void *param){
    int cashier_id = *(int *) param; // get ID

    while(1){
        //if no customers,then wait
        sem_wait(&custom_number);
        printf("Cashier [%d] accepts an order.\n", cashier_id);

        //if no burger, wait
        sem_wait(&occupied_rack);
            //pthread_mutex_lock(&access_rack);
            // num_rack--;
            //printf("num_rack after cashier is: %d\n",num_rack);
            sem_post(&av_rack);
            //pthread_mutex_unlock(&access_rack);
        sem_post(&custom_eating);

        //only one can access to the data;
        pthread_mutex_lock(&access_cus);
        customer_served++;
        pthread_mutex_unlock(&access_cus);

        printf("Cashier [%d] takes a burger to customer.\n", cashier_id);   
    }
}

void *customer_func( void *param){
    int id = *(int *) param;

    int t=rand()%CUSTOMER_TIME+1;
    sleep(t);
    
    printf("Customer [%d] comes.\n", id);
    
    sem_post(&custom_number);
        sem_wait(&av_cashier);
            sem_wait(&custom_eating);
        sem_post(&av_cashier);

    pthread_mutex_lock(&access_cus);
        if(customer_served==customers) 
            sem_post(&finish);
    pthread_mutex_unlock(&access_cus);  

    pthread_exit(0);
}


void *monitor(void *param) {
    sem_wait(&finish); // all the customers have been served
    int i;
    for (i = 0; i < cooks; ++i) pthread_kill(cook_thread[i], SIGUSR1); // kill the cooks
    for (i = 0; i < cashiers; ++i) pthread_kill(cashier_thread[i], SIGUSR1); // kill the cashiers
    pthread_exit(0);
}

int main(int argc, char **argv){

    //exception
    if(argc!=5){
        printf("parameter number error!\n");
        return 0;
    }

    //exclude some invalid (no-number) input
    int i;
    for(i=1;i<=4;++i){
        int k=0;
       while (argv[i][k])
      { 
        if(argv[i][k]>='0'&&argv[i][k]<='9')
            k++;
        else{
            printf("invalid char!\n");
            return 0;
        }    

      } 
    }
    
    //get the value of parameters
    cooks=atoi(argv[1]);
    cashiers=atoi(argv[2]);
    customers=atoi(argv[3]);
    rack_size=atoi(argv[4]);
    
    //exclude some terrible value
    if(rack_size==0|cooks==0|cashiers==0|customers==0){
        printf("Parameters should not be less than one!\n");
        return 0;        
    }

    printf("Cooks [%d], Cashiers [%d], Customers [%d]\n", cooks, cashiers, customers);
    printf("Begin run.\n");

    //get the id for customers,cooks and cashiers
    int max = cooks;
    if (max < cashiers) max = cashiers;
    if (max < customers) max = customers;
    int *IDs = (int *) malloc((max + 1) * sizeof(int));

    for (i = 0; i <= max; ++i)
        IDs[i] = i;
    //pthread_mutex_init(&access_rack,NULL);
    pthread_mutex_init(&access_cus,NULL);
    sem_init(&av_rack,0,rack_size);
    sem_init(&av_cashier,0,cashiers);
    sem_init(&occupied_rack,0,0);
    sem_init(&custom_number,0,0);
    sem_init(&custom_eating,0,0);
    sem_init(&finish,0,0);

    srand(time(NULL));

    //create three kinds of threads
    cook_thread = malloc(cooks*sizeof(pthread_t));
    cashier_thread = malloc(cashiers*sizeof(pthread_t));
    customer_thread = malloc(customers*sizeof(pthread_t));

    for (i = 0; i < cooks; ++i) {
        pthread_create(cook_thread+i, NULL, cook_func, (void *) &IDs[i + 1]);
    }
    cashiers=atoi(argv[2]);
    for (i = 0; i < cashiers; ++i) {
        pthread_create(cashier_thread+i, NULL, cashier_func, (void *) &IDs[i + 1]);
    }
    for (i = 0; i < customers; ++i) {
        pthread_create(customer_thread+i, NULL, customer_func, (void *) &IDs[i + 1]);
    }

    //set a monitor which can kill all of the threads;
    pthread_t _monitor;
    pthread_create(&_monitor, NULL,monitor, NULL);
    pthread_join(_monitor, NULL);

    //release the memory
    free(IDs);
    free(cook_thread);
    free(cashier_thread);
    free(customer_thread);

    //destroy the sem
    //pthread_mutex_destroy(&access_rack);
    pthread_mutex_destroy(&access_cus);
    sem_destroy(&av_rack);
    sem_destroy(&av_cashier);
    sem_destroy(&occupied_rack);
    sem_destroy(&custom_number);
    sem_destroy(&custom_eating);

}

//adb push /opt/ptree/thread/libs/armeabi/BBC /data/misc