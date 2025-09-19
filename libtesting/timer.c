#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "timer.h"

struct timespec start, end;

void startTimer(){
	clock_gettime(CLOCK_MONOTONIC, &start);
}

double stopTimer(){
	clock_gettime(CLOCK_MONOTONIC, &end);

	double elapsedTime = (double) (end.tv_sec - start.tv_sec);
	elapsedTime += (double) (end.tv_nsec - start.tv_nsec) / (double) 1e9;

	return elapsedTime;
}

