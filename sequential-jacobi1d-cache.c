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

		for (i = 2; i < n; i += 2) {
			u[i] = (utmp[i-1] + utmp[i+1] + h2 * f[i]) / 2;
		}

		for (i = 3; i < n; i += 2) {
			u[i] = (utmp[i-1] + utmp[i+1] + h2 * f[i]) / 2;
		}


	}
	free(utmp);
}

void jacobi1(int nsweeps, int n, double *u, double *f)
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
		utmp[0] = (u[0] + u[1] + h2 * f[0]) / 2;
		for (i = 1; i < n-1; i+=2){
			utmp[i] = (u[i-1] + u[i+1] + h2 * f[i]) / 2;
			utmp[i+1] = (u[i] + u[i+2] + h2 * f[i+1]) / 2;
		}
		if (i == n-1){
			utmp[i] = (u[i-1] + u[i+1] + h2 * f[i]) / 2;
		}

		/* Old data in utmp; new data in u */
		u[0] = (utmp[0] + utmp[1] + h2 * f[0]) / 2;
		for (i = 1; i < n-1; i+=2){
			u[i] = (utmp[i-1] + utmp[i+1] + h2 * f[i]) / 2;
			u[i+1] = (utmp[i] + utmp[i+2] + h2 * f[i+1]) / 2;
		}
		if (i == n-1){
			u[i] = (utmp[i-1] + utmp[i+1] + h2 * f[i]) / 2;
		}
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
	int i, n, nsteps;
	double *u, *f, h;
	char *ptr, *fname;
	timing_t tstart, tend;

	/* Process arguments */
	n = strtol(argv[1], &ptr, 10); /* Convierte la entrada de CLI(str) a int */
	nsteps = strtol(argv[2], &ptr, 10);
	fname  = (argc > 3) ? argv[3] : NULL;

	h = 1.0 / n;

	/* Allocate and initialize arrays */
	u = (double *)malloc((n + 1) * sizeof(double));
	f = (double *)malloc((n + 1) * sizeof(double));
	memset(u, 0, (n + 1) * sizeof(double));
	for (i = 0; i <= n; ++i)
		f[i] = i * h;

	/* Run the solver */
	get_time(&tstart);
	jacobi1(nsteps, n, u, f);
	get_time(&tend);

	/* Run the solver */
	printf("CPU time with n %d, nsteps %d = %.7Lf seconds\n", 
		n, nsteps, timespec_diff(tstart, tend));

	if (fname)
		write_solution(n, u, fname);

	/* Libera memoria */
	free(f);
	free(u);

	return 0;
}