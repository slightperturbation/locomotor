/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef ELSE_HUMANOID_ARTICULATED_BODY_HPP
#define ELSE_HUMANOID_ARTICULATED_BODY_HPP

// Copyright (C) 2010, Brian F. Allen
//
// Contact: vector@acm.org
//

/** \file 
 \author Brian F. Allen (vector@acm.org)
 */

// Project Includes
#include <ArticulatedBody.hpp>
#include <ode/ode.h>

namespace Else
{
    
    struct HumanoidArticulatedBodyDesc
	{
		double height;
		double weight;
		double hipBreadth;
	};
    
	void loadDesc( HumanoidArticulatedBodyDesc& outBodyDesc, const char* argBodyDescFilename = BODY_FILE_NAME );
    
    
    /// HumanoidArticulatedBody provides a concrete ArticulatedBody that 
    /// is roughly of human form.
    class HumanoidArticulatedBody : public ArticulatedBody
    {
    public:
        //HumanoidArticulatedBody( dWorldID argWorldID, dSpaceID argSpaceID );
        
        //HumanoidArticulatedBody( dWorldID argWorldID, dSpaceID argSpaceID, 
        //                         const char* argBodyDescFilename );
        
        HumanoidArticulatedBody( dWorldID argWorldID, dSpaceID argSpaceID,
                                 double argHeight, double argWeight, double argHipBreadth );
        
        //HumanoidArticulatedBody( dWorldID argWorldID, dSpaceID argSpaceID, 
        //                         const ArticulatedBody& other );
        
        virtual ~HumanoidArticulatedBody();

        virtual void createBodySegments( double argHipWidth );
        virtual void createBody( void );
        void setAnkleStrength( double arg );
        void setFootLengthPerHeight( double arg );
        void setFootWidthPerHeight( double arg );

        virtual void getSensors( ControllerPtr argController, double argDeltaTime );
        virtual void setActuators( const ControllerPtr argController );
        
        virtual void writeCurrJointTraces( double argT );
        virtual void acquireJointAngleData( std::vector< double >& out );
        
        virtual double getInitialWaistHeight( void );
        virtual double getWaistHeight( void );
        virtual double getWeight( void );
        
        friend class FitnessFunc;
        friend class BalanceFitnessFunc;
        friend class WalkFitnessFunc;
        friend class VelocityFitnessFunc;
        friend class StepFitnessFunc;
        
        
        
        
        
//        /// TEMP
//        static int getOutputCountPerNetwork( void )
//        {
//            int count = 2;
//            if( CONTROL_KNEE_ACTUATOR )
//            {
//                count++;
//            }
//            if( CONTROL_SPINE_ACTUATOR )
//            {
//                count++;
//            }
//            if( USING_CROSS_OVER_COMMUNICATION )
//            {
//                count++;
//            }
//            return count;
//        }
//        static int getControllerOutputCount( void )
//        {
//            int count = 4;
//            if( CONTROL_KNEE_ACTUATOR )
//            {
//                count += 2;
//            }
//            if( CONTROL_SPINE_ACTUATOR )
//            {
//                count++;
//            }
//            if( USING_CROSS_OVER_COMMUNICATION )
//            {
//                count += 2;
//            }
//            return count;
//        }
        
        // Actuator count is the number of actual actuators
        // that the ArticulatedBody is controlling.  It is *not* the number
        // of outputs of the controlling network.
//        static int getActuatorCount( void )
//        {
//            // Total, for each side.  4 = (R,L) x (Hipx, HipZ)
//            int count = 6;//4;
//            if( CONTROL_KNEE_ACTUATOR )
//            {
//                // One for each side.  KneeR, KneeL
//                count += 2;
//            }
//            if( CONTROL_SPINE_ACTUATOR )
//            {
//                // One for each side.  SpineR, SpineL
//                count += 2;
//            }
//            return count;
//        }
        
    protected:
        void addAnkle( gmtl::Vec3d argAnchor, dBodyID argShank, dBodyID argFoot, BodyJointEnum argJointName );
        void createKnee( gmtl::Vec3d argAnchor, dBodyID argThigh, dBodyID argShank, BodyJointEnum argJointName );
        void addHip( gmtl::Vec3d argAnchor, dBodyID argThigh, BodyJointEnum argJointName );
        
    private:
        double myRightHipVelScale[3]; 
        double myLeftHipVelScale[3];
        
        double myHeight;
        double myWeight;
        double myAnkleStrength;
        double myFootLengthPerHeight;
        double myFootWidthPerHeight;

        HumanoidArticulatedBodyDesc myDesc; 
    };
}

#endif
