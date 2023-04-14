#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

int **matrixA, **matrixB, **matrixResult;
int n, row_threads, temp_num_threads, num_threads;

void *calculate_matrix_multiplication(void *t)
{
	int c = (long)t;
	int begin = row_threads * c;
	int i, j, a, end;

	if (c < temp_num_threads - 1) {
		end = begin + row_threads;
	} else {
		end = n;
	}

	for (a = 0; a < n; a++) {
		for (i = begin; i < end; i++) {
			for (j = 0; j < n; j++)
				matrixResult[a][i] += matrixA[a][j] * matrixB[j][i];
		}
	}
	pthread_exit((void *)t);
}

void handle_threads(int n)
{
	int rc;
	long t;
	void *status;

	temp_num_threads = num_threads;
	if (n <= temp_num_threads) {
		temp_num_threads = n;
		row_threads = 1;
	} else {
		row_threads = (n / temp_num_threads) + ((n % temp_num_threads) != 0); /* ceil */
	}

	pthread_t threads[temp_num_threads];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (t = 0; t < temp_num_threads; t++) {
		rc = pthread_create(&threads[t], &attr, calculate_matrix_multiplication, (void *)t);
		if (rc) {
			printf("ERROR; return code form pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	pthread_attr_destroy(&attr);
	for (t = 0; t < temp_num_threads; t++) {
		rc = pthread_join(threads[t], &status);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
	}
}

void print_matrix(int **matrix, int n)
{
	int i, j;
	for (i = 0; i < n; i++) {
		printf("\n");
		for (j = 0; j < n; j++)
			printf("\t%d", matrix[i][j]);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	char *ptr;
	int i, j;
	struct timespec start, finish;
	double elapsed;

	n = strtol(argv[1], &ptr, 10);  /* Convierte la entrada de CLI(str) a int */
	num_threads = strtol(argv[2], &ptr, 10);
	
	/* Reserva memoria para las filas de las matrices */
	matrixA = (int **)malloc(n * sizeof(int *));
	matrixB = (int **)malloc(n * sizeof(int *));
	matrixResult = (int **)malloc(n * sizeof(int *));

	/* Reserva memora para las columnas de las matrices */
	for (i = 0; i < n; i++) {
		matrixA[i] = (int *)malloc(n * sizeof(int));
		matrixB[i] = (int *)malloc(n * sizeof(int));
		matrixResult[i] = (int *)malloc(n * sizeof(int));
	}

	srand(time(NULL)); /* Inicializa el generador de números aleatorios */

	/* Llena las matrices con números random */
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			matrixA[i][j] = rand() % 13;
			matrixB[i][j] = rand() % 13;
		}
	}

	/* Se toma el tiempo antes y después de las operaciones */
	clock_gettime(CLOCK_MONOTONIC, &start);
	//time_t begin = time(NULL);
	//clock_t begin = clock();

	handle_threads(n);

	clock_gettime(CLOCK_MONOTONIC, &finish);
	//time_t end = time(NULL);
	//clock_t end = clock();

	elapsed = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	printf("CPU time with matrix size %d and %d threads = %.7f seconds\n", n, num_threads, elapsed);

	/*print_matrix(matrixA, n);
	print_matrix(matrixB, n);
	print_matrix(matrixResult, n);*/

	/* Libera memoria de las matrices */
	for (i = 0; i < n; i++) {
		free(matrixA[i]);
		free(matrixB[i]);
		free(matrixResult[i]);
	}
	free(matrixA);
	free(matrixB);
	free(matrixResult);

	pthread_exit(NULL);

	return 0;
}
