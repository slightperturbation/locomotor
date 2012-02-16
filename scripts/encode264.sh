#!/bin/sh

if [ -n "$1" ] 
	then
	inName=$1
else
	inName="Gen-0_Trial-0_%04d.tiff"
fi

if [ -n "$2" ] 
	then
	outName=$2
else
	outName="out.mov"
fi

if [ -n "$3" ] 
	then
	frameRate=$3
else
	frameRate="60"
fi

#ffmpeg -y -i $inName -r $frameRate -an -v 1 -vcodec libx264 -b 1000k -bt 175 -refs 1 -deblockalpha 0 -deblockbeta 0 -subq 1 -me_range 21 -level 30 -g 300 -keyint_min 30 -sc_threshold 40 -rc_eq 'blurCplx^(1-qComp)' -qcomp 0.7 -qmax 51 -cmp 1 -f mov -pass 1 /dev/null
#ffmpeg -y -i $inName -r $frameRate    -v 1 -vcodec libx264 -b 1000k -bt 175 -refs 1 -deblockalpha 0 -deblockbeta 0 -subq 6 -me_range 21 -level 30 -g 300 -keyint_min 30 -sc_threshold 40 -rc_eq 'blurCplx^(1-qComp)' -qcomp 0.7 -qmax 51 -cmp 1 -acodec aac -ab 96 -ar 48000 -ac 2 -f mov -pass 2 $outName

#ffmpeg -y -i $inName -r $frameRate -an -pass 1 -threads 0 -vpre fastfirstpass -vcodec libx264 -b 512k -f mov -pass 1 /dev/null
#ffmpeg -y -i $inName -r $frameRate     -pass 2 -threads 0 -vcodec libx264 -b 512k -f mov  $outName

#ffmpeg -y -i $inName -r $frameRate -an -threads 0 -vcodec libx264 -b 512k -f mov $outName

ffmpeg -y -r $frameRate -i $inName -vcodec libx264 -threads 0 -pass 1 -vpre fastfirstpass -b 4500k out.mov
ffmpeg -y -r $frameRate -i $inName -vcodec libx264 -threads 0 -pass 2 -vpre max -b 4500k out.mov
