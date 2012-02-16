/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

// Copyright © 2004, Dexter Allen, 
//
// Contact: vector@acm.org
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


// Project Includes
#include <GLRenderer.hpp>
#include <Locatable.hpp>

// External Includes
#include <FL/gl.h>
#include <FL/glu.h>
#include <iostream>
#include <cstdio>

Else::GLRenderer::GLRenderer( int argWidth, int argHeight )
{
std::cerr << "GLRenderer CTor\n";
  myCameraPos[0] = 2.5;
  myCameraPos[1] = 2.5;
  myCameraPos[2] = 1.0;
  
  myCameraTarget[0] = 0.0;
  myCameraTarget[1] = 0.0;
  myCameraTarget[2] = 1.0;

  myCameraUp[0] = 0.0;
  myCameraUp[1] = 0.0;
  myCameraUp[2] = 1.0;

	myCameraRot = 0.0;
  myCameraElev = 0.0;
  
  myScreenSize[0] = argWidth;
	myScreenSize[1] = argHeight;

	myCameraNearClip = 0.01;
	myCameraFarClip  = 50.0;
	myCameraFoV = 70.0;
std::cerr << "GLRenderer CTor - Done\n";
}

Else::GLRenderer::~GLRenderer( void )
{
}

void Else::GLRenderer::resize( int argNewWidth,
																 int argNewHeight )
{
std::cerr << "GLRenderer Resize to " << argNewWidth << ", " << argNewHeight << "\n";
	
	if( !argNewHeight ) argNewHeight = 1; // ensure not zero

	myScreenSize[0] = argNewWidth;
	myScreenSize[1] = argNewHeight; 

std::cerr << "GLRenderer about to glViewport\n";
	glViewport( 0, 0, argNewWidth, argNewHeight );
std::cerr << "GLRenderer about to glMatrixMode\n";
	glMatrixMode( GL_PROJECTION );
std::cerr << "GLRenderer about to glLoadIdentity\n";
	glLoadIdentity();
std::cerr << "GLRenderer about to gluPerspective\n";
	gluPerspective( myCameraFoV,
		(GLfloat)argNewWidth / (GLfloat)argNewHeight, // Aspect Ratio
		myCameraNearClip,
		myCameraFarClip );
std::cerr << "GLRenderer Resize Done\n";
}

void Else::GLRenderer::setupLights( float argR, float argG, float argB )
{
	glEnable( GL_LIGHTING );

	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHT1 );

	// light attributes
	const GLfloat light_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
	//  const GLfloat light_diffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	const GLfloat light_diffuse[]  = { argR, argG, argB, 1.0f };
	const GLfloat light_specular[] = { 0.1f, 0.1f, 0.1f, 0.5f };

	const GLfloat light_ambient1[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
	const GLfloat light_diffuse1[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	const GLfloat light_specular1[] = { 0.1f, 0.1f, 0.1f, 1.0f };

	// setup the light attributes
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);

	// set the light position
	GLfloat lightPosition[] = { 0.0f, -1.0f, 10.0f, 1.0f };
	GLfloat lightPosition1[] = { 10.0f, 15.0f, 100.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
}



void
Else::GLRenderer
::drawBox( const gmtl::Vec3d argCenterLocation,
					 const gmtl::Vec3d argDimensions, 
					 const double argOrientation[12] )
{
	using namespace gmtl;
	glPushMatrix();
	{
		glColor4f( 0.7f, 0.7f, 0.7f, 0.8f );
		setTransform( argCenterLocation, argOrientation );
		glScalef( argDimensions[0], 
			argDimensions[1], 
			argDimensions[2] );
		drawCube( 1.0 );
	}
	glPopMatrix();
}

void 
Else::GLRenderer
::setTransform( const gmtl::Vec3d argPos, const double R[12] )
{
	GLfloat matrix[16];
	matrix[0]=R[0];
	matrix[1]=R[4];
	matrix[2]=R[8];
	matrix[3]=0;
	matrix[4]=R[1];
	matrix[5]=R[5];
	matrix[6]=R[9];
	matrix[7]=0;
	matrix[8]=R[2];
	matrix[9]=R[6];
	matrix[10]=R[10];
	matrix[11]=0;
	matrix[12]=(double)argPos[0];
	matrix[13]=(double)argPos[1];
	matrix[14]=(double)argPos[2];
	matrix[15]=1;
	glMultMatrixf (matrix);
}


/// Taken from GLUT source.
void
Else::GLRenderer
::drawCube( GLfloat argSize )
{
	static GLfloat n[6][3] = {
		{-1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, -1.0}
	};
	static GLint faces[6][4] = {
		{0, 1, 2, 3},
		{3, 2, 6, 7},
		{7, 6, 5, 4},
		{4, 5, 1, 0},
		{5, 6, 2, 1},
		{7, 4, 0, 3}
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -argSize / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] =  argSize / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -argSize / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] =  argSize / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -argSize / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] =  argSize / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}

}

