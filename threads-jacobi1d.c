#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timing.h"
#include <math.h>
#include <pthread.h>

double *u;
double *f;
int nsweeps;
int n;
int filas_hilo;
int num_hilos;
int temp_num_hilos;
int hilos;
//#define hilos 2;
/* --
 * Do nsweeps sweeps of Jacobi iteration on a 1D Poisson problem
 *
 *    -u'' = f
 *
 * discretized by n+1 equally spaced mesh points on [0,1].
 * u is subject to Dirichlet boundary conditions specified in
 * the u[0] and u[n] entries of the initial vector.
 */
void *jacobi(void *t)
{
	int c = (long)t;
	int inicio = filas_hilo * c;
	int fin;

	printf("holi soy c = %d", c);
	if (c < temp_num_hilos - 1) {
		fin = filas_hilo + inicio;
	} else {
		fin = n;
	}

	int i, sweep;
	double h = 1.0 / n;
	double h2 = h * h;
	double *utmp = (double *)malloc((n + 1) * sizeof(double));

	/* Fill boundary conditions into utmp */
	utmp[0] = u[0];
	utmp[n] = u[n];

	for (sweep = 0; sweep < nsweeps; sweep += 2) {

		/* Old data in u; new data in utmp */
		for (i = inicio+1; i < fin; ++i)
			utmp[i] = (u[i - 1] + u[i + 1] + h2 * f[i]) / 2;

		/* Old data in utmp; new data in u */
		for (i = inicio+1; i < fin; ++i)
			u[i] = (utmp[i - 1] + utmp[i + 1] + h2 * f[i]) / 2;
	}
	pthread_exit((void *)t);
	free(utmp);
}

void calcular_jacobi(int n)
{
	int rc;
	long t;
	void *status;

	temp_num_hilos = hilos;
	if (n <= temp_num_hilos) {
		temp_num_hilos = n;
		filas_hilo = 1;
	} else 
		filas_hilo = (n / temp_num_hilos) + ((n % temp_num_hilos) != 0); /* ceil */


	pthread_t threads[temp_num_hilos];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (t = 0; t < temp_num_hilos; t++) {
		rc = pthread_create(&threads[t], &attr, jacobi, (void *)t);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	pthread_attr_destroy(&attr);
	for (t = 0; t < temp_num_hilos; t++) {
		rc = pthread_join(threads[t], &status);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
	}
}

void write_solution(int n, double *u, const char *fname)
{
	int i;
	double h = 1.0 / n;
	FILE *fp = fopen(fname, "w+");
	for (i = 0; i <= n; ++i)
		fprintf(fp, "%g %g\n", i * h, u[i]);
	fclose(fp);
}

int main(int argc, char **argv)
{
	int i;
	double h;
	timing_t tstart, tend;
	char *fname;

	/* Process arguments */
	n = (argc > 1) ? atoi(argv[1]) : 100;
	nsweeps = (argc > 2) ? atoi(argv[2]) : 100;
	hilos = (argc > 3) ? atoi(argv[3]) : 2;
	fname = (argc > 4) ? argv[4] : NULL;
	h = 1.0 / n;

	/* Allocate and initialize arrays */
	u = (double *)malloc((n + 1) * sizeof(double));
	f = (double *)malloc((n + 1) * sizeof(double));
	memset(u, 0, (n + 1) * sizeof(double));
	for (i = 0; i <= n; ++i)
		f[i] = i * h;

	/* Run the solver */
	get_time(&tstart);

	calcular_jacobi(n);

	get_time(&tend);

	/* Run the solver */
	printf("n: %d\n"
		 "nsteps: %d\n"
		 "Elapsed time: %Lg s\n",
		 n, nsweeps, timespec_diff(tstart, tend));

	/* Write the results */
	if (fname)
		write_solution(n, u, fname);

	free(f);
	free(u);

	return 0;
}