#!/bin/sh

for experDir in exper_* ; do
    fitFile="${experDir}/data/Fitness_run_0.txt"
    tail -1 "${fitFile}"
done
