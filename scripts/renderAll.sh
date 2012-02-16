#!/bin/sh
for stateFile in *.state; do
    base=`basename $stateFile .state`
    mkdir $base
    ribFile=${base}.rib
    stateToRIB -i $stateFile -o $ribFile
    mv $ribFile $base/
    cd $base
    rndr -t:16 $ribFile
    for imgFile in *.tiff ; do
	convert $imgFile `basename $imgFile .tiff`.png 
    done
    ffmpeg -i Gen-0_Trial-0_%04d.png -b 250k -threads 8 ${base}.mp4
    cd ..
done
