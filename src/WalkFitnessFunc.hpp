/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_WALKFITNESSFUNC_HPP
#define ELSE_WALKFITNESSFUNC_HPP
//
// $Id: WalkFitnessFunc.hpp,v 1.17 2004/07/21 20:58:43 dexabraxis Exp $
// $Date: 2004/07/21 20:58:43 $
//
// Copyright 2009, Brian F. Allen
//
// Contact: vector@acm.org
//

// Project Includes
#include <Else.hpp>
#include <FitnessFunc.hpp>
#include <PhysicsSimulator.hpp>

// External Includes

namespace Else 
{
    class WalkFitnessFunc : public HistoricalFitnessFunc
    {
    public:
        WalkFitnessFunc( PopulationFitnessPtr argParent )
        :
        HistoricalFitnessFunc( argParent ),
        myBaseFitnessValue( 50.0 ),
        myRequiredHeight( DEFAULT_WALK_REQUIRED_HEIGHT ),
        myTimeValue( DEFAULT_WALK_TIME_VALUE ),
        myTimeStepsSinceLastImprovement( 0 ),
        myHarnessHeight( DEFAULT_HARNESS_HEIGHT ),
        myDistanceFactor( 1.0 ),
        myCoMDistanceFactor( 0.0 ),
        myHarnessFactor( 1e-6 ),
        myStrideFactor( 0.0 ),
        myStandingFootDistanceFactor( 0.0 ),
        myRequiredImprovementPeriod( 1000.0 ),
        myIsUnstable( false ),
        myFartherDistanceFactor( 0.0 )
        {
        }
        
        virtual void setDoubleParam( const std::string& argName, double argValue )
        {
            if( argName == "Height" )
            {
                myRequiredHeight = argValue;
                return;
            }
            if( argName == "TimeValue" )
            {
                myTimeValue = argValue;
				return;
            }
            if( argName == "HarnessHeight" )
            {
                myHarnessHeight = argValue;
                return;
            }
            if( argName == "DistanceFactor" )
            {
                myDistanceFactor = argValue;
                return;
            }
            if( argName == "FartherDistanceFactor" )
            {
                myFartherDistanceFactor = argValue;
                return;
            }
            if( argName == "CoMDistanceFactor" )
            {
                myCoMDistanceFactor = argValue;
                return;
            }
            if( argName == "HarnessFactor" )
            {
                myHarnessFactor = argValue;
                return;
            }
            if( argName == "StrideFactor" )
            {
                myStrideFactor = argValue;
                return;
            }
            if( argName == "StandingFootDistanceFactor" )
            {
                myStandingFootDistanceFactor = argValue;
                return;
            }
            if( argName == "RequiredImprovementPeriod" )
            {
                myRequiredImprovementPeriod = argValue;
                return;
            }
            // Don't know it, so pass to parent class
            FitnessFunc::setDoubleParam( argName, argValue );
        }
        
        virtual void evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody  )
        {
            using namespace gmtl;
            argBody->setHarnessHeight( myHarnessHeight );
            double fitness = 2.0 * myBaseFitnessValue;
 
            gmtl::Point3d posR = argBody->getLastCollision( RightFoot );
            const dReal* rFoot = dBodyGetPosition( argBody->myBodies[RightFoot] );
            gmtl::Point3d posL = argBody->getLastCollision( LeftFoot );
            const dReal* lFoot = dBodyGetPosition( argBody->myBodies[LeftFoot] );
            const dReal* com = dBodyGetPosition( argBody->myBodies[Waist] );
            
            //std::cerr << "Right: " << rFoot[0] << ", " << rFoot[1] << ", " << rFoot[2] << "\n";
            //std::cerr << "Left : " << lFoot[0] << ", " << lFoot[1] << ", " << lFoot[2] << "\n";
            
            // double r = ( gmtl::LineSegd( gmtl::Point3d(0,0,0), posR ) ).getLength();
            // double l = ( gmtl::LineSegd( gmtl::Point3d(0,0,0), posL ) ).getLength();
            
            double feetDistance = LineSegd( Point3d( rFoot[0], rFoot[1], rFoot[2] ), Point3d( lFoot[0], lFoot[1], lFoot[2] ) ).getLength();
            
            double r = posR[1] ;
            double l = posL[1] ;
            double c = com[1] ;
            
            double further = std::max<double>( r, l );
            double closer = std::min<double>( r, l );
            double stepLength = further - closer;
            
            double strideFactor = 1.0;
            // TODO -- use the froude number based on the character's height
            if( stepLength > 0.3 && stepLength < 1.0 )
            {
                // bonus for natural stride
                strideFactor += myStrideFactor;
            }
            
            // moving forward
            fitness += myDistanceFactor * strideFactor * std::min<double>( closer, c ) ; // min of feet or com
            fitness += myFartherDistanceFactor * further ;
            
            // CoM distance factor
            fitness += myCoMDistanceFactor * com[1];
            
            fitness += myTimeValue * argSim->getElapsedTime();            // longer they are up, the better
            fitness += myStandingFootDistanceFactor * (1.0 / ( 0.85 + feetDistance ) );  // closest possible foot position is ~15cm
            fitness -= myHarnessFactor * argBody->getAppliedHarness() / argSim->getElapsedTime();
            
            //std::cout << "harness factor: " <<  argBody->getAppliedHarness() * myHarnessFactor << "\n";
            if( ( !myHistory.empty() ) 
               && ( abs( myHistory.back() - fitness ) < 0.02 ) )
            {
                ++myTimeStepsSinceLastImprovement;
            }
            else
            {
                myTimeStepsSinceLastImprovement = 0;
            }
            
            fitness = (fitness > myBaseFitnessValue) ? fitness : myBaseFitnessValue;
            
            myHistory.push_back( fitness ); 
            //      std::cout << argBody->myTrunkBodyID
            //        << " has fitness " << fitness << "\n";
        }
        
