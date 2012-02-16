#!/bin/sh

if [ ! -e "$1" ]
	then
	echo "Usage: `basename $0` file.rib"
	exit
fi

for i in {1..8}
do
	echo "Starting render at frame $i, step 8, file $1"
	rndr -t:2 -t -f $i:8:9999 $1 &
done
wait
echo "All renders finished at `date`"


