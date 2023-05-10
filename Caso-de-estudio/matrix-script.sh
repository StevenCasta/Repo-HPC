#! /bin/bash
for j in {1..10}
do
for a in 2 4 8 16
do
for i in 1000 1500 2000 2500 3000 3500 4000 4500
do
./openmp-sequential-matrix.out $i $a >> times-openmp-sequential-matrix.doc
done
done
done

