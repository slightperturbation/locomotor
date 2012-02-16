/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
//
// $Id: Main.cpp,v 1.4 2004/06/28 00:08:05 dexabraxis Exp $
// $Date: 2004/06/28 00:08:05 $
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

// Internal Includes
#include <Else.hpp>
#include <neat/neat.h>
#include <EvolverUI.hpp>
#include <Tardis.hpp>

// External Includes
#include <Fl/Fl.H>
#ifdef WIN32
#include <direct.h>
#endif

EvolverUI* gblEvolverUI;

Else::CumulativeTimer* Else::gblTotalTimer;
Else::CumulativeTimer* Else::gblPhysicsTimer;
Else::CumulativeTimer* Else::gblControlTimer;
Else::CumulativeTimer* Else::gblEmbryogenesisTimer;
Else::CumulativeTimer* Else::gblIOTimer;

int main( int argc, char* argv[] )
{
	using namespace Else;
	
	std::cerr << "ELSE - Initializing\n";
  std::ios_base::sync_with_stdio(true);

  // profilers
  gblTotalTimer = new Else::CumulativeTimer;
  gblPhysicsTimer = new Else::CumulativeTimer;
  gblControlTimer = new Else::CumulativeTimer;
  gblEmbryogenesisTimer = new Else::CumulativeTimer;
  gblIOTimer = new Else::CumulativeTimer;

  gblTotalTimer->start();

	std::cerr << "ELSE - About to create GUI\n";
  gblEvolverUI = new EvolverUI();

	std::cerr << "ELSE - About to start GUI\n";
  gblEvolverUI->show( argc, argv );

	std::cerr << "ELSE - About to write settings\n";
  std::ofstream settingsFile( "defaultSettings.txt", std::ios::app );
  settingsFile << "-----------------------------------\n"
//	  << "Build: " << __TIMESTAMP__ << "\n"
#ifdef WIN32
	  << "Directory: " << _getcwd( NULL, 0 ) << "\n--------------------------\n"
#endif
	  << "\n DEFAULT_FORCE_SCALE\t" << DEFAULT_FORCE_SCALE
	  << "\n DEFAULT_USE_FAST_STEP\t" << DEFAULT_USE_FAST_STEP
	  << "\n DEFAULT_TIME_STEP\t" << DEFAULT_TIME_STEP
	  << "\n DEFAULT_TIME_SUBSTEPS\t" << DEFAULT_TIME_SUBSTEPS
	  << "\n DEFAULT_ERP\t" << DEFAULT_ERP
	  << "\n DEFAULT_CFM\t" << DEFAULT_CFM
	  << "\n DEFAULT_CONTROLLER_UPDATE_RATE\t" << DEFAULT_CONTROLLER_UPDATE_RATE
	  << "\n DEFAULT_ALLOW_INTERNAL_COLLISIONS\t" << DEFAULT_ALLOW_INTERNAL_COLLISIONS
	  << "\n DEFAULT_STEPS_PER_EVAL\t" << DEFAULT_STEPS_PER_EVAL
	  << "\n DEFAULT_HARNESS_HEIGHT\t" << DEFAULT_HARNESS_HEIGHT
	  << "\n DEFAULT_HARNESS_GENERATIONS\t" << DEFAULT_HARNESS_GENERATIONS
	  << "\n DEFAULT_HARNESS_FORWARD_VEL\t" << DEFAULT_HARNESS_FORWARD_VEL
	  << "\n DEFAULT_HARNESS_FORWARD_FACTOR\t" << DEFAULT_HARNESS_FORWARD_FACTOR
	  << "\n DEFAULT_HARNESS_VERTICAL_FACTOR\t" << DEFAULT_HARNESS_VERTICAL_FACTOR
	  << "\n DEFAULT_WALK_REQUIRED_HEIGHT\t" << DEFAULT_WALK_REQUIRED_HEIGHT
	  << "\n DEFAULT_STEP_REQUIRED_HEIGHT\t" << DEFAULT_STEP_REQUIRED_HEIGHT
	  << "\n DEFAULT_WALK_TIME_VALUE\t" << DEFAULT_WALK_TIME_VALUE
	  << "\n DEFAULT_GRAVITY\t" << DEFAULT_GRAVITY
	  << "\n DEFAULT_ANKLE_STRENGTH\t" << DEFAULT_ANKLE_STRENGTH
	  << "\n DEFAULT_COLLISION_ERP\t" << DEFAULT_COLLISION_ERP
	  << "\n DEFAULT_COLLISION_CFM\t" << DEFAULT_COLLISION_CFM
	  << "\n DEFAULT_STABILITY_THRESHOLD\t" << DEFAULT_STABILITY_THRESHOLD
	  << "\n KNEE_TORQUE_SCALE\t" << KNEE_TORQUE_SCALE
	  << "\n HIP_TORQUE_SCALE\t" << HIP_TORQUE_SCALE
	  << "\n";
  settingsFile << "\n-------------------------------\nNEAT PARAMETERS\n\n";
  NEAT::write_neat_params( settingsFile );
  settingsFile.close();

	std::cerr << "ELSE - About to enter gui loop\n";
  Fl::run();

  gblTotalTimer->stop();

  std::ofstream timingFile( "profileTimes.txt", std::ios::app );
  timingFile << "-----------------------------\n"
	// << Build: " << __TIMESTAMP__
	  << "\nTotal Time:   " << gblTotalTimer->get() 
	  << "\nPhysics Time: " << gblPhysicsTimer->get() 
	  << "\nControl Time: " << gblControlTimer->get() 
	  << "\nEmbryo Time:  " << gblEmbryogenesisTimer->get()
	  << "\nIO Time:      " << gblIOTimer->get() 
	  << "\n";
  timingFile.close();

  delete gblTotalTimer;
  delete gblPhysicsTimer;
  delete gblControlTimer;
  delete gblEmbryogenesisTimer;
  delete gblIOTimer;
  delete gblEvolverUI;
  return 1;
}

//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Main.cpp,v $
// Revision 1.4  2004/06/28 00:08:05  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.3  2004/05/03 16:36:59  dexabraxis
// Integration with NEAT.  Basic UI in place. (possible threading bug)
//
// Revision 1.2  2004/04/30 23:29:36  dexabraxis
// *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////
