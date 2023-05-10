#! /bin/bash
for j in {1..10}
do
for a in 2 4 8 16
do
for i in 500 1000 1500 2000 2500 3000 3500 4000
do
./openmp-sequential-jacobi1d.out 10000000 $i $a >> times-openmp-sequential-jacobi1d.doc
done
done
done

