#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int **matrixA, **matrixB, **matrixResult;

void calculate_multiplication(int n)
{
	int a, i, j;
	for (a = 0; a < n; a++) {
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++)
				matrixResult[a][i] += matrixA[a][j] * matrixB[i][j];
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
	int i, j, n;
	double time_spent;
	n = strtol(argv[1], &ptr, 10);  /* Convierte la entrada de CLI(str) a int */

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
	time_t begin = time(NULL);
	//clock_t begin = clock();

	calculate_multiplication(n);

	time_t end = time(NULL);
	//clock_t end = clock();

	time_spent = (double)(end - begin); // Si se usa clock(), dividir por CLOCKS_PER_SEC
	printf("CPU time = %.7f seconds\n", time_spent);

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