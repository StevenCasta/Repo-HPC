#! /bin/bash
for j in {1..10}
do
for a in 2 4 6
do
for i in 500 600 700 800 900 1000 1100 1200 1300 1400 1500
do
./sequential-matrix $i >> times-sequential-matrix.doc
./sequential-matrix-transpose $i >> times-sequential-matrix-transpose.doc
./threads-matrix $i $a >> times-threads-matrix.doc
./sequential-matrix $i $a >> times-processes-matrix.doc
done
done
done
