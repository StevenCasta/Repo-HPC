#! /bin/bash
for j in {1..10} 
do
for i in 1500 2000 2500 3000 3500 4000 4500 5000 5500 6000
do
./sequential-matrix.out $i >> times-sequential-matrix.doc
./sequential-matrix-O3.out $i >> times-sequential-matrix-O3.doc
./sequential-matrix-transpose.out $i >> times-sequential-matrix-transpose.doc
done
done

for j in {1..10}
do
for a in 2 4 8 16
do
for i in 1500 2000 2500 3000 3500 4000 4500 5000 5500 6000
do
./threads-matrix.out $i $a >> times-threads-matrix.doc
./processes-matrix.out $i $a >> times-processes-matrix.doc
done
done
done
