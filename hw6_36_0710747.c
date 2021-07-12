//gcc -pthread hw6_36_0710747.c

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX 30

/* Semaphores */
sem_t barber_ready; 
sem_t customer_ready;
sem_t modifySeats;

/* Inputs */
int seats;
int customers;
int t[MAX];
int available_seats;
int served_custs = 0;
int no_served_custs = 0;

void *barber_function()
{    
    	int counter = 0;
    	while(1) {
        	sem_wait(&customer_ready);
        	sem_wait(&modifySeats);
        	available_seats++;
        	sem_post(&modifySeats);
        	sem_post(&barber_ready);        
        	printf("Barber services a client, available seats: %d\n", available_seats);
		usleep(200000);    
        	counter++;
		served_custs++;	
        	if (counter == (customers - no_served_custs)) {
            		break;
		}
    	}
    	pthread_exit(NULL);    
}

void *customer_function()
{  
    	sem_wait(&modifySeats); 
    	if (available_seats > 0) {
        	available_seats--;
        	printf("Client takes a seat, available seats: %d\n", available_seats);
        	sem_post(&customer_ready);
        	sem_post(&modifySeats);         
        	sem_wait(&barber_ready); 
    	} else {
        	sem_post(&modifySeats);
        	no_served_custs++;
        	//printf("A Customer left.\n");
    	}
    	pthread_exit(NULL);
}

int main()
{
	FILE *p;
	char ch[100];
	int i = 0;
	p = fopen("input.txt", "r");
	while (fgets(ch, 100, p) != NULL) {
		if (i == 0) {
			seats = atoi(ch);
		} else if (i == 1) {
			customers = atoi(ch);
		} else {
			t[i - 2] = atoi(ch);
		}
		i++;
	}		
    	pthread_t barber;
    	pthread_t customer[MAX];
    	sem_init(&customer_ready, 0, 0);
    	sem_init(&barber_ready, 0, 0);
    	sem_init(&modifySeats, 0, 1);
    
    	available_seats = seats; 
    
    	/* Create barber thread*/
    	pthread_create(&barber, NULL, barber_function, NULL);  
    
    	/* Create customer_maker thread*/
   	for (i = 0; i < customers; i++) {
		pthread_create(&customer[i], NULL, customer_function, NULL); 
	    	if (i != customers) {
			usleep(t[i] * 1000);
		}	
 	}    
   	/* Wait for threads to finish */
    	pthread_join(barber, NULL);
	for (i = 0; i < customers; i++) {
    		pthread_join(customer[i], NULL);
	}
        printf("Serviced clients: %d\n", served_custs);
	printf("Unserviced clients: %d\n", no_served_custs);

	return 0;	
}