void Else::GLRenderer::flush( void )
{
	// clear all the buffers
	glClearColor( 0.2f, 0.2f, 0.2f, 0.2f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // don't need to clear the color, due to skybox
	glDisable(GL_TEXTURE_2D);
	glEnable( GL_DEPTH_TEST );
	glShadeModel( GL_FLAT );

	
	// set the model transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set camera position
	gluLookAt( myCameraPos[0],    myCameraPos[1],    myCameraPos[2],
		myCameraTarget[0], myCameraTarget[1], myCameraTarget[2], 
		myCameraUp[0],     myCameraUp[1],     myCameraUp[2] );
	

	//glEnableClientState( GL_VERTEX_ARRAY );
	//glEnableClientState( GL_NORMAL_ARRAY );

	glRotatef( myCameraRot, 0, 0, 1 );
  glRotatef( -1 * myCameraElev, 1, 0, 0 );
  
	//myCameraRot += 1.0;
	//glRotatef( myCameraRot/10.0, 1.0, 0.0, 0.0 );

	if( myChaseObjectPtr )
	{
		const gmtl::Point3d& pos = myChaseObjectPtr->getPosition();
		const gmtl::Quatd& orient = myChaseObjectPtr->getOrientation();

		gmtl::AxisAngled aAngle = gmtl::make< gmtl::AxisAngled > ( orient );
		gmtl::Vec3d axis = aAngle.getAxis();
		double angle = gmtl::Math::rad2Deg( aAngle.getAngle() );
		glRotated( -1 * angle, axis[0], axis[1], axis[2] );
		glTranslated( -1 * pos[0], -1 * pos[1], -1 * pos[2] );
	}

	setupLights( 0.4f, 0.4f, 0.4f );

	// queued up structures
	for( Renderables::iterator i = myRenderQueue.begin(); 
		i != myRenderQueue.end(); ++i )
	{
		glPushMatrix();
		(*i)->render( this );
		glPopMatrix();
	}

  //  queued up pointer structures
  for( std::vector< Renderable* >::iterator i = myRenderPtrQueue.begin(); 
    i != myRenderPtrQueue.end(); ++i )
  {
    glPushMatrix();
    (*i)->render( this );
    glPopMatrix();
  }


	{
		// Axis to show WTF is up.
		glDisable( GL_LIGHTING );
		glPushMatrix();
		{
			glColor4d( 0.3, 0.3, 0.3, 1.0 );
			glTranslated( 0.0, 0.0, -0.04 );
			drawCube( 0.05 );
		}
		glPopMatrix();
		glPushMatrix();
		{
			glTranslated( 0.1, 0.0, 0.0 );
			glColor3d( 1.0, 0.0, 0.0 );
			drawCube( 0.02 );
		}
		glPopMatrix();
		glPushMatrix();
		{
			glTranslated( 0.0, 0.1, 0.0 );
			glColor3d( 0.0, 1.0, 0.0 );
			drawCube( 0.02 );
		}
		glPopMatrix();
		glPushMatrix();
		{
			glTranslated( 0.0, 0.0, 0.1 );
			glColor3d( 0.0, 0.0, 1.0 );
			drawCube( 0.02 );
		}
		glPopMatrix();
	}

	myRenderQueue.clear();
  myRenderPtrQueue.clear();
	reportErrors( "End of Rendering" );
}

void
Else::GLRenderer
::drawMarker( gmtl::Vec3d argPos )
{
  glPushMatrix();
  glTranslated( argPos[0], argPos[1], argPos[2] );
  glDisable( GL_DEPTH_TEST );

  drawCube( 0.05 );
  glEnable( GL_DEPTH_TEST );
  glPopMatrix();
}

void 
Else::GLRenderer
::setChaseObject( LocatablePtr argChase )
{
	myChaseObjectPtr = argChase;
}


void 
Else::GLRenderer
::reportErrors( std::string argMsg )
{
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString( errCode );
		fprintf( stderr, "OpenGL Error: %s: %s\n", argMsg.c_str(), errString );
	}
}

void 
Else::GLRenderer
:: render( RenderablePtr argRenderable )
{
	myRenderQueue.push_back( argRenderable );
}

void 
Else::GLRenderer
:: render( Renderable* argRenderable )
{
  myRenderPtrQueue.push_back( argRenderable );
}

//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: GLRenderer.cpp,v $
// Revision 1.7  2004/05/09 23:29:47  dexabraxis
// Greatly improved stability.  Added clown feet.
//
// Revision 1.6  2004/05/08 08:33:23  dexabraxis
// Reworking body construction-- explicit.
//
// Revision 1.5  2004/05/05 03:48:41  dexabraxis
// Still have initial hip rotation problem.  Using PD-controllers.
//
// Revision 1.4  2004/05/03 20:05:32  dexabraxis
// Fixed occasional deadlock/crash bug.  Still slow during viz.
//
// Revision 1.3  2004/05/03 16:36:59  dexabraxis
// Integration with NEAT.  Basic UI in place. (possible threading bug)
//
// Revision 1.2  2004/04/30 23:29:36  dexabraxis
// *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////
