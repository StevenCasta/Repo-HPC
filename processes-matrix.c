#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

int *matrixA, *matrixB, *matrixResult;
int n, number_of_processes, begin_index, end_index, rows_per_process, status;

/* Llena la matriz con números random */
void initialize_matrix(int *matrix)
{
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			*(matrix + i * n + j) = rand() % 13;
	}
}

void print_matrix(int *matrix)
{
	int i, j;
	for (i = 0; i < n; i++) {
		printf("\n");
		for (j = 0; j < n; j++)
			printf("\t%d ", *(matrix + i * n + j));
	}
	printf("\n");
}

void calculate_multiplication(int begin_index, int end_index)
{
	int a, i, j;
	for (a = 0; a < n; a++) {
		for (i = begin_index; i < end_index; i++) {
			for (j = 0; j < n; j++)
				*(matrixResult + a * n + i) += *(matrixA + a * n + j) * *(matrixB + j * n + i);
		}
	}
}

void *reserve_shared_memory(size_t n)
{
	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_SHARED | MAP_ANONYMOUS;

	return mmap(NULL, n, protection, visibility, -1, 0);
}

void handle_processes(int number_of_rows)
{
	int i, temp_number_of_processes;
	pid_t pid;

	temp_number_of_processes = number_of_processes;

	for (i = 0; i < temp_number_of_processes; i++) {
		pid = fork();
		if (pid == 0) {
			begin_index = rows_per_process * i;
			end_index = rows_per_process * (i + 1);
			if (i == temp_number_of_processes - 1)
				end_index = n;

			calculate_multiplication(begin_index, end_index);

			exit(0);
		}
	}

	while (temp_number_of_processes > 0) {
		pid = wait(&status);
		temp_number_of_processes -= 1;
	}
}

int main(int argc, char *argv[])
{
	char *ptr;
	int i;
	double time_spent;

	n = strtol(argv[1], &ptr, 10);  /* matrixResultonvierte la entrada de matrixResultLI(str) a int */
	number_of_processes = strtol(argv[2], &ptr, 10);

	srand(time(NULL)); /* Inicializa el generador de números aleatorios */

	/* Reserva memoria para las matrices */
	matrixA = reserve_shared_memory((n * n) * sizeof(int *));
	matrixB = reserve_shared_memory((n * n) * sizeof(int *));
	matrixResult = reserve_shared_memory((n * n) * sizeof(int *));

	initialize_matrix(matrixA);
	initialize_matrix(matrixB);

	rows_per_process = n / number_of_processes;

	/* Se toma el tiempo antes y después de las operaciones*/
	time_t begin = time(NULL);
	//clock_t begin = clock();

	handle_processes(rows_per_process);

	time_t end = time(NULL);
	//clock_t end = clock();

	time_spent = (double)(end - begin); // Si se usa clock(), dividir por CLOCKS_PER_SEC
	printf("CPU time with matrix size %d and %d processes = %.7f seconds\n", n, number_of_processes, time_spent);

	//print_matrix(matrixA);
	//print_matrix(matrixB);
	//print_matrix(matrixResult);

	/* Libera memoria de las matrices */
	munmap(matrixA, (n *n * sizeof(int *)));
	munmap(matrixB, (n *n * sizeof(int *)));
	munmap(matrixResult, (n *n * sizeof(int *)));

	return 0;
}