#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timing.h"
#include <omp.h>

void jacobi(int nsweeps, int n, int num_threads, double *u, double *f)
{
	int i, sweep;
	double h  = 1.0 / n;
	double h2 = h * h;
	double* utmp = (double *)malloc((n + 1) * sizeof(double));

	/* Fill boundary conditions into utmp */
	utmp[0] = u[0];
	utmp[n] = u[n];

	omp_set_num_threads(num_threads);
	#pragma omp parallel for private(sweep, i) shared(utmp, u, f)
	for (sweep = 0; sweep < nsweeps; sweep += 2) {
		/* Old data in u; new data in utmp */
		for (i = 1; i < n; ++i)
			utmp[i] = (u[i-1] + u[i+1] + h2 * f[i]) / 2;

		/* Old data in utmp; new data in u */
		for (i = 1; i < n; ++i)
			u[i] = (utmp[i-1] + utmp[i+1] + h2 * f[i]) / 2;
	}
	free(utmp);
}

void write_solution(int n, double *u, const char *fname)
{
	int i;
	double h = 1.0 / n;
	FILE* fp = fopen(fname, "w+");
	for (i = 0; i <= n; ++i)
		fprintf(fp, "%g %g\n", i*h, u[i]);
	fclose(fp);
}

int main(int argc, char *argv[])
{
	int i, n, num_threads, nsteps;
	double *u, *f, h, begin_time, end_time, run_time;
	char *ptr, *fname;

	/* Process arguments */
	n = strtol(argv[1], &ptr, 10); /* Convierte la entrada de CLI(str) a int */
	nsteps = strtol(argv[2], &ptr, 10);
	num_threads = strtol(argv[3], &ptr, 10);
	fname  = (argc > 4) ? argv[4] : NULL;

	h = 1.0 / n;

	/* Allocate and initialize arrays */
	u = (double *)malloc((n + 1) * sizeof(double));
	f = (double *)malloc((n + 1) * sizeof(double));
	memset(u, 0, (n + 1) * sizeof(double));
	for (i = 0; i <= n; ++i)
		f[i] = i * h;

	/* Run the solver */
	begin_time = omp_get_wtime();
	jacobi(nsteps, n, num_threads, u, f);
	end_time = omp_get_wtime();

	/* Run the solver */
	run_time = end_time - begin_time;
	printf("CPU time with n %d, nsteps %d and %d threads = %.7f seconds\n", n, nsteps, num_threads, run_time);

	if (fname)
		write_solution(n, u, fname);

	/* Libera memoria */
	free(f);
	free(u);

	return 0;
}
