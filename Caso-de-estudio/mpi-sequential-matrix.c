#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int **matrixA, **matrixB, **matrixResult;

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
	int i, j, k, rank, size, to, rows, workload, rleft, offset, n;
	double time_spent;
	struct timespec start, finish;
	double elapsed;
	
	n = strtol(argv[1], &ptr, 10);  /* Convierte la entrada de CLI(str) a int */

	/* Reserva memoria para las filas de las matrices */
	matrixA = (int **)malloc(n * sizeof(int *));
	matrixB = (int **)malloc(n * sizeof(int *));
	matrixResult = (int **)malloc(n * sizeof(int *));

	/* Reserva memoria para las columnas de las matrices */
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

	/* Inicializa MPI */
	MPI_Status status;
	MPI_Init(NULL,NULL);

	/* ID Procesos */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	/* Número de procesos */
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	/* Nodos */
	char node[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(node, &n);
	
	/* Envía matrixB a los nodos */	 
	for(i = 0; i < n; i++) {
		MPI_Bcast(&matrixB[i][0], n, MPI_INT, 0, MPI_COMM_WORLD);
	}

	/* Nodo cabeza */
	if (rank == 0) {
		/* Se toma el tiempo antes y después de las operaciones */
		clock_gettime(CLOCK_MONOTONIC, &start);

		workload = n / (size - 1);
		rleft = n % (size - 1);
		offset = 0;
		
		for (to = 1; to < size; to++) {
			if (to > rleft) {
				rows = workload;
			} else {
				rows = workload + 1;
			}

			/* Envía la carga de trabajo a cada nodo */
			MPI_Send(&rows, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
			MPI_Send(&offset, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
			for(j = offset; j < offset+rows; j++){
				MPI_Send(&matrixA[j][0], n, MPI_INT, to, 1, MPI_COMM_WORLD);
			}
			offset = offset + rows;
		}

		/* Recibe resultados */
		for (i = 1; i < size; i++){
			MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&offset, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			for(j = offset; j < offset+rows; j++) {
				MPI_Recv(&matrixResult[j][0], rows*n, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			}
		}
		
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		printf("CPU time with matrix size %d = %.5f seconds\n", n, elapsed);

	}

	/* Cálculo nodos */
	else {
		if(rank != 0) {
			printf("Receiver node %s with rank:%d\n", node, rank);

			/* Recibe carga de trabajo */
			MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			for(j = 0; j < rows; j++){
				MPI_Recv(&matrixA[j][0], n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			}

			/* Calculate multiplication */
			k = 0;
			while(k < n) {
				for (i = 0; i < rows; i++) {
					matrixResult[i][k] = 0;
					for (j = 0; j < n; j++) {
						matrixResult[i][k] = matrixResult[i][k] + matrixA[i][j] * matrixB[j][k];
					}
				}
				k++;
			}

			/* Se envian los resultados al nodo cabeza */
			MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
			MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
			for(j = 0; j < rows; j++) {
				MPI_Send(&matrixResult[j][0], n, MPI_INT, 0, 2, MPI_COMM_WORLD);
			}
		}
	}

	/* Finaliza MPI */
	MPI_Finalize();

	/*print_matrix(matrixA, n);
	print_matrix(matrixB, n);
	print_matrix(matrixResult, n);/*

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