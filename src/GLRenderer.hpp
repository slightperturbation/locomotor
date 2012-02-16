/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
#ifndef ELSE_GLRENDERER_HPP
#define ELSE_GLRENDERER_HPP

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


/** \file 
    \author Dexter Allen (vector@acm.org)
*/

// Project Includes
#include <Else.hpp>
#include <Renderable.hpp>

// External Includes

namespace Else 
{

  /// Responsible for managing the OpenGL display information.
  /// GLRenderer handles all the needed rendering parameters
  /// such as camera position, projection, etc.
  class GLRenderer
  {
  public:
    GLRenderer( int argWidth, int argHeight );
    ~GLRenderer( void );
    void resize( int argNewWidth, int argNewHeight );
    
		void render( RenderablePtr argRenderable );
    void render( Renderable* argRenderable );

		void setChaseObject( LocatablePtr argChase );

		void drawBox( const gmtl::Vec3d argCenterLocation,
									const gmtl::Vec3d argDimensions, 
									const double argOrientation[12] );

    void drawMarker( gmtl::Vec3d argPos );

		/// Call when finished with queueing up objects for this scene.
		/// Renders all of the queued up Renerables and clears the queue.
		void flush( void );

    void setCameraElevation( float argDegrees )
    {
      myCameraElev = argDegrees;
    }
    void setCameraRotation( float argDegrees )
    {
      myCameraRot = argDegrees;
    }
		static void reportErrors( std::string argMsg );
  private:
    float myCameraPos[3];
    float myCameraTarget[3];
    float myCameraUp[3];
    float myCameraFoV;
		float myCameraRot;
    float myCameraElev;
   
    float myCameraNearClip;
    float myCameraFarClip;

    int myScreenSize[2];

    void setupLights( float argR, float argG, float argB );

		Renderables myRenderQueue;
    std::vector< Renderable* >  myRenderPtrQueue;
		LocatablePtr myChaseObjectPtr;

		/// Taken from GLUT source.
		void drawCube( float argSize );

		void setTransform( const gmtl::Vec3d argPos, const double R[12] );
  };
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: GLRenderer.hpp,v $
// Revision 1.5  2004/05/08 08:33:23  dexabraxis
// Reworking body construction-- explicit.
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
