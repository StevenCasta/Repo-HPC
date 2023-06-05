#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int *road;

void print_road(int *road, int n) {
	for (int i = 0; i < n; i++) {
		printf("\t%d", road[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	int rank, size, n, to, rows, workload, rleft, offset, i, j, iterations;
	double time_spent, elapsed;
	struct timespec start, finish;
	char *ptr;

	n = strtol(argv[1], &ptr, 10); /* Convierte la entrada de CLI(str) a int */
	iterations = strtol(argv[2], &ptr, 10);

	// Reserva de Memoria
	road = (int *)malloc(n * sizeof(int));

	srand(time(NULL)); /* Inicializa el generador de números aleatorios */

	/* Llena road con ceros y unos */
	for (int i = 0; i < N; i++) {
		road[i] = rand() % 2;
	}

	/* Inicializa MPI */
	MPI_Status status;
	MPI_Init(NULL, NULL);

	/* ID procesos */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Número de procesos */
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	/* Nodos */
	char node[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(node, &n);

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
				for (j = offset; j < offset + rows; j++) {
					MPI_Send(&road[j], n, MPI_INT, to, 1, MPI_COMM_WORLD);
				}
			offset = offset + rows;
		}

		/* Recibe resultados */
		for (i = 1; i < size; i++) {
			MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&offset, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			for (j = offset; j < offset + rows; j++) {
				MPI_Recv(&road[j], rows*n, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		printf("CPU time with road size %d = %.5f seconds\n", n, elapsed);

	}

	/* Cálculo de nodos */
	else {
		if (rank != 0) {
			printf("Receiver node %s with rank:%d\n", node, rank);

			/* Recibe carga de trabajo */
			MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			for (j = 0; j < rows; j++) {
			  MPI_Recv(&road[j], n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			}

			/* Simulador de tráfico */
			for (i = 0; i < iteraciones; i++) {
				for (i = 0; i < n; i++) {
					if (i+1 == n) {
						if (road[i] == 1 && road[0] == 0) {
							road[i] = 0;
							road[0] = 1;
						}
					} else {
						if (road[i] == 1 && road[i + 1] == 0) {
							road[i] = 0;
							road[i + 1] = 1;
						}
					}
				}
			}

			/* Se envian los resultados al nodo cabeza */
			MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
			MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
			for (j = 0; j < rows; j++) {
				MPI_Send(&road[j], N, MPI_INT, 0, 2, MPI_COMM_WORLD);
			}
		}
	}

	/* Finaliza MPI */
	MPI_Finalize();

	/* Libera memoria del arreglo */
	free(road);

	return 0;
}