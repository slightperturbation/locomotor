/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ELSE_SIMULATION_WINDOW_HPP
#define ELSE_SIMULATION_WINDOW_HPP
//
// $Id: SimulationWindow.hpp,v 1.8.2.2 2006/04/20 09:39:51 dexabraxis Exp $
// $Date: 2006/04/20 09:39:51 $
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
#include <GLRenderer.hpp>
#include <PhysicsSimulator.hpp>

// External Includes
#include <Fl/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <Fl/gl.h>
#include <ode/ode.h>

#include <iostream>

class EvolverUI;

namespace Else
{
  static void redrawCallback( void* argSimWindow );

	/// The OpenGL Window that shows the 3d view of the current simulation.
	/// 
  class SimulationWindow : public Fl_Gl_Window {
  public:
      SimulationWindow( int argX, 
          int argY, 
          int argW, 
          int argH ) 
          : Fl_Gl_Window( argX, 
                          argY,
                          argW,
                          argH,
                          "Active Simulation" ),
          myRenderer( new GLRenderer( argW, argH ) ),
          myIsEnabled( true )
      {
std::cerr << "SimulationWindow Ctor - Start\n";
          mode( FL_RGB | FL_DEPTH | FL_DOUBLE );
std::cerr << "SimulationWindow Ctor - About to resize\n";
          myRenderer->resize( argW, argH );
std::cerr << "SimulationWindow Ctor - About to enable\n";
          enable( true );
std::cerr << "SimulationWindow Ctor - Done\n";
      }

      ~SimulationWindow( void )
      {
          enable( false );
      }

      void enable( bool argIsEnabled )
      {
          if( !mySim || !mySim->getBody() )
          {
              std::cout << "Rendering enabled, but no body or sim.\n";
          }
          if( argIsEnabled ) 
          {
              Fl::add_idle( &Else::redrawCallback, this );
          } 
          else 
          {
              Fl::remove_idle( &Else::redrawCallback, this );
          }
          myIsEnabled = argIsEnabled;
      }

    void setSim( PhysicsSimulatorPtr argSim )
    {
      mySim = argSim;
    }

    void drawString( const char* argMsg )
    {
      std::cout << argMsg;
    }

    void setCameraElevation( float argDegrees )
    {
      myRenderer->setCameraElevation( argDegrees );
    }
    void setCameraRotation( float argDegrees )
    {
      myRenderer->setCameraRotation( argDegrees );
    }

    void screenShot( const std::string& argPrefix )
    {
        saveScreenImage( argPrefix, w(), h() );
    }

    void saveAnim( const std::string& argPrefix, int argCameraPos, bool argPreview )
    {
        mySim->saveAnim( argPrefix, w(), h(), argCameraPos, argPreview );
    }

    void stopAnim( void )
    {
        mySim->stopAnim();

    }

  private:
      void draw();
        GLRendererPtr myRenderer;
        PhysicsSimulatorPtr mySim;
        bool myIsEnabled;
    };


  static void redrawCallback( void* argSimWindow )
  {
    SimulationWindow* lclWindow = (SimulationWindow*) argSimWindow;
    if( lclWindow ) 
      lclWindow->redraw();
  }

}

#endif

//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: SimulationWindow.hpp,v $
// Revision 1.8.2.2  2006/04/20 09:39:51  dexabraxis
// 1_4
//
// Revision 1.8.2.1  2006/04/17 01:29:29  dexabraxis
// *** empty log message ***
//
// Revision 1.8  2004/07/21 20:58:43  dexabraxis
// Added a "polarity" input to tell which side was being evalutated.  Runs seem to achieve good strides by 250k evaluations.
//
// Revision 1.7  2004/07/19 06:08:29  dexabraxis
// Added image output.
// Added web content.
//
// Revision 1.6  2004/06/28 00:08:06  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.5  2004/05/12 09:43:47  dexabraxis
// Tweaks to the fitness function, Started on fitness graph.
//
// Revision 1.4  2004/05/05 03:48:41  dexabraxis
// Still have initial hip rotation problem.  Using PD-controllers.
//
// Revision 1.3  2004/05/03 16:36:59  dexabraxis
// Integration with NEAT.  Basic UI in place. (possible threading bug)
//
// Revision 1.2  2004/04/30 23:29:36  dexabraxis
// *** empty log message ***
//
// Revision 1.1  2004/04/29 08:36:50  dexabraxis
// Initial import.
//
//
//////////////////////////////////////////////////////////////////////////

