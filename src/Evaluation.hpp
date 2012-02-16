/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_EVALUATION_HPP
#define ELSE_EVALUATION_HPP
//
// $Id: Evaluation.hpp,v 1.18 2004/07/26 21:57:40 dexabraxis Exp $
// $Date: 2004/07/26 21:57:40 $
//
// Copyright (c) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//


// Project Includes
#include <Else.hpp>
#include <Controller.hpp>
#include <ArticulatedBody.hpp>
#include <PhysicsSimulator.hpp>
#include <FitnessFunc.hpp>
#include <Tardis.hpp>

namespace Else 
{

    /// Represents the Evaluation of a single organism.
    /// Holds only data that changes with the evaluation of an org, and thus must be
    /// private to the thread of execution of the evaluation.
    class Evaluation
    {
    public:

        Evaluation( int argSimulationStepLimit,
                    ControllerPtr argController,
                    FitnessFuncPtr argFitnessFunc,
                    PhysicsSimulatorPtr argSim,
                    ArticulatedBodyPtr argBody,
                    const std::vector<BodyState>& argInitialState,
                    int argDisplayRate,
                    int argUpdateRate,
                    int argHarnessGens );

        /// "Run" this evaluation, returning the final computed fitness as a FitnessFunc.
        /// argGeneration is the generation this individual is from, while
        /// argTotalGenerations is the total number of generations that will be
        /// evaluated.  These are used to compute the harness strength (and possibly
        /// other differences, such as terrain features).
        FitnessFuncPtr operator() ( int argGeneration, int argTotalGenerations );

        int getDisplayRate( void );
        void setDisplayRate( int arg );
        
        const std::vector< std::vector< BodyState > >& getStateData( void );
        const std::vector< std::vector< double > >& getSensorData( void );
        const std::vector< std::vector< double > >& getActuatorData( void );
        const std::vector< std::vector< double > >& getNeuronActivationData( void );
        const std::vector< std::vector< double > >& getJointAngleData( void );

        ControllerPtr getController( void );

        // getLastFitness
        // ...
    private:
        int mySimSteps;
        ControllerPtr myController;
        PhysicsSimulatorPtr mySim;
        ArticulatedBodyPtr myBody;
        FitnessFuncPtr myFitnessFunc;
        std::vector< BodyState > myInitialState;
        int myDisplayRate;
        int myUpdateRate;
        int myTrialCount;
        int myHarnessGens;
        std::vector< std::vector< BodyState > > myStateData;
        std::vector< std::vector< double > > mySensorData;
        std::vector< std::vector< double > > myActuatorData;
        std::vector< std::vector< double > > myNeuronActivationData;
        std::vector< std::vector< double > > myJointAngleData;
    };
    
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Evaluation.hpp,v $
// Revision 1.18  2004/07/26 21:57:40  dexabraxis
// Release v6.  Balanced muscle strength and improved physics speed.
//
// Revision 1.17  2004/07/21 20:58:43  dexabraxis
// Added a "polarity" input to tell which side was being evalutated.  Runs seem to achieve good strides by 250k evaluations.
//
// Revision 1.16  2004/07/20 05:38:25  dexabraxis
// Reduced parameter space for evolution.
// Created (for NEAT Controller) two networks per organism, each spawned from the same genome.
// Added "cross-over" between the networks (one's outputs are the other's inputs).
//
// Revision 1.15  2004/07/19 06:08:29  dexabraxis
// Added image output.
// Added web content.
//
// Revision 1.14  2004/07/19 00:30:12  dexabraxis
// Many bug fixes and better harness and Echo state net support.
//
// Revision 1.13  2004/07/11 01:34:06  dexabraxis
// Using 4-run fitness evals
//
// Revision 1.12  2004/07/10 19:20:50  dexabraxis
// Added angular damper to harness.
//
// Revision 1.11  2004/07/10 07:18:33  dexabraxis
// Added "harness" and stop on non-foot collision.
//
// Revision 1.10  2004/05/30 20:19:58  dexabraxis
// Refactored NeatController.  Alllowed multiple runs.  Possibliy introduced bug of output # changing.
//
// Revision 1.9  2004/05/14 01:01:07  dexabraxis
// *** empty log message ***
//
// Revision 1.8  2004/05/12 09:43:47  dexabraxis
// Tweaks to the fitness function, Started on fitness graph.
//
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
