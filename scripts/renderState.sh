#!/bin/sh

#  Use Pixie to render a single .state file
# Usage:  
#		renderState.sh statefile label
# Example:
#		renderState.sh xxx.state "25_DEC_09_exper_mutation_01"

if [ -z "$1" ]
	then
	echo "Must supply the .state file to render"
	return -1
fi
if ! test -e "$1" 
	then
	echo "Must supply the .state file to render"
	return -1
fi

stateFile="$1"

if [ -n "$2" ]
	then
	prefix="${2}_"
else
	prefix=""
fi

base=${prefix}`basename $stateFile .state`
if test -e "tmp_$base"
	then
	rm -r "tmp_$base"
fi
mkdir "tmp_$base"
ribFile=${base}.rib
stateToRIB -i "$stateFile" -o "$ribFile"
mv "$ribFile" "tmp_$base"/
cd "tmp_$base"
# optionally reduce size
parallelRender.sh $ribFile
for imgFile in *.tiff ; do
	convert "$imgFile" `basename $imgFile .tiff`.png 
done
encode264.sh Gen-0_Trial-0_%04d.png out.mov
#ffmpeg -y -i Gen-0_Trial-0_%04d.png -an -v 1 -vcodec libx264 -b 500k -bt 175 -refs 1 -deblockalpha 0 -deblockbeta 0 -subq 1 -me_range 21 -level 30 -g 300 -keyint_min 30 -sc_threshold 40 -rc_eq 'blurCplx^(1-qComp)' -qcomp 0.7 -qmax 51 -cmp 1 -s 640x480 -f mov -pass 1 /dev/null
#ffmpeg -y -i Gen-0_Trial-0_%04d.png    -v 1 -vcodec libx264 -b 500k -bt 175 -refs 1 -deblockalpha 0 -deblockbeta 0 -subq 6 -me_range 21 -level 30 -g 300 -keyint_min 30 -sc_threshold 40 -rc_eq 'blurCplx^(1-qComp)' -qcomp 0.7 -qmax 51 -cmp 1 -s 640x480 -acodec aac -ab 96 -ar 48000 -ac 2 -f mov -pass 2 out.mov
mv out.mov "${base}.mov"
uploadVideo.py "${base}.mov" "$base" "Uploaded automatically by Laetoli - neuroevolved controllers [`date`]"
mv "${base}.mov" ../
cd ..
#rm -rf tmp_$base