        virtual bool shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
        { 
            HumanoidArticulatedBodyPtr humanBody
                    = boost::dynamic_pointer_cast< HumanoidArticulatedBody >( argBody );

            if( humanBody->testStability() )
            {
                // Punish instability
                myHistory.push_back( myBaseFitnessValue ); 
                std::cerr << "STAB ";
                return true;
            }
            //if( argBody->hasNonFootCollision() )
            //{
            //    //std::cerr << "COL ";
            //    // non-foot collided with floor
            //  return true;
            //}
            if( myTimeStepsSinceLastImprovement > myRequiredImprovementPeriod )
            {
                std::cerr << "STAG ";
                return true;
            }
            //const dReal* pos = dBodyGetPosition( argBody->myBodies[Trunk] );
            double height = humanBody->getWaistHeight();
            bool isBelowRequiredHeight = ( height < ( myRequiredHeight * humanBody->getInitialWaistHeight() ) );
            //if( isBelowRequiredHeight )
            //{
            //std::stringstream msg;
            //msg << "BelowMinHeight.  height=" << height << ", minHeight = " << ( myRequiredHeight * argBody->getInitialWaistHeight() ) << "\n";
            //std::cerr << msg.str();
            //}
            return isBelowRequiredHeight;
            //return (pos[2] < myRequiredHeight); 
        }
        
    private:
        double myBaseFitnessValue;
        double myRequiredHeight;
        double myTimeValue;
        double myHarnessHeight;
        double myHarnessFactor;
        double myDistanceFactor;
        double myCoMDistanceFactor;
        double myStrideFactor;
        double myFartherDistanceFactor;
        double myStandingFootDistanceFactor;
        double myRequiredImprovementPeriod;
        bool myIsUnstable;
        int myTimeStepsSinceLastImprovement;
    };
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: WalkFitnessFunc.hpp,v $
// Revision 1.17  2004/07/21 20:58:43  dexabraxis
// Added a "polarity" input to tell which side was being evalutated.  Runs seem to achieve good strides by 250k evaluations.
//
// Revision 1.16  2004/07/19 18:21:56  dexabraxis
// Tweaks to physics parameters.  Added code for saving images.
//
// Revision 1.15  2004/07/19 00:30:12  dexabraxis
// Many bug fixes and better harness and Echo state net support.
//
// Revision 1.14  2004/07/11 01:34:06  dexabraxis
// Using 4-run fitness evals
//
// Revision 1.13  2004/07/10 19:20:50  dexabraxis
// Added angular damper to harness.
//
// Revision 1.12  2004/07/10 07:18:33  dexabraxis
// Added "harness" and stop on non-foot collision.
//
// Revision 1.11  2004/06/28 00:08:06  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.10  2004/06/10 15:10:00  dexabraxis
// Added "ClumpNetwork" basics (no evolution yet)
//
// Revision 1.9  2004/06/07 14:05:26  dexabraxis
// *** empty log message ***
//
// Revision 1.8  2004/05/30 20:19:58  dexabraxis
// Refactored NeatController.  Alllowed multiple runs.  Possibliy introduced bug of output # changing.
//
// Revision 1.7  2004/05/14 01:01:07  dexabraxis
// *** empty log message ***
//
// Revision 1.6  2004/05/12 03:07:43  dexabraxis
// *** empty log message ***
//
// Revision 1.5  2004/05/12 00:53:29  dexabraxis
// Greatly improved stability.  Added clown feet.
//
// Revision 1.4  2004/05/11 21:17:13  dexabraxis
// *** empty log message ***
//
// Revision 1.3  2004/05/09 08:56:14  dexabraxis
// In middle of switching to new body code.
//
// Revision 1.2  2004/05/08 08:33:23  dexabraxis
// Reworking body construction-- explicit.
//
// Revision 1.1  2004/05/05 03:48:41  dexabraxis
// Still have initial hip rotation problem.  Using PD-controllers.
//
//
//////////////////////////////////////////////////////////////////////////

