/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_ELSE_HPP
#define ELSE_ELSE_HPP
//
// $Id: Else.hpp,v 1.21.2.1 2006/04/20 08:13:36 dexabraxis Exp $
// $Date: 2006/04/20 08:13:36 $
//
// Copyright 2009, Brian F. Allen
//
// Contact: vector@acm.org


/** \file 
\brief Main header file for the Else project.
	This file will be included by all cpp files in the
	project, so be mindful of what you include.

	\author Brian F. Allen (vector@acm.org)
*/

#ifdef MSVC
#pragma warning(disable:4244 4305 4100)  // ignore useless complaints
#endif

// Needed to use OpenGL on Windows
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN  // somewhat limit Win32 pollution
#include <windows.h>
#endif

#ifdef WIN32
#define DATA_DIR "data\\"
#define TEXTURE_DATA_DIR "data\\"
#else
#define DATA_DIR "data/"
#define TEXTURE_DATA_DIR "data/"
#endif

// The default window size
#define SCREEN_WIDTH						1000
#define SCREEN_HEIGHT						1000

// Defines the maximum update time per refresh (aka step) allowed
#define UPDATE_TIME_LIMIT_SECS (0.1)

//#undef dSINGLE
#define	dDOUBLE

//Enable either walking or stepping.  Of course, this needs to be improved.
#define WALKING_DEFAULTS
//#define STEPPING_DEFAULTS

#ifdef WALKING_DEFAULTS
#define	DEFAULT_FORCE_SCALE							(100)
#define	DEFAULT_USE_FAST_STEP						(0)
#define	DEFAULT_TIME_STEP							(0.0165)
#define	DEFAULT_TIME_SUBSTEPS						(35)
#define	DEFAULT_ERP									(0.2)
#define	DEFAULT_CFM									(1e-7)
#define	DEFAULT_CONTROLLER_UPDATE_RATE	            (4)    /* should be 20 to 200 ms, measured in TIME_STEPS per update */
#define	DEFAULT_ALLOW_INTERNAL_COLLISIONS           (false)
#define	DEFAULT_STEPS_PER_EVAL			            (2571)  /* 90 secs at TIME_STEP */
#define DEFAULT_HARNESS_HEIGHT                      (1.5)
#define DEFAULT_HARNESS_GENERATIONS                 (150)  
#define DEFAULT_HARNESS_FORWARD_VEL					(0.8) // TODO -- Base on FROUDE number
#define DEFAULT_HARNESS_FORWARD_FACTOR				(1)
#define DEFAULT_HARNESS_VERTICAL_FACTOR				(1)
#define DEFAULT_HARNESS_SAGITAL_FACTOR				(1)
#define DEFAULT_WALK_REQUIRED_HEIGHT                (0.7)
#define DEFAULT_STEP_REQUIRED_HEIGHT                (0.675)
#define DEFAULT_WALK_TIME_VALUE                     (0)
#define DEFAULT_GRAVITY                             (-9.8)
#define DEFAULT_ANKLE_STRENGTH                      (3.5)  // 0.16 in evolverDB release 1.1, 3.5 in release 1.0, 0.5 for SCA06 submission.
#define DEFAULT_COLLISION_ERP                       (0.2)
#define DEFAULT_COLLISION_CFM                       (1e-7)
#define DEFAULT_STABILITY_THRESHOLD                 (0.025)  /* allowed divergence of joints in meters */
#define KNEE_TORQUE_SCALE							(1.0)
#define HIP_TORQUE_SCALE							(5.0)
#define DEFAULT_HINGE_MOTOR_TYPE					(MotorTorque)
#endif

#ifdef STEPPING_DEFAULTS
#define	DEFAULT_FORCE_SCALE							(125)
#define	DEFAULT_USE_FAST_STEP						(0)
#define	DEFAULT_TIME_STEP							(0.0165)
#define	DEFAULT_TIME_SUBSTEPS						(35)
#define	DEFAULT_ERP									(0.1)
#define	DEFAULT_CFM									(1e-7)
#define	DEFAULT_CONTROLLER_UPDATE_RATE	            (4)    /* should be 20 to 200 ms, measured in TIME_STEPS per update */
#define	DEFAULT_ALLOW_INTERNAL_COLLISIONS           (false)
#define	DEFAULT_STEPS_PER_EVAL			            (5000)  /* 90 secs at TIME_STEP */
#define DEFAULT_HARNESS_HEIGHT                      (1.5)
#define DEFAULT_HARNESS_GENERATIONS                 (150)
#define DEFAULT_FORWARD_VEL							(0.8) // TODO -- Base on FROUDE number
#define DEFAULT_FORWARD_FACTOR						(0)
#define DEFAULT_VERTICAL_FACTOR						(0)
#define DEFAULT_WALK_REQUIRED_HEIGHT                (0.7)
#define DEFAULT_STEP_REQUIRED_HEIGHT                (0.675)
#define DEFAULT_WALK_TIME_VALUE                     (0)
#define DEFAULT_GRAVITY                             (-9.8)
#define DEFAULT_ANKLE_STRENGTH                      (0.16)  // 3.5 in release 1.0, 0.5 for SCA06 submission.
#define DEFAULT_COLLISION_ERP                       (0.2)
#define DEFAULT_COLLISION_CFM                       (1e-7)
#define DEFAULT_STABILITY_THRESHOLD                 (0.05)  /* allowed divergence of joints in meters */
#define KNEE_TORQUE_SCALE							(0.5)
#define HIP_TORQUE_SCALE							(10.0)
#endif


