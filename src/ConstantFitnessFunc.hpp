/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_CONSTANTFITNESSFUNC_HPP
#define ELSE_CONSTANTFITNESSFUNC_HPP
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
  class ConstantFitnessFunc : public HistoricalFitnessFunc
  {
  public:
    ConstantFitnessFunc( PopulationFitnessPtr argParent )
      : HistoricalFitnessFunc( argParent ),
        myConstant( 0.1 )
    {
    }

    virtual void setDoubleParam( const std::string& argName, double argValue )
    {
      if( argName == "Value" )
      {
        myConstant = argValue;
        return;
      }
      // Don't know it, so pass to parent class
      FitnessFunc::setDoubleParam( argName, argValue );
    }

    virtual void evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
    {
      myHistory.push_back( myConstant );
    }

    virtual bool shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
    { 
      return false;
    }
  private:
    double myConstant;

  };
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: ConstantFitnessFunc.hpp,v $
// Revision 1.2  2004/05/08 08:33:22  dexabraxis
// Reworking body construction-- explicit.
//
// Revision 1.1  2004/05/05 03:48:41  dexabraxis
// Still have initial hip rotation problem.  Using PD-controllers.
//
//
//////////////////////////////////////////////////////////////////////////

