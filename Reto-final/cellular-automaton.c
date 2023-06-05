#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int *road;

void print_road(int *road, int n) {
	for (int i = 0; i < n; i++) {
		printf("\t%d", road[i]);
	}
	printf("\n");
}

void update_road(int *road, int n) {
	int i;
	for (i = 0; i < n; i++) {
		if(i+1 == n) {
			if(road[i]==1 && road[0]==0) {
				road[i] = 0;
				road[0] = 1;
			}
		}

		else {
			if(road[i]==1 && road[i+1]==0) {
				road[i] = 0;
				road[i+1] = 1;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	char *ptr;
	int n, iterations, i;
	double time_spent, elapsed;
	struct timespec start, finish;

	n = strtol(argv[1], &ptr, 10); /* Convierte la entrada de CLI(str) a int */
	iterations = strtol(argv[2], &ptr, 10);

	/* Reserva memoria para road */
	road = (int *)malloc(n * sizeof(int));
	
	srand(time(NULL)); /* Inicializa el generador de números aleatorios */

	/* Llena road con ceros y unos */
	for (i = 0; i < n; i++) {
		road[i] = rand() % 2;
	}

	//print_road(road, n);

	/* Se toma el tiempo antes y después de las operaciones */
	clock_gettime(CLOCK_MONOTONIC, &start);

	for(i = 0; i < iterations; i++) {
		//printf("\n");
		update_road(road, n);
		//print_road(road, n);
	}

	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	printf("CPU time with road size %d = %.5f seconds\n", n, elapsed);

	/* Libera memoria del arreglo */
	free(road);

	return 0;
}