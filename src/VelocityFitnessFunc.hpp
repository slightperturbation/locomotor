/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_VELOCITYFITNESSFUNC_HPP
#define ELSE_VELOCITYFITNESSFUNC_HPP
//
//
// Copyright (C) 2009, Brian F. Allen
//
// Contact: vector@acm.org
//


// Project Includes
#include <Else.hpp>
#include <FitnessFunc.hpp>

// External Includes

namespace Else 
{
	class VelocityFitnessFunc : public HistoricalFitnessFunc
	{
	public:
		VelocityFitnessFunc( PopulationFitnessPtr argParent )
        : HistoricalFitnessFunc( argParent ), 
          myRequiredHeight( 0.25 )
		{
		}

		virtual void setDoubleParam( const std::string& argName, double argValue )
		{
			if( argName == "Height" )
			{
				myRequiredHeight = argValue;
				return;
			}

			// Don't know it, so pass to parent class
			FitnessFunc::setDoubleParam( argName, argValue );
		}
        
        virtual void evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
		{
			double fitness = 0.0;
            myIsUnstable = argBody->testStability();
			if( myIsUnstable)
			{
				// Stability Bad
				myHistory.push_back( 0 );
				return;
			}

			dVector3 vel;
            dBodyGetRelPointVel( argBody->myBodies[Trunk],
													 0, 0, 0, 
													 vel );
			fitness += 10. * vel[1] - abs( vel[0] ) - abs( vel[2] );
			
			if( ( !myHistory.empty() ) 
				&& ( abs( myHistory.back() - fitness ) < 0.0001 ) )
			{
				++myTimeStepsSinceLastImprovement;
			}
			else
			{
				myTimeStepsSinceLastImprovement = 0;
			}

			myHistory.push_back( fitness ); 
		}
        
        

        virtual bool shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody  )
		{ 
            if( argBody->testStability() )
			{
				// Punish instablitity
				myHistory.push_back( 0 );
				return true;
			}
			if( myTimeStepsSinceLastImprovement > 50 )
			{
				return true;
			}
            const dReal* pos = dBodyGetPosition( argBody->myBodies[Trunk] );
			return (pos[2] < myRequiredHeight); 
		}

	private:
		double myRequiredHeight;
		double myTimeValue;
		bool myIsUnstable;
		int myTimeStepsSinceLastImprovement;
	};
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: VelocityFitnessFunc.hpp,v $
// Revision 1.1  2004/06/28 01:51:17  dexabraxis
// Adding initial preliminary version of echo state nets.
//
//
//////////////////////////////////////////////////////////////////////////