#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>
#include <gmtl/gmtl.h>
#include <vector>

// external forward decl's
	class NEAT;
class EvolverUI;

namespace Else
{
	// forward decl's
	class PhysicalWorld;
	class OpenGLRenderer;
	class Renderable;
	class Locatable;
	class ArticulatedBody;
    class HumanoidArticulatedBody;
    class AcrobotArticulatedBody;
    class ArticulatedBodyFactory;

    class Controller;
    class ControllerFactory;

	class ClumpController;
	class ClumpNetwork;
	class ClumpOrganism;
	class ClumpGenome;
	class ClumpPopulation;
	class ClumpLink;
	class Neuron;

	class Evaluation;
    class EvaluationFactory;

	class FitnessFuncFactory;
    class PopulationFitness;
	class FitnessFunc;
	class BalanceFitnessFunc;
	class ConstantFitnessFunc;
	class WalkFitnessFunc;
    class StrategyFitnessFunc;

	class PhysicsSimulator;
    class PhysicsSimulatorFactory;
	class Evolver;
	class SimulationWindow;

	class ElseException;
	class Tardis;
	class CumulativeTimer;
    
    class EvoReporter;

// typedefs

	typedef double										Time;

	typedef boost::shared_ptr< PhysicalWorld >          PhysicalWorldPtr;
	typedef boost::shared_ptr< OpenGLRenderer >         OpenGLRendererPtr;
	typedef boost::shared_ptr< Renderable >             RenderablePtr;
	typedef boost::shared_ptr< Locatable >              LocatablePtr;
	typedef boost::shared_ptr< ArticulatedBody >        ArticulatedBodyPtr;
    typedef boost::shared_ptr< HumanoidArticulatedBody > HumanoidArticulatedBodyPtr;
    typedef boost::shared_ptr< AcrobotArticulatedBody > AcrobotArticulatedBodyPtr;
    typedef boost::shared_ptr< ArticulatedBodyFactory > ArticulatedBodyFactoryPtr;
	typedef boost::shared_ptr< Controller >             ControllerPtr;
    typedef boost::shared_ptr< ControllerFactory >      ControllerFactoryPtr;
	typedef boost::shared_ptr< PhysicsSimulator >       PhysicsSimulatorPtr;
    typedef boost::shared_ptr< PhysicsSimulatorFactory > PhysicsSimulatorFactoryPtr;
	typedef boost::shared_ptr< FitnessFunc >            FitnessFuncPtr;
	typedef boost::shared_ptr< FitnessFuncFactory >     FitnessFuncFactoryPtr;
    typedef boost::shared_ptr< PopulationFitness >      PopulationFitnessPtr;
	typedef boost::shared_ptr< Evolver >                EvolverPtr;
    typedef boost::shared_ptr< Evaluation >             EvaluationPtr;
    typedef boost::shared_ptr< EvaluationFactory >      EvaluationFactoryPtr;
    typedef boost::shared_ptr< ClumpGenome >            ClumpGenomePtr;
	typedef boost::shared_ptr< ClumpNetwork >           ClumpNetworkPtr;
	typedef boost::shared_ptr< ClumpOrganism >          ClumpOrganismPtr;

	typedef std::vector< RenderablePtr >                Renderables;
	typedef std::vector< ArticulatedBodyPtr >           ArticulatedBodies;
	typedef std::vector< LocatablePtr >                 Locatables;
}


#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Else.hpp,v $
// Revision 1.21.2.1  2006/04/20 08:13:36  dexabraxis
// Descreased hip width
// updated harness force to be uni-directional
// increased min height
// changed rendering angle
// added support for "preview" render (w/o GUI switch though)
//
// Revision 1.21  2004/07/26 21:57:40  dexabraxis
// Release v6.  Balanced muscle strength and improved physics speed.
//
// Revision 1.20  2004/07/21 20:58:43  dexabraxis
// Added a "polarity" input to tell which side was being evalutated.  Runs seem to achieve good strides by 250k evaluations.
//
// Revision 1.19  2004/07/20 05:38:25  dexabraxis
// Reduced parameter space for evolution.
// Created (for NEAT Controller) two networks per organism, each spawned from the same genome.
// Added "cross-over" between the networks (one's outputs are the other's inputs).
//
// Revision 1.18  2004/07/20 04:26:58  dexabraxis
// Port to linux
//
// Revision 1.17  2004/07/19 18:21:56  dexabraxis
// Tweaks to physics parameters.  Added code for saving images.
//
// Revision 1.16  2004/07/19 00:30:12  dexabraxis
// Many bug fixes and better harness and Echo state net support.
//
// Revision 1.15  2004/07/11 01:34:06  dexabraxis
// Using 4-run fitness evals
//
// Revision 1.14  2004/07/10 19:20:50  dexabraxis
// Added angular damper to harness.
//
// Revision 1.13  2004/07/10 07:18:33  dexabraxis
// Added "harness" and stop on non-foot collision.
//
// Revision 1.12  2004/06/28 01:51:17  dexabraxis
// Adding initial preliminary version of echo state nets.
//
// Revision 1.11  2004/06/10 15:10:00  dexabraxis
// Added "ClumpNetwork" basics (no evolution yet)
//
// Revision 1.10  2004/06/09 03:41:04  dexabraxis
// Added stubs for Echo State Networks (or "Clump" networks).
// Added combo box to select type of network/evolver.
//
// Revision 1.9  2004/06/01 05:43:50  dexabraxis
// Fixed bug with single genome "playback"
//
// Revision 1.8  2004/05/11 21:17:13  dexabraxis
// *** empty log message ***
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
