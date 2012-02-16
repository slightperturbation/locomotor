/*
 *  PopulationFitness.cpp
 *  else
 *
 *  Created by vector on 10/15/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#include "PopulationFitness.hpp"

using namespace Else;


Else::PopulationFitness
::PopulationFitness( const std::string& argFitnessType )
: myFitnessType( argFitnessType )
{
    // NOOP
}

void
Else::PopulationFitness
::setDoubleParam( const std::string& argName, double argParam )
{
    myDoubleParams[ argName ] = argParam;
}

Else::FitnessFuncPtr
Else::PopulationFitness
::createInstance( void )
{
    FitnessFuncPtr ff = myFactory.make( shared_from_this(), myFitnessType );
    for( std::map< std::string, double >::iterator i = myDoubleParams.begin();
        i != myDoubleParams.end();
        ++ i )
    {
        ff->setDoubleParam( i->first, i->second );
    }
    return ff;
}

void
Else::PopulationFitness
::resetToPopSize( size_t argPopulationSize )
{
    myInstances.clear();
    for( size_t i = 0; i < argPopulationSize; ++i )
    {
        myInstances.push_back( createInstance() );
    }
}

size_t 
Else::PopulationFitness
::popSize( void )
{
    return myInstances.size();
}

FitnessFuncPtr
Else::PopulationFitness
::operator[] ( size_t argIndex )
{
    return myInstances[ argIndex ];
}

void 
Else::PopulationFitness
::endGeneration( void )
{
    // Clear all instances, but first set all their parents to null!
    // choose which FitnessFuncs will be recorded in history
    // for now, record all
    for( std::vector< FitnessFuncPtr >::iterator i = myInstances.begin();
        i != myInstances.end(); ++i )
    {
        myHistoricalFitnessFuncs.push_back( *i );
    }
    myInstances.clear();
}
