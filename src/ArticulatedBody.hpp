/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef ELSE_ARTICULATED_BODY_HPP
#define ELSE_ARTICULATED_BODY_HPP

// Copyright (C) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//

/** \file 
 \author Brian F. Allen (vector@acm.org)
 */

// Project Includes
#include <Else.hpp>
#include <Renderable.hpp>
#include <Locatable.hpp>
#include <RibRenderer.hpp>
#include <BodyState.hpp>

// External Includes
#include <ode/ode.h>
#include <cassert>
#include <map>
#include <vector>

#ifdef COCOA_GUI
#include <OpenGLRenderer.hpp>
#endif

#ifdef ELSE_GUI
#include <FL/gl.h>
#include <GLRenderer.hpp>
#endif 

#include <fstream>

#define BODY_FILE_NAME "default.body"
#define JOINT_TRACE_FILE_NAME "jointTrace.txt"
#define NEURAL_TRACE_FILE_NAME "neuralTrace.txt"
#define DEFAULT_BODY_HEIGHT (1.75)
#define DEFAULT_BODY_WEIGHT (75.0)
#define DEFAULT_HIP_BREADTH (0.1625)  


// 0.145 = 29cm bi-iliac breadth was mean of 1946 Army males
// 0.1625 = 32.5cm 95th percentile of 1946 Army males
// 0.1045 = 20.9cm, 5th percentile of 1946 Army females 
#define CONTROL_KNEE_ACTUATOR (false)
#define CONTROL_SPINE_ACTUATOR (true)
#define USING_CROSS_OVER_COMMUNICATION (true)

namespace Else
{	


    /// Abstract class for the physically simulated articulated body controlled
    /// by a Controller, and evaluated by a FitnessFunc.
    /// Although loosely coupled to PhysicsSimulator, the ArticulatedBody
    /// is actually uniquely tied to the Simulator used in construction.
    /// You cannot use an articulatedBody built on one simulator in a different simulator.
    class ArticulatedBody : public Locatable, public Renderable
    {
    public:
        ArticulatedBody( dWorldID argWorldID, dSpaceID argSpaceID );
        virtual ~ArticulatedBody();
                
        /// Fills outSensors with the data gathered from the sensors.
        virtual void getSensors( ControllerPtr argController, double argDeltaTime ) = 0;
        
        /// Provides new values that the body should use to control it's actuators.
        virtual void setActuators( const ControllerPtr argController ) = 0;
        
        virtual void writeCurrJointTraces( double argT ) = 0;
        virtual void acquireJointAngleData( std::vector< double >& out ) = 0;
        
        /// Build a new body with the specified overall proportions.
        virtual void createBody( void ) = 0;
        
        bool isInSimulator( dWorldID argWorldID ) { return myWorldID == argWorldID; }
        
        
        /// Nodestructivly adds the body states of this body to the outStates vector.
        /// After calling, outStates will have myBodies.size() additional records
        /// containing the state of each owned body segment.
        void acquireState( std::vector< std::vector< BodyState > >& outStates );
        
        /// Sets the states of each body to the states specified in the argument.
        /// Note that if a body segment doesn't have a corresponding (id-wise) BodyState
        /// in the argStates vector, then it will not be modified.
        void setState( const std::vector< BodyState >& argStates );
        
        /// Sets argument "out" to the state of the body segment indicated by argBody
        void getBodyState( BodySegmentEnum argBody, BodyState& out );
        
        /// Sets the state of the body segment indicated by argBodyState.id to 
        /// the state of argBodyState.  Only that body segement is changed.
        void setBodyState( const BodyState& argBodyState );
        
        void setBodyState( const std::vector< BodyState >& argBodyState );
        

        /*
        void recordJointTraces( const std::string& argFileName )
        {
            std::string filename( argFileName );
            if( filename.empty() )
            {
                filename = JOINT_TRACE_FILE_NAME;
            }
            myJointTraceFile.open( filename.c_str() );
            myJointTraceFile << "# T RHX RHZ RK RAX RAZ LHX LHZ LK LAX LAZ WX WY WZ WQW WQX WQY WQZ\n";
            myJointTraceFile << "# 1  2   3   4  5   6   7   8   9  10  11 12 13 14  15  16  17  18\n";
        }
        void endJointTraces( void )
        {
            myJointTraceFile.close();
        }
        
        void recordNeuralTraces( const std::string& argFileName )
        {
            std::string filename( argFileName );
            if( filename.empty() )
            {
                filename = NEURAL_TRACE_FILE_NAME;
            }
            myNeuralTraceFile.open( filename.c_str() );
        }
        void endNeuralTraces( void )
        {
            myNeuralTraceFile.close();
        }        
        */
        void setHasController( bool arg )
        {
            myHasController = arg;
        }
        
