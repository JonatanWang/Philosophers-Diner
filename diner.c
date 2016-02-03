/* diner.c -- lab3: Simulation of diner philosopher problem. */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int cnt = 48;
char table[200] ="";

pthread_mutex_t mutex[201];
/*
* "|E|"== eating, "_T_" == thinking & "_W_" == waiting.
*/

struct phil_parms
{
    int pos;  		// Positions of philosophers in table[200].
    int noPhils;	// Total number of philosophers
    int left;	    	 // Position of the left chopstick
    int right;		 // Position of the right chopstick
    int mostRight; 	/* Position of the most right chopstick 
			(i.e. the most left chopstick). */
    int lock;	 	// Commandline input of deadlock situation. 0:unlock; 1:lock.
};

void* philosophize (void* parameters){
	
	   	
    	struct phil_parms *p = (struct phil_parms*) parameters; 
	
	while(cnt>1){	
        // If it is not deadlock
        if(p->lock == 0)
        {
            // Philosopher locks up the left chopstick.
            if(pthread_mutex_lock(&mutex[p->left])==0)
            {
                // Philosofer tries to lock up the right chopstick.
		// if succeeds, continue eat, think & wait.
		// if fails, unlock the left chopstick 
		// for other philosophers to avoid deadlock.
                if(pthread_mutex_trylock(&mutex[p->right])==0)
                {
		    // The most right chopstick == the most left chopstick.
                    if(p->right == 0)
                    {
                        table[0] = '|';
                    }
                    if(p->left == 0)
                    {
                        table[p->mostRight] = '|';
                    } 

                    // Out put "|E|" pattern.
                    table[p->pos-1] = '|'; 
                    table[p->pos] = 'E';
                    table[p->pos+1] = '|';
		    
		    // Eat 1 - 3 second. 
                    sleep(rand()%2 + 1);
		
		    // Put down the chopsticks when eating is done. 
                    if(p->right == 0)  
                    {
                        
                        table[0] = '_';
                    }

                    if(p->left == 0)
                    {
                        table[p->mostRight] = '_';
                    }

                    // Restore the pattern when eating is done. 
                    table[p->pos+1] = '_';
                    table[p->pos-1] = '_';
		    
                    // Unlock the left chopstick. 
                    pthread_mutex_unlock(&mutex[p->left]);
                    // Unlock the right chopstick. 
                    pthread_mutex_unlock(&mutex[p->right]);
                    // Output "T" and "W" pattern, symbols for thinking
		    // and waiting. 
                    table[p->pos] = 'T';
                    sleep(rand()%5 + 2); 	 // think 2 - 6 second
                    table[p->pos] = 'W'; 
                }
                else
		    // Release the left chopstick to avoid deadlock.
                    pthread_mutex_unlock(&mutex[p->left]);
            }
        }

        if(p->lock == 1) 
	// Deadlock situation: every philosopher lifts up the left
	// chopstick and waiting for the right one. No one can eat. 
        {
            pthread_mutex_lock(&mutex[p->left]);
            table[p->pos - 1] = '|';
	    //table[p->pos] = '|';
//	    table[p->pos + 1] = '|';
            table[p->mostRight] = '|';
            sleep(rand()%5 + 2);
	    // Make deadlock here.
            pthread_mutex_lock(&mutex[p->right]);
//            table[p->pos - 1] = '|';
	    //table[p->pos] = 'W';
	    table[p->pos+1] = '|'; 
            //table[p->mostRight] = '|';
            sleep(rand()%5 + 2);
            table[p->pos] = 'W';
            
        }
      }
    return NULL;
}

int main (int argc, char* argv[]){
    
    	int lock = atoi(argv[1]);
    	int noPhils = atoi(argv[2]);
    	pthread_t phils[noPhils]; /*Some sort of array of phils are needed*/
    	struct phil_parms phil[noPhils]; 
    
    	int i=0;
    	int round = 0;
    
    // Initialize the phil[] array.
    for(i = 0; i < noPhils; i++){
	// Count all positions of philosophers.
        phil[i].pos = i * 2 + 1; 
        phil[i].noPhils = noPhils; 
        phil[i].lock = lock; 
        phil[i].left = i; // Begin from the most left side.
        phil[i].mostRight = noPhils * 2; // The last position of table[] arry
        if(i != noPhils - 1){
            	// If does not sit at the last seat of table[].
		phil[i].right = i + 1; // Position number of the right chopstick.
        }
        else{
            
		phil[i].right = 0; // Make the most right chopstick == first.
        }
        // Fill all philosophers' positions as "W" at very beginning.
        table[i * 2 + 1] = 'W';
    }
	
    // Fill all chopsticks' positions as "_" at very beginning.
    for(i = 0; i < noPhils * 2 + 1; i += 2 ){
        table[i] = '_';
    }
	
    // Parallell threads for every philosopher.
    for(i = 0; i < noPhils; i++){
   
	     pthread_create(&phils[i], NULL, &philosophize, &phil[i]); 
	     //pthread_join(phils[i], NULL);
    }

    while(round<48){
        printf("Round %2d: %s\n", round + 1, table);
        sleep(1);
        round ++;
	cnt --;
    }

 
    // Make sure all thread ends properly. 
    for(i = 0; i < noPhils; i++){

        pthread_join(phils[i], NULL); 
    }

    return 0;
}
