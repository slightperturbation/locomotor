for movName in *.mov ; do
	base=`basename $movName .mov`
	uploadVideo.py ${base}.mov "$base" "Uploaded automatically by Laetoli - neuroevolved controllers."
done