        void setForceScale( double argScale )
        {
            myForceScale = argScale;
        }

        gmtl::Vec3d getPosition( void )
        {
            const dReal* p = dBodyGetPosition( myBodies[Waist] );
            return gmtl::Vec3d( p[0], p[1], p[2] );      
        }	
        
        gmtl::Vec3d getUpVector( void )
        {
            dVector3 result;
            dBodyVectorToWorld( myBodies[Trunk], 0, 0, 1, result);
            return gmtl::Vec3d( result[0], result[1], result[2] );
        }
        
        gmtl::Quatd getOrientation(void);
        
        void recordCollision( BodySegmentEnum argCollidingBodySegment, double argTime );
        
        gmtl::Point3d getLastCollision( BodySegmentEnum argBody );

        // TODO: move to HumanoidAB
        SideEnum getSideOfLastCollision( void );
        
        gmtl::Point3d getPointOfLastFootCollision( void );
        
        double getAppliedHarness( void )
        {
            return myAppliedHarness;
        }
                
        // TODO: move to HumanoidAB
        bool hasNonFootCollision( void );
        
        void manuallyOverrideActuator( int argActuatorID, double argSetting );
        
        /// Set all of the segments and joints to "starting" positions
        void reset( void );        
        void clear( void );        
        
        void render( laetoli::Renderer* argRenderer );
        
        void render( OpenGLRenderer* argRenderer );
        
        bool testStability( void );
        
        

        static double unitNumber( std::vector< double >::const_iterator arg );
        
        double scaleActuator( double argControllerOut, double argMin, double argMax );
        
        /// returns the needed force (really ode-velocity term) needed
        /// to move the actuator from the curr position to the desired.
        /// Note that this implements a "PD" and not a "PID" controller.
        /// The main promblem with missing the Integral term is an increased
        /// vibration and longer settling time.
        double computePD( double argCurrPos,
                          double argCurrVel,
                          double argDesiredPos,
                          double k_p, double k_d );
        
        /// Sets the need motor velocity to maintain the desired extension ratio.
        /// Extension ratio is 0 to 1, where 0 is min angle, 1 is max extension.
        void maintainAMotorAngle( dJointID argJointID,
                                  int argAxis,
                                  double argDesiredExtension );
        
        void actuateHinge( dJointID argHingeJointID, double argDesiredNorm );
        
        void actuateHingeVel( dJointID argHingeJointID,
                              double argDesiredNormVelocity );   
        
        void actuateHingeTorque( dJointID argHingeJointID,
								 double argDesiredNormTorque );
        
        void actuateHingePos( dJointID argHingeJointID,
                              double argDesiredExtension ); 
        
        void setHarnessHeight( double argHarnessHeight )
        {
            myHarnessHeight = argHarnessHeight;
        }
        
        
        void addHarnessForce( double argStrength, double argTimeStep );

		
        /// All inputs are in Unit-Male Normalized coordinates.
        /// argBottom, 
        dBodyID createVerticalSegment(BodySegmentEnum argSegmentName,
                                      double argLateralDist, 
                                      double argCenter, 
                                      double argWidth, 
                                      double argHeight, 
                                      double argWeight,
                                      double argLength = 0,
                                      double argLengthOffset = 0 );
        
        friend class FitnessFunc;
        friend class BalanceFitnessFunc;
        friend class WalkFitnessFunc;
        friend class VelocityFitnessFunc;
        friend class StepFitnessFunc;

    protected:
        dWorldID myWorldID;
        dSpaceID mySpaceID;
        
        typedef std::map< BodyJointEnum, dJointID > JointMap;
        typedef std::map< BodySegmentEnum, dBodyID > BodyMap;
        typedef std::map< BodySegmentEnum, gmtl::Point3d > CollisionPointMap;
        typedef std::map< BodySegmentEnum, double > CollisionTimeMap;
        typedef std::vector< dGeomID > GeomVec;
        typedef std::map< BodySegmentEnum, gmtl::Vec3d > BodyDimensions;
        
        typedef std::map< BodySegmentEnum, dJointID > HarnessJointMap;
        
        BodyMap myBodies;
        JointMap myJoints;
        JointMap myMotors;
        HarnessJointMap myHarnessJoints;
        
