#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timing.h"

void jacobi(int nsweeps, int n, double *u, double *f)
{
	int i, sweep;
	double h  = 1.0 / n;
	double h2 = h * h;
	double* utmp = (double *)malloc((n + 1) * sizeof(double));

	/* Fill boundary conditions into utmp */
	utmp[0] = u[0];
	utmp[n] = u[n];

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

int main(int argc, char *argv[])
{
	int i, n, nsteps;
	double *u, *f, h;
	char *ptr;
	timing_t tstart, tend;

	/* Process arguments */
	n = strtol(argv[1], &ptr, 10); /* Convierte la entrada de CLI(str) a int */
	nsteps = strtol(argv[2], &ptr, 10);

	h = 1.0 / n;

	/* Allocate and initialize arrays */
	u = (double *)malloc((n + 1) * sizeof(double));
	f = (double *)malloc((n + 1) * sizeof(double));
	memset(u, 0, (n + 1) * sizeof(double));
	for (i = 0; i <= n; ++i)
		f[i] = i * h;

	/* Run the solver */
	get_time(&tstart);
	jacobi(nsteps, n, u, f);
	get_time(&tend);

	/* Run the solver */
	printf("CPU time with n %d, nsteps %d = %Lg seconds\n", 
		n, nsteps, timespec_diff(tstart, tend));

	free(f);
	free(u);

	return 0;
}
