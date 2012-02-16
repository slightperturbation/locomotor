/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_BALANCEFITNESSFUNC_HPP
#define ELSE_BALANCEFITNESSFUNC_HPP
//
// Copyright (C) 2009, Brian F. Allen
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

	class BalanceFitnessFunc : public HistoricalFitnessFunc
	{
	public:
    BalanceFitnessFunc( PopulationFitnessPtr argParent )
      : HistoricalFitnessFunc( argParent ),
        myFitness( 0 ),
        myRequiredHeight( 0.7 )
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

    virtual void reset( void )
    {
      HistoricalFitnessFunc::reset();
      myFitness = 0;
    }
        
    virtual void evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
    {
      const dReal* pos = dBodyGetPosition( argBody->myBodies[Waist] );
      // longer they are up, the better
      // get Z-component of Torso -- standing
      myFitness += pos[2];

      myHistory.push_back( myFitness ); 
      //      std::cout << argBody->myTrunkBodyID
      //        << " has fitness " << fitness << "\n";
    }

    virtual bool shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
    { 
      const dReal* pos = dBodyGetPosition( argBody->myBodies[Waist] );
      return (pos[2] < myRequiredHeight ); 
    }

  private:
    double myFitness;
    double myRequiredHeight;

	};
}

#endif
