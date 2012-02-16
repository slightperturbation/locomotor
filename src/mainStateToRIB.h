/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
#ifndef _MAINSTATETORIB_H
#define _MAINSTATETORIB_H
/*
 *  mainStateToRIB.h
 *  else
 *
 *  Created by Brian F. Allen on 7/3/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */

// Internal Includes
#define ELSE_CONSOLE_BUILD

#include <Else.hpp>
#include <Tardis.hpp>
#include <PhysicsSimulator.hpp>
#include <ArticulatedBody.hpp>
#include <Renderer.hpp>
#include <RibRenderer.hpp>
#include <fstream>

#include <string>
#include <sstream>
Else::CumulativeTimer* Else::gblTotalTimer;
Else::CumulativeTimer* Else::gblPhysicsTimer;
Else::CumulativeTimer* Else::gblControlTimer;
Else::CumulativeTimer* Else::gblEmbryogenesisTimer;
Else::CumulativeTimer* Else::gblIOTimer;

using namespace Else;
using namespace laetoli;
using namespace std;

int main( int argc, char* argv[] )
{
    gblTotalTimer = new Else::CumulativeTimer;
	gblPhysicsTimer = new Else::CumulativeTimer;
	gblControlTimer = new Else::CumulativeTimer;
	gblEmbryogenesisTimer = new Else::CumulativeTimer;
	gblIOTimer = new Else::CumulativeTimer;
    
    
    int camera = 0;
    std::string filename( "out.rib" );
    std::string inFilename( "in.state" );
    if( argc > 2 )
    {
        for( int i = 0; i < argc - 1; ++i )
        {
            std::string arg( argv[i] );
            if( arg == "-c" )
            {
                camera = atoi( argv[i+1] );
            }
            if( arg == "-o" )
            {
                filename = argv[i+1];
            }
            if( arg == "-i" )
            {
                inFilename = argv[i+1];
            }
        }
    }
    
    // std::cin gets the state file
    // std::cout puts the RIB
    
    vector< vector< BodyState > > anim;
    std::ifstream inFile( inFilename.c_str() );
    inFile >> anim;
    
    PhysicsSimulator sim;
    FileRibRenderer rib( filename, camera );
    ArticulatedBody body( sim.getWorldID(), sim.getSpaceID() /*, body desc filename */ );
    sim.setBody( &body );
    sim.addRenderer( &rib );
    sim.begin();
    
    int frame = 1;
    for( vector< vector< BodyState > >::const_iterator iter = anim.begin(); iter != anim.end(); ++iter )
    {
        body.setBodyState( *iter );
        rib.beginFrame( frame++ );
        rib.render( &body );
        rib.endFrame();
    }
    
    sim.end();
    delete gblTotalTimer;
	delete gblPhysicsTimer;
	delete gblControlTimer;
	delete gblEmbryogenesisTimer;
	delete gblIOTimer;
    return 0;
}

#endif
