#! /bin/bash
for j in {1..10}
do
for a in 10000000
do
for i in 500 1000 1500 2000 2500 3000 3500 4000
do
./sequential-jacobi1d-O3.out $a $i >> times-sequential-jacobi1d-O3.doc
done
done
done

