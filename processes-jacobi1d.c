#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "timing.h"
#include <math.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

double *u, *f;
int n, nsweeps, number_of_processes, begin_index, end_index, rows_per_process, status;

void *reserve_shared_memory(size_t n)
{
	double protection = PROT_READ | PROT_WRITE;
	double visibility = MAP_SHARED | MAP_ANONYMOUS;

	return mmap(NULL, n, protection, visibility, -1, 0);
}

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
	int i, sweep;
	double h = 1.0 / n;
	double h2 = h * h;

	double *utmp = reserve_shared_memory((n + 1) * sizeof(double *));
	/* Fill boundary conditions into utmp */
	utmp[0] = u[0];
	utmp[n] = u[n];

	for (sweep = 0; sweep < nsweeps; sweep += 2) {

		/* Old data in u; new data in utmp */
		for (i = begin_index +1 ; i < end_index; ++i)
			utmp[i] = (u[i-1] + u[i+1] + h2 * f[i]) / 2;

		/* Old data in utmp; new data in u */
		for (i = begin_index + 1; i < end_index; ++i)
			u[i] = (utmp[i-1] + utmp[i+1] + h2 * f[i]) / 2;
	}
	munmap(utmp, ((n + 1) * sizeof(double *)));
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
	int i;
	double h;
	char *ptr;
	timing_t tstart, tend;

	/* Process arguments */
	n = strtol(argv[1], &ptr, 10); /* Convierte la entrada de CLI(str) a int */
	nsweeps = strtol(argv[2], &ptr, 10);
	number_of_processes = strtol(argv[3], &ptr, 10);

	h = 1.0 / n;

	/* Reserva memoria para las matrices */
	u = reserve_shared_memory((n + 1) * sizeof(double *));
	f = reserve_shared_memory((n + 1) * sizeof(double *));
	memset(u, 0, (n + 1) * sizeof(double));
	for (i = 0; i <= n; ++i)
		f[i] = i * h;

	rows_per_process = n / number_of_processes;

	/* Se toma el tiempo antes y después de las operaciones */
	get_time(&tstart);
	handle_processes(rows_per_process);
	get_time(&tend);

	printf("CPU time with n %d, nsteps %d and %d processes = %Lg seconds\n", 
		n, nsweeps, number_of_processes, timespec_diff(tstart, tend));

	/* Libera memoria de las matrices */
	munmap(u, ((n + 1) * sizeof(double *)));
	munmap(f, ((n + 1) * sizeof(double *)));

	return 0;
}