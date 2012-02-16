#!/usr/bin/python

import sys
import gdata.youtube
import gdata.youtube.service

def uploadVideo( filename, title, desc ):
	clientId = 'ytapi-EvoComp-1'
	yt_service = gdata.youtube.service.YouTubeService( email='chokma@gmail.com', password='pantaRhei')
	yt_service.developer_key = 'AI39si6hBmhZUcgy06AS3c2m4yDt6B-_lnsNAyvJiMSnfRxjhh81HH3-ztJXKl9MMSkoJlQqp4o022NDrKHzBJLh2PQpXjFuTQ'
	yt_service.client_id = clientId
	#yt_service.email = 'brian.foster.allen@gmail.com'
	#yt_service.password = 'pantaRhei'
	token = 'AIwbFAS9PLtVfAlbB_suiBcJf8KrWuV_yzd-OJ8qC5D492r3dmAn4S715K7QyDeIbUSX3vz8rQVcu9WgPAGgqlv7VOKBawelFNXIcdFZnv6ZkCea01BMLZv2DvEewRZLBXvUlwgZHilVy43_vaWn_nStvmsBxpP35A'
	yt_service.SetClientLoginToken(token)
	#yt_service.ProgrammaticLogin()
	#token = yt_service.GetClientLoginToken()
	#print "token", token
	mediaGroup = gdata.media.Group(
	  title = gdata.media.Title(text = title),
	  description = gdata.media.Description(description_type='plain',
	                                        text=desc),
	  keywords = gdata.media.Keywords(text='neuroevolution, bipedal robot, physical animation, Project Laetoli'),
	  category = [gdata.media.Category(
	      text='Film',
	      scheme='http://gdata.youtube.com/schemas/2007/categories.cat',
	      label='Film & Animation')],
	  player=None
	)
	videoEntry = gdata.youtube.YouTubeVideoEntry(media=mediaGroup)
	yt_service.InsertVideoEntry( video_entry = videoEntry, filename_or_handle = filename )

if __name__ == "__main__":
	if( len(sys.argv) != 4 ):
		print "Usage: uploadVideo.py filename title desc"
		exit(0)
	uploadVideo( sys.argv[1], sys.argv[2], sys.argv[3] )
	