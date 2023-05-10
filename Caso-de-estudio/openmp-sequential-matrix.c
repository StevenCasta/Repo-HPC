#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int **matrixA, **matrixB, **matrixResult;

void calculate_multiplication(int n, int num_threads)
{
	int a, i, j;
	omp_set_num_threads(num_threads);
	#pragma omp parallel for collapse(2) private(a, i, j) shared(matrixA, matrixB, matrixResult)
	for (a = 0; a < n; a++) {
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++)
				matrixResult[a][i] += matrixA[a][j] * matrixB[j][i];
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
	int i, j, n, num_threads;
	double begin_time, end_time, run_time;

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

	/* Llena las matrices A y B con números random y la matriz resultado con ceros*/
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			matrixA[i][j] = rand() % 13;
			matrixB[i][j] = rand() % 13;
			matrixResult[i][j] = 0;
		}
	}

	/* Se toma el tiempo antes y después de las operaciones */
	begin_time = omp_get_wtime();
	calculate_multiplication(n, num_threads);
	end_time= omp_get_wtime();

	run_time = end_time - begin_time;
	printf("CPU time with matrix size %d and %d threads = %.7f seconds\n", n, num_threads, run_time);

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

	return 0;
}