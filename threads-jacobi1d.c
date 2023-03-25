#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timing.h"
#include <math.h>
#include <pthread.h>

double *u, *f;
int n, nsweeps, row_threads, num_threads, temp_num_threads;

void *calculate_jacobi(void *t)
{
	int c = (long)t;
	int begin = row_threads * c;
	int i, sweep, end;

	if (c < temp_num_threads - 1) {
		end = begin + row_threads;
	} else {
		end = n;
	}

	double h = 1.0 / n;
	double h2 = h * h;
	double *utmp = (double *)malloc((n + 1) * sizeof(double));

	/* Fill boundary conditions into utmp */
	utmp[0] = u[0];
	utmp[n] = u[n];

	for (sweep = 0; sweep < nsweeps; sweep += 2) {

		/* Old data in u; new data in utmp */
		for (i = begin +1 ; i < end; ++i)
			utmp[i] = (u[i-1] + u[i+1] + h2 * f[i]) / 2;

		/* Old data in utmp; new data in u */
		for (i = begin + 1; i < end; ++i)
			u[i] = (utmp[i-1] + utmp[i+1] + h2 * f[i]) / 2;
	}
	free(utmp);
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
		rc = pthread_create(&threads[t], &attr, calculate_jacobi, (void *)t);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
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

int main(int argc, char *argv[])
{
	int i;
	double h;
	char *ptr;
	timing_t tstart, tend;

	/* Process arguments */
	n = strtol(argv[1], &ptr, 10); /* Convierte la entrada de CLI(str) a int */
	nsweeps = strtol(argv[2], &ptr, 10);
	num_threads = strtol(argv[3], &ptr, 10);

	h = 1.0 / n;

	/* Allocate and initialize arrays */
	u = (double *)malloc((n + 1) * sizeof(double));
	f = (double *)malloc((n + 1) * sizeof(double));
	memset(u, 0, (n + 1) * sizeof(double));
	for (i = 0; i <= n; ++i)
		f[i] = i * h;

	/* Run the solver */
	get_time(&tstart);
	handle_threads(n);
	get_time(&tend);

	/* Run the solver */
	printf("CPU time with n %d, nsteps %d and %d threads = %Lg seconds\n", 
		n, nsweeps, num_threads, timespec_diff(tstart, tend));

	free(f);
	free(u);

	return 0;
}