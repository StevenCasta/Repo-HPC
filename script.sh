#! /bin/bash
for j in {1..10}
do
for a in 2 4 8 16
do
for i in 4000 4500
do
./threads-matrix.out $i $a >> times2-threads-matrix.doc
./processes-matrix.out $i $a >> times2-processes-matrix.doc
done
done
done
