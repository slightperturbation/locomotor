#!/bin/sh

# Usage:  startEvolverDBServers.sh count
#  e.g.,  startEvolverDBServers.sh 8   (default is 1)

EXE=/Users/vector/src/evolverDB-build-desktop/bin/evolverDB
count=1
if [ -z $1 ] ; then
	echo "`basename $0`: processCount"
	exit -1
else
	count=$1
fi

echo "Spawning $count db-based server processes."

machinename=`hostname`
base="exper_db_$machinename"
for (( i=1; i<=count; i++ ))
do
	name="${base}_${i}"
	nohup $EXE -label "${machinename}_${i}" 2>&1 > ${name}.log &
	sleep 10
done
wait
wait