        GeomVec myGeoms;
		CollisionPointMap myCollisionPoints;
        CollisionTimeMap myCollisionTimes;
        
        dJointGroupID myJointGroupID;
        BodyDimensions myBodyDims;
        
        MotorTypeEnum myHingeMotorType;
        
        
        int myOverrideActuatorID;
        double myOverrideActuatorSetting;
        
        double myForceScale;
        bool myHasController;
        double myHarnessHeight;
        
        // the "raw cost", fitness-wise, of amount of force applied by the harness
        double myAppliedHarness;
        
        std::map< dJointID, double > myPrevJointPos;
        double myLastTimeStep;
        
        std::vector< double > myActuatorActivation;
        
        bool myIsUnstable;
        
        std::ofstream myJointTraceFile;
        std::ofstream myNeuralTraceFile;
    };
}

//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: ArticulatedBody.hpp,v $
// Revision 1.32.2.2  2006/04/20 08:13:36  dexabraxis
// Descreased hip width
// updated harness force to be uni-directional
// increased min height
// changed rendering angle
// added support for "preview" render (w/o GUI switch though)
//
// Revision 1.32.2.1  2006/04/17 01:29:29  dexabraxis
// *** empty log message ***
//
// Revision 1.32  2004/07/26 21:57:40  dexabraxis
// Release v6.  Balanced muscle strength and improved physics speed.
//
// Revision 1.31  2004/07/21 21:08:28  dexabraxis
// Linux/gcc port
//
// Revision 1.30  2004/07/21 20:58:43  dexabraxis
// Added a "polarity" input to tell which side was being evalutated.  Runs seem to achieve good strides by 250k evaluations.
//
// Revision 1.29  2004/07/20 05:38:25  dexabraxis
// Reduced parameter space for evolution.
// Created (for NEAT Controller) two networks per organism, each spawned from the same genome.
// Added "cross-over" between the networks (one's outputs are the other's inputs).
//
// Revision 1.28  2004/07/19 18:21:56  dexabraxis
// Tweaks to physics parameters.  Added code for saving images.
//
// Revision 1.27  2004/07/19 00:30:12  dexabraxis
// Many bug fixes and better harness and Echo state net support.
//
// Revision 1.26  2004/07/11 01:34:06  dexabraxis
// Using 4-run fitness evals
//
// Revision 1.25  2004/07/10 19:20:49  dexabraxis
// Added angular damper to harness.
//
// Revision 1.24  2004/07/10 07:18:33  dexabraxis
// Added "harness" and stop on non-foot collision.
//
// Revision 1.23  2004/06/28 01:51:17  dexabraxis
// Adding initial preliminary version of echo state nets.
//
// Revision 1.22  2004/06/28 00:08:05  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.21  2004/06/10 15:10:00  dexabraxis
// Added "ClumpNetwork" basics (no evolution yet)
//
// Revision 1.20  2004/06/07 14:05:26  dexabraxis
// *** empty log message ***
//
// Revision 1.19  2004/05/14 07:55:10  dexabraxis
// Fixed bug with mutiple runs crashing.  Added random seed to UI
//
// Revision 1.18  2004/05/14 01:01:07  dexabraxis
// *** empty log message ***
//
// Revision 1.17  2004/05/12 09:43:47  dexabraxis
// Tweaks to the fitness function, Started on fitness graph.
//
// Revision 1.16  2004/05/12 03:07:43  dexabraxis
// *** empty log message ***
//
// Revision 1.15  2004/05/12 00:53:29  dexabraxis
// Greatly improved stability.  Added clown feet.
//
// Revision 1.14  2004/05/11 21:17:12  dexabraxis
// *** empty log message ***
//
// Revision 1.13  2004/05/09 23:29:47  dexabraxis
// Greatly improved stability.  Added clown feet.
//
// Revision 1.12  2004/05/09 18:01:04  dexabraxis
// Modularized the ODE code, but still too unstable.
//
// Revision 1.11  2004/05/09 08:56:14  dexabraxis
// In middle of switching to new body code.
//
// Revision 1.10  2004/05/08 23:40:23  dexabraxis
// Tweaks to motor strengthts
//
// Revision 1.9  2004/05/08 17:01:42  dexabraxis
// Tweaks to motor strengthts
//
// Revision 1.8  2004/05/08 08:33:22  dexabraxis
// Reworking body construction-- explicit.
//
//////////////////////////////////////////////////////////////////////////

#endif
