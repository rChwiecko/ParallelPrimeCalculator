#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

//struct that will be sent to the threads
typedef struct {
	int low;
	int high;
	int thread_num;
} thread_args;

//struct that sends result out from thread
typedef struct {
	long int count;
	long int sum;
}thread_result;


int isPrime(int n) {
    //added edge case handle
    if (n <= 1){
	return 0;
    }
    // Check divisibility from 2 to N-1
    for (int i = 2; i < n; i++) {      
        // If N is divisible by i, it is not a prime number
        if (n % i == 0) {
            return 0;
        }
    }

    // If no divisors were found, N is a prime number
    return 1;
}

//thread function
void *thread(void *arg){
	thread_args *tArgs = (thread_args *)arg;//collect args
	int low = tArgs->low; //collect low value (where the thread starts)
	int high = tArgs->high; //collect high value (where the thread ends)
	int t_num = tArgs->thread_num; //thread number
	printf("Thread # %d is finding primes from low = %d to high = %d\n", t_num, low, high);
	thread_result *res = malloc(sizeof(thread_result)); //allocat space to the result
	//if allocation fails, return NULL
	if (res == NULL){
		printf("error creating result callback\n");
		return NULL;
	}
	//initialize count and sum
	res->count = 0;
	res->sum = 0;
	//loop through starting from low value to high value
	for (int i = low; i < high; i++){
		if (isPrime(i)){
			//if the number in the loop is prime, increment sum and count
			res->sum += i;
			res->count += 1;
		}
	}
	printf("Thread # %d Sum is %ld, Count is %ld\n", t_num, res->sum, res->count);
	//exit and return result 
	pthread_exit((void *)res);
}


// Assume thread_args and thread_result structs, and thread function are defined elsewhere

int main(int argc, char *argv[]) {
        // Check if exactly two command-line arguments are provided (number of threads and max value)
        if (argc != 3) {
                printf("Please provide 2 commands\n");
                exit(1);
        }
        else {
                // Parse the number of threads and the maximum range from command-line arguments
                int num_threads = atoi(argv[1]);            // Convert first argument to integer for thread count
                pthread_t threads[num_threads];             // Array to hold thread IDs
                thread_args *tArgs[num_threads];            // Array to hold arguments for each thread
                int max = atoi(argv[2]);                    // Convert second argument to integer for max range

                // Calculate the range each thread will process
                int range = ceil((double)atoi(argv[2]) / atoi(argv[1])); // Divide max by thread count
                int lo;                                     // Lower bound for each thread's range
                int hi;                                     // Upper bound for each thread's range
                long int grandCount = 0;                    // Total count across all threads
                long int grandSum = 0;                      // Total sum across all threads

                // Create threads and assign each a range to process
                for (int i = 0; i < num_threads; i++) {
                        lo = range * i;                     // Calculate the starting point of the range
                        hi = range * (i + 1);               // Calculate the ending point of the range
                        if (hi > max) {                     // Adjust hi to not exceed max
                                hi = max;
                        }

                        // Allocate memory for each thread's arguments
                        tArgs[i] = malloc(sizeof(thread_args));
                        if (tArgs[i] == NULL) {             // Check if memory allocation was successful
                                printf("error allocating memory\n");
                                return 1;
                        }

                        // Set the arguments for each thread
                        tArgs[i]->low = lo;                 // Set lower bound of range for the thread
                        tArgs[i]->high = hi;                // Set upper bound of range for the thread
                        tArgs[i]->thread_num = i;           // Assign thread number for identification

                        // Create the thread and pass in its arguments
                        if (pthread_create(&threads[i], NULL, thread, (void *)tArgs[i]) != 0) {
                                printf("Error in thread creation\n");
                                free(tArgs[i]);             // Free allocated memory in case of error
                                return 1;
                        }
                }

                // Wait for all threads to complete and collect their results
                for (int i = 0; i < num_threads; i++) {
                        thread_result *res;                 // Pointer to hold the result returned by each thread
                        pthread_join(threads[i], (void **)&res); // Wait for thread completion and capture result

                        grandSum += res->sum;               // Accumulate sum from each thread's result
                        grandCount += res->count;           // Accumulate count from each thread's result
                        
                        free(res);                          // Free the memory allocated for the result
                        free(tArgs[i]);                     // Free the memory allocated for thread arguments
                }

                // Print the final accumulated results from all threads
                printf("GRAND SUM IS %ld, COUNT IS %ld\n", grandSum, grandCount);
        }

        return 0; // Exit the program
}
