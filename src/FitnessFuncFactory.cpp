/*
 *  FitnessFuncFactory.cpp
 *  else
 *
 *  Created by vector on 10/17/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */
#include <FitnessFuncFactory.hpp>
#include <FitnessFunc.hpp>
#include <ElseException.hpp>
#include <BalanceFitnessFunc.hpp>
#include <ConstantFitnessFunc.hpp>
#include <WalkFitnessFunc.hpp>
#include <VelocityFitnessFunc.hpp>
#include <StepFitnessFunc.hpp>
#include <StrategyFitnessFunc.hpp>
#include <AcrobotFitnessFunc.hpp>

// External Includes
#include <sstream>

Else::FitnessFuncPtr 
Else::FitnessFuncFactory
::make( PopulationFitnessPtr argParent, const std::string& argFitnessType )
{
    if( "Balance" == argFitnessType )
    {
        return FitnessFuncPtr( new BalanceFitnessFunc( argParent ) );
    }
    else if ( "Constant" == argFitnessType )
    {
        return FitnessFuncPtr( new ConstantFitnessFunc( argParent ) );
    }
    else if ( "Walk" == argFitnessType )
    {
        return FitnessFuncPtr( new WalkFitnessFunc( argParent ) );
    }
    else if ( "Velocity" == argFitnessType )
    {
        return FitnessFuncPtr( new VelocityFitnessFunc( argParent ) );
    }
    else if ( "Step" == argFitnessType )
    {
        return FitnessFuncPtr( new StepFitnessFunc( argParent ) );
    }
    else if ( "WalkStrategy" == argFitnessType )
    {
        return FitnessFuncPtr( new StrategyFitnessFunc( argParent, "Walk" ) );
    }
    else if ( "StepStrategy" == argFitnessType )
    {
        return FitnessFuncPtr( new StrategyFitnessFunc( argParent, "Step" ) );
    }
    else if ( "Acrobot" == argFitnessType )
    {
        return FitnessFuncPtr( new AcrobotFitnessFunc( argParent ) );
    }
    
    // Fall-through
    std::stringstream out;
    out << "Unknown fitness type \"" << argFitnessType << "\".\n";
    throw ElseException( out.str() );
}
