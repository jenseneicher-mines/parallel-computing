#include <stdio.h>
#include <cstdlib>
#include <omp.h>
#include <getopt.h>

#include "timepoint.h"
#include <iostream>
using namespace std;
int main(int argc, char ** argv) {
    
    int N = 1 << 20, opt = 0, options_index = 0;

    static struct option long_options[] = {
        {"numbers", required_argument, 0, 'n'}
    };

    while ((opt = getopt_long(argc, argv, "n:", long_options, &options_index)) != -1) {
        switch (opt) {
            case 'n':
                N = atoi(optarg);
                if (N < 0) {
                    fprintf(stderr, "Invalid argument, array must have positive length\n");
                    exit(1);
                }
                break;
            case '?':
                break;
            default:
                exit(1);
        }
    }

    uint32_t max_serial, max_parallel;
    uint64_t sum_serial, sum_parallel;

    int *Array = new int[N];
    int sum = 0;
    int max = 0;

    // Create an array with N number of random integers
    #pragma omp parallel for
    for(int i = 0; i < N; i++){
	Array[i] = rand() % 1000;
    }

    // Serial sum and max
    TimePoint t1;
    for(int i = 0; i < N; i++){
	sum = Array[i] + sum;
	if(max < Array[i]){
		max = Array[i];
	}
     }
    TimePoint t2;
 
    max_serial = max;
    sum_serial = sum;


    // parallel sum and max
    int trueSum = 0;
    int trueMax = 0;
    int threadSum = 0;
    int threadMax = 0;

    TimePoint t3;
    #pragma omp parallel shared(trueSum, trueMax) firstprivate(threadSum, threadMax)
    {
	#pragma omp for
	for(int i = 0; i < N; i++){
		threadSum = Array[i] + threadSum;
		if(threadMax < Array[i]){
			threadMax = Array[i];
		}
	}
	#pragma omp atomic update
	trueSum = trueSum + threadSum;

	#pragma omp critical
   	{
		if(threadMax > trueMax){
                        #pragma omp atomic write
			trueMax = threadMax;
		}

	}
    }
    TimePoint t4;
    

	max_parallel = trueMax;
	sum_parallel = trueSum;
	//set variables and display times
	cout << "Serial time:" << t2-t1 << endl; 
	cout << "Parallel time:" << t4-t3 << endl;

    printf("Serial max: %u\n", max_serial);
    printf("Serial sum: %lu\n", sum_serial);
    printf("Parallel max: %u\n", max_parallel);
    printf("Parallel sum: %lu\n", sum_parallel);
    
    return 0;
}
