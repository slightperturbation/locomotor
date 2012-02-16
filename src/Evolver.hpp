/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef ELSE_EVOLVER_HPP
#define ELSE_EVOLVER_HPP

// Copyright (c) 2004, Brian F. Allen,
//
// Contact: vector@acm.org
//


/** \file 
 \author Brian F. Allen (vector@acm.org)
 */

// Internal Includes
#include <Else.hpp>
#include <BodyState.hpp>

// External Includes
#include <neat/neat.h>
#include <neat/genetics.h>

#include <vector>
#include <sstream>

namespace Else 
{
    
    /// Template class for the entire evolutionary process.
    /// Most typical usage is just go()
    /// A single run can be done using singleRun()
    class Evolver
    {
    public:
        Evolver( EvaluationFactoryPtr argEvaluationFactory )
            :
            myEvaluationFactory( argEvaluationFactory ),
            myIsRunning( false ),
            myDisplayRate( 1 ),
            myUpdateRate( DEFAULT_CONTROLLER_UPDATE_RATE )
        {
            // Noop
        }
        
        virtual ~Evolver() {}
        
        
        /// Begin the execution of all the runs.
        /// Subclasses are responsible for resettiung the myIsRunning var.
        virtual void go( EvoReporter* reporter ) = 0;
        
        
        /// @TODO @FIXME
        void setUpdateRate( int arg )
        {
            myUpdateRate = arg;
        }
        
        /// @TODO @FIXME
        int getUpdateRate( void )
        {
            return myUpdateRate;
        }
        
        virtual bool isRunning( void )
        {
            return myIsRunning;
        }
        
        virtual void toggleRunning( void )
        {
            myIsRunning = !myIsRunning;
        }
        std::vector< std::vector< double > > mySensorData;
        std::vector< std::vector< double > > myActuatorData;
        std::vector< std::vector< double > > myNeuronActivationData;
        std::vector< std::vector< double > > myJointAngleData;
        std::vector< std::string > myAnimations;
    protected:
        EvaluationFactoryPtr myEvaluationFactory;

    protected:
        bool myIsRunning;
        int myDisplayRate;
        int myUpdateRate;
    };
    
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Evolver.hpp,v $
// Revision 1.13  2004/07/26 21:57:40  dexabraxis
// Release v6.  Balanced muscle strength and improved physics speed.
//
// Revision 1.12  2004/07/19 06:08:29  dexabraxis
// Added image output.
// Added web content.
//
// Revision 1.11  2004/07/19 00:30:12  dexabraxis
// Many bug fixes and better harness and Echo state net support.
//
// Revision 1.10  2004/06/28 00:08:05  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.9  2004/06/09 03:41:04  dexabraxis
// Added stubs for Echo State Networks (or "Clump" networks).
// Added combo box to select type of network/evolver.
//
// Revision 1.8  2004/05/30 20:19:58  dexabraxis
// Refactored NeatController.  Alllowed multiple runs.  Possibliy introduced bug of output # changing.
//
// Revision 1.7  2004/05/14 01:01:07  dexabraxis
// *** empty log message ***
//
// Revision 1.6  2004/05/12 09:43:47  dexabraxis
// Tweaks to the fitness function, Started on fitness graph.
//
// Revision 1.5  2004/05/08 08:33:23  dexabraxis
// Reworking body construction-- explicit.
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
//////////////////////////////////////////////////////////////////////////
