#!/bin/sh

if [ -n "$1" ]
	then
	prefix="${1}_"
else
	prefix=""
fi

for stateFile in *.state; do
	if [ -n "$prefix" ]; then 
		renderState.sh $stateFile $prefix
	else
		renderState.sh $stateFile
	fi
		
	#     base=${prefix}`basename $stateFile .state`
	#     mkdir $base
	#     ribFile=${base}.rib
	#     stateToRIB -i $stateFile -o $ribFile
	#     mv $ribFile $base/
	#     cd $base
	# # optionally reduce size
	#     rndr -t -t:16 $ribFile
	#     for imgFile in *.tiff ; do
	# 	convert $imgFile `basename $imgFile .tiff`.png 
	#     done
	# encode264.sh Gen-0_Trial-0_%04d.png out.mov
	# #ffmpeg -y -i Gen-0_Trial-0_%04d.png -an -v 1 -vcodec libx264 -b 500k -bt 175 -refs 1 -deblockalpha 0 -deblockbeta 0 -subq 1 -me_range 21 -level 30 -g 300 -keyint_min 30 -sc_threshold 40 -rc_eq 'blurCplx^(1-qComp)' -qcomp 0.7 -qmax 51 -cmp 1 -s 640x480 -f mov -pass 1 /dev/null
	# #ffmpeg -y -i Gen-0_Trial-0_%04d.png    -v 1 -vcodec libx264 -b 500k -bt 175 -refs 1 -deblockalpha 0 -deblockbeta 0 -subq 6 -me_range 21 -level 30 -g 300 -keyint_min 30 -sc_threshold 40 -rc_eq 'blurCplx^(1-qComp)' -qcomp 0.7 -qmax 51 -cmp 1 -s 640x480 -acodec aac -ab 96 -ar 48000 -ac 2 -f mov -pass 2 out.mov
	# mv out.mov ../${base}.mov
	#     cd ..
	# #rm -rf $base
	# uploadVideo.py ${base}.mov "$base" "Uploaded automatically by Laetoli - neuroevolved controllers."
done

