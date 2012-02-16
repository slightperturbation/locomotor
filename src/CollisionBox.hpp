/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ELSE_COLLISIONBOX_HPP
#define ELSE_COLLISIONBOX_HPP
//
// $Id: CollisionBox.hpp,v 1.4 2004/05/09 23:29:47 dexabraxis Exp $
// $Date: 2004/05/09 23:29:47 $
//
// Copyright © 2004, Dexter Allen
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
#include <Else.hpp>

// External Includes
#ifdef ELSE_GUI
#include <Fl/gl.h>
#endif

namespace Else 
{

  class CollisionBox : public Renderable, public Locatable
  {
  public:
    CollisionBox( dSpaceID argSpaceID, double argSizeX, double argSizeY, double argSizeZ )
    {
      myGeomID = dCreateBox( argSpaceID, argSizeX, argSizeY, argSizeZ );
      dGeomSetData( myGeomID, 0 );
      mySize = gmtl::Vec3d( argSizeX, argSizeY, argSizeZ );
    }

    dGeomID getGeomID()
    {
      return myGeomID;
    }

#ifdef ELSE_GUI
    void render( GLRenderer* argRenderer )
    {
      glDisable( GL_LIGHTING );
      glColor4d( 0.2, 0.2, 1.0, 0.5 );
      const dReal* p = dGeomGetPosition( myGeomID );
      const gmtl::Vec3d pos( p[0], p[1], p[2] );
      argRenderer->drawBox( pos, mySize, 
        dGeomGetRotation( myGeomID ) );
      glEnable( GL_LIGHTING );
    }
#endif
      void render( OpenGLRenderer* argRenderer )
      {
          //glDisable( GL_LIGHTING );
          //glColor4d( 0.2, 0.2, 1.0, 0.5 );
          //const dReal* p = dGeomGetPosition( myGeomID );
          //const gmtl::Vec3d pos( p[0], p[1], p[2] );
          //argRenderer->drawBox( pos, mySize, 
          //                     dGeomGetRotation( myGeomID ) );
          //glEnable( GL_LIGHTING );          
      }

    gmtl::Vec3d getPosition( void )
    {
      const dReal* p = dGeomGetPosition( myGeomID );
      return gmtl::Vec3d( p[0], p[1], p[2] );      
    }

    void setPosition( gmtl::Vec3d argNewPosition )
    {
      dGeomSetPosition( myGeomID, argNewPosition[0], 
        argNewPosition[2], argNewPosition[2] );
    }

    gmtl::Quatd getOrientation(void)
    {
      gmtl::Quatd qd;
      dQuaternion  q;
      dGeomGetQuaternion( myGeomID, q );
      qd[0] = q[0];
      qd[1] = q[1];
      qd[2] = q[2];
      qd[3] = q[3];

      return qd;
    }

    void setOrientation( gmtl::Quatd argNewOrient )
    {
      dQuaternion  q;
      for( int i = 0; i < 4; ++i ) q[i] = argNewOrient[i];
      q[0] = argNewOrient[1];
      q[1] = argNewOrient[2];
      q[2] = argNewOrient[3];
      q[3] = argNewOrient[0];

      dGeomSetQuaternion( myGeomID, q );
    }

  private:
    gmtl::Vec3d mySize;
    dGeomID myGeomID;
  };
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: CollisionBox.hpp,v $
// Revision 1.4  2004/05/09 23:29:47  dexabraxis
// Greatly improved stability.  Added clown feet.
//
// Revision 1.3  2004/05/08 08:33:22  dexabraxis
// Reworking body construction-- explicit.
//
// Revision 1.2  2004/05/05 03:48:41  dexabraxis
// Still have initial hip rotation problem.  Using PD-controllers.
//
// Revision 1.1  2004/05/03 16:36:59  dexabraxis
// Integration with NEAT.  Basic UI in place. (possible threading bug)
//
//
//////////////////////////////////////////////////////////////////////////

