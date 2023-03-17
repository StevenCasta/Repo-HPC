#! /bin/bash
for j in {1..10}
do
for a in 2 4 6
do
for i in 10 20 30 40 50 60 70 80 90 100
do
./test1 $i $a >> times.doc
done
done
done
