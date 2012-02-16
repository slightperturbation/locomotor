/*
 *  OpenGLRenderer.cpp
 *  elseViewer
 *
 *  Created by vector on 8/2/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#include "OpenGLRenderer.hpp"


void
Else::OpenGLRenderer
::render( Renderable* argRenderable )
{
    glPushMatrix();
    argRenderable->render( this );
    glPopMatrix();
}

void
Else::OpenGLRenderer
::render( RenderablePtr argRenderable )
{
    glPushMatrix();
    (argRenderable)->render( this );
    glPopMatrix();
}

void
Else::OpenGLRenderer
::drawBox( const gmtl::Vec3d argCenterLocation,
          const gmtl::Vec3d argDimensions, 
          const double argOrientation[12] )
{
	using namespace gmtl;
	glPushMatrix();
	{
		//glColor4f( 0.7f, 0.7f, 0.7f, 0.8f );
		setTransform( argCenterLocation, argOrientation );
		glScalef( argDimensions[0], 
                 argDimensions[1], 
                 argDimensions[2] );
		drawCube( 1.0 );
	}
	glPopMatrix();
}


void 
Else::OpenGLRenderer
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
Else::OpenGLRenderer
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

void 
Else::OpenGLRenderer
::drawAxis( float argSize )
{
		// Axis to show WTF is up.
		glPushMatrix();
		{
			glColor4d( 0.3, 0.3, 0.3, 1.0 );
			glTranslated( 0.0, 0.0, -0.04 );
			drawCube( argSize * 2.0 );
		}
		glPopMatrix();
		glPushMatrix();
		{
			glTranslated( 0.1, 0.0, 0.0 );
			glColor3d( 1.0, 0.0, 0.0 );
			drawCube( argSize );
		}
		glPopMatrix();
		glPushMatrix();
		{
			glTranslated( 0.0, 0.1, 0.0 );
			glColor3d( 0.0, 1.0, 0.0 );
			drawCube( argSize );
		}
		glPopMatrix();
		glPushMatrix();
		{
			glTranslated( 0.0, 0.0, 0.1 );
			glColor3d( 0.0, 0.0, 1.0 );
			drawCube( argSize );
		}
		glPopMatrix();
}

void
Else::OpenGLRenderer
::drawMarker( gmtl::Vec3d argPos )
{
    glPushMatrix();
    glTranslated( argPos[0], argPos[1], argPos[2] );
    glDisable( GL_DEPTH_TEST );
    glColor3d( 1.0, 0.0, 0.0 );
    drawCube( 0.05 );
    glEnable( GL_DEPTH_TEST );
    glPopMatrix();
}

