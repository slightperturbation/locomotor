#!/bin/sh

# Usage:  spawnExper.sh count gens
#  e.g.,  spawnExper.sh 8 1000


count=8
if [ -z $1 ] ; then
	echo "`basename $0`: processCount generations rounds"
	exit -1
else
	count=$1
fi

gens=1000
if [ -z $2 ] ; then
	echo "`basename $0`: processCount generations rounds"
	exit -1
else
	gens=$2
fi

rounds=1
if [ -z $3 ] ; then
	echo "`basename $0`: processCount generations rounds"
	exit -1
else
	rounds=$3
fi

echo "Spawing $count processes for $rounds rounds, each of $gens generations."

if [ -e default.ne ] ; then
	for (( r=1; r<=rounds; r++ ))
	do		
		base=exper
		for (( i=1; i<=count; i++ ))
		do
			name=${base}_${r}_${i}
			mkdir ${name}
			cd ${name}
			mkdir data
			cp ../default.ne ./
			cp ../default.body ./
			sleep 2  # needed to stagger the timing for randomness
			evolve -g $gens 2>&1 > ${name}.log &
			cd ..
		done
		wait
		echo "FINISHED ROUND $r"
	done
fi
wait  # wait for all renders to be done
