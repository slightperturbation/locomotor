/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_STEPFITNESSFUNC_HPP
#define ELSE_STEPFITNESSFUNC_HPP
//
// Copyright (C) 2009, Brian F. Allen
//
// Contact: vector@acm.org
//


// Project Includes
#include <Else.hpp>
#include <FitnessFunc.hpp>

// External Includes
//#define DEFAULT_STEP_TARGET_POS_L					-0.11024, 0.0191109, 0.0220548
//#define DEFAULT_STEP_TARGET_POS_R					0.0997417, 0.0201547, 0.028093


#define DEFAULT_STEP_TARGET_POS_L 0.15, 0.0, 0.0
#define DEFAULT_STEP_TARGET_POS_R -0.15, 0.35, 0.0

namespace Else 
{
    class StepFitnessFunc : public HistoricalFitnessFunc
    {
    public:
        StepFitnessFunc( PopulationFitnessPtr argParent )
        : HistoricalFitnessFunc( argParent ),
        myRequiredHeight( DEFAULT_STEP_REQUIRED_HEIGHT ),
        myTimeStepsSinceLastImprovement( 0 ),
		myHarnessFactor( 5e-7 ),
        myHarnessHeight( DEFAULT_HARNESS_HEIGHT ),
        myAllowedTime( 6.0 ),
        myStrideLength( 1.0 ),
        myIsUnstable( false )
        { }
        
        virtual void setDoubleParam( const std::string& argName, double argValue )
        {
            if( argName == "Height" )
            {
                myRequiredHeight = argValue;
                return;
            }
            if( argName == "HarnessHeight" )
            {
                myHarnessHeight = argValue;
                return;
            }
            if( argName == "HarnessFactor" )
            {
                myHarnessFactor = argValue;
            }
            if( argName == "AllowedTime" )
            {
                myAllowedTime = argValue;
            }
            if( argName == "Stride" )
            {
                myStrideLength = argValue;
            }
            // Don't know it, so pass to parent class
            FitnessFunc::setDoubleParam( argName, argValue );
        }
        
        virtual void evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
        {
            // Right: 0.0997417, 0.0201547, 0.028093
            // Left : -0.11024, 0.0191109, 0.0220548
            
            argBody->setHarnessHeight( myHarnessHeight );
            
            //const dReal* waist = dBodyGetPosition( argBody->myBodies[Waist] );
            gmtl::Point3d rCol = argBody->getLastCollision( RightFoot );
            const dReal* rFoot = dBodyGetPosition( argBody->myBodies[RightFoot] );
            gmtl::Point3d posR( rFoot[0], rFoot[1], rFoot[2] );
            gmtl::Point3d lCol = argBody->getLastCollision( LeftFoot );
            const dReal* lFoot = dBodyGetPosition( argBody->myBodies[LeftFoot] );
            gmtl::Point3d posL( lFoot[0], lFoot[1], lFoot[2] );
            
            gmtl::Vec3d upVec = argBody->getUpVector();
            gmtl::normalize(upVec);
            const gmtl::Vec3d up( 0,0,1 );
            const double cosUp = dot( upVec, up );

            if( myHistory.empty() ) 
            {
                // First step, so establish targets.
                // we care that (1) steps alternate, and 
                // (2) the new front will be a given distance beyond the old front foot
                const double stepLength = 0.5 * myStrideLength;  // half-stride distance
                
                // starting feet positions, projected to the ground
                myTargetR = posR;  myTargetR[2] = 0.0;
                myTargetL = posL;  myTargetL[2] = 0.0;
                if( myTargetR[1] > myTargetL[1] )
                {
                    // right is in front, so step with left
                    myTargetL[1] = myTargetR[1] + stepLength;
                }
                else
                {
                    // left in front, step with right
                    myTargetR[1] = myTargetL[1] + stepLength;
                }
            }
            
            const double rDist = ( gmtl::LineSegd( myTargetR, posR ) ).getLength();
            const double lDist = ( gmtl::LineSegd( myTargetL, posL ) ).getLength();
            
            const double r = std::max<double>( 0.0, 1.0 - rDist ); //std::min<double>( 20.0, 0.1 / rDist );
            const double l = std::max<double>( 0.0, 1.0 - lDist ); //std::min<double>( 20.0, 0.1 / lDist );
            
            const double t = argSim->getElapsedTime();
            
            double fitness = myHistory.empty() ? 100.0 : myHistory.back();
            //fitness = (r + l) + 0.9 * fitness;
            fitness += t * std::min<double>( r, l ) * cosUp * cosUp ;
            
            fitness -= argBody->getAppliedHarness() * myHarnessFactor / argSim->getElapsedTime();
            
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
            myHistory.push_back( fitness ); 
            //      std::cout << argBody->myTrunkBodyID
            //        << " has fitness " << fitness << "\n";
        }
        
        virtual bool shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody  )
        { 
            HumanoidArticulatedBodyPtr humanBody
                    = boost::dynamic_pointer_cast< HumanoidArticulatedBody >( argBody );

            if( humanBody->testStability() )
            {
                // Punish instability
                myHistory.push_back( 1.0 ); 
                //std::cerr << "STAB ";
                return true;
            }
            gmtl::Vec3d upVec = humanBody->getUpVector();
            gmtl::normalize(upVec);
            const gmtl::Vec3d up( 0,0,1 );
            const double cosUp = dot( upVec, up );
            if( cosUp < 0.95 )
            {
                return true;
            }
            
            // TODO Why does this fail?
            //if( argBody->hasNonFootCollision() )
            //{
            //std::cerr << "COL ";
            // non-foot collided with floor
            //    return true;
            //}
            if( myTimeStepsSinceLastImprovement > 300 || argSim->getElapsedTime() > 15.0 )
            {
                //std::cerr << "STAG ";
                return true;
            }
            
            if( myAllowedTime < argSim->getElapsedTime() ) 
            {
                return true;
            }
            
            double height = humanBody->getWaistHeight();
            bool isBelowRequiredHeight = ( height < ( myRequiredHeight * humanBody->getInitialWaistHeight() ) );
            return isBelowRequiredHeight;
            
        }
        
    private:
        double myRequiredHeight;
        double myHarnessHeight;
        double myHarnessFactor;
        double myAllowedTime;
        double myStrideLength;
        gmtl::Point3d myTargetL;
        gmtl::Point3d myTargetR;
        bool myIsUnstable;
        int myTimeStepsSinceLastImprovement;
    };
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: WalkFitnessFunc.hpp,v $
//
//////////////////////////////////////////////////////////////////////////

