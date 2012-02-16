#include "GLTools.h"
//#include <windows.h>
#include <FL/gl.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <iomanip>

// **********************************************
void saveScreenImage( const std::string& argNamePrefix, int argX, int argY )
{	
	static int ourID = 1;

	unsigned char *buffer;	

	std::stringstream filename;

	int w = argX;	
	int h = argY;	

	int buf_size = 18 + (w * h * 3);	// Header is 18 bytes

	unsigned char temp;	
	FILE *screenshotFile;	

	filename << std::setfill( '0' );
	filename << argNamePrefix << "_" << std::setw( 4 ) << ourID << ".tga";
    
	if( !(screenshotFile = fopen(filename.str().c_str(), "wb")) )
	  {
		return;
	  }
	ourID++;

	if (!(buffer = (unsigned char *) calloc(1, buf_size)))
	  {
	    std::cerr << "Unable to get memory for screenshot.\n";
		return;
	  }

	buffer[2] = 2;			// Non-compressed
	buffer[12] = w & 255;	
	buffer[13] = w >> 8;	
	buffer[14] = h & 255;	
	buffer[15] = h >> 8;	
	buffer[16] = 24;		// 24 bits per pixel

	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer + 18);

	// Convert from RGB to BGR
	for( int i = 18; i < buf_size; i += 3 )
	{
		temp = buffer[i];
		buffer[i] = buffer[i + 2];
		buffer[i + 2] = temp;
	}	

	fwrite( buffer, sizeof(unsigned char), buf_size, screenshotFile );
	fclose( screenshotFile );	
	free( buffer );
}
