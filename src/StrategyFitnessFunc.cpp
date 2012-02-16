/*
 *  StrategyWalkFitnessFunc.cpp
 *  else
 *
 *  Created by vector on 10/15/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */
#include <StrategyFitnessFunc.hpp>

#include <PopulationFitness.hpp>
#include <FitnessFuncFactory.hpp>
#include <FitnessFunc.hpp>
#include <Math.hpp>

#include <vector>
#include <cmath>

bool isNear( double argA, double argB )
{
    return 0.01 > std::abs( argA - argB );
}

double pointDistance( std::vector< double >& from, std::vector< double >& to )
{
    if( from.empty() || to.empty() )
    {
        return 0.0;
    }
    double d = from.back() - to.back();
    return d*d;
}

double eulerDistance( std::vector< double >& from, std::vector< double >& to )
{
    const size_t length = std::min( from.size(), to.size() );
    double totDist = 0.0;
    size_t i = 0;
    for( ; i < length; ++i )
    {
        double d = from[i] - to[i];
        totDist += d*d;
    }
    
    // and add all the excess from the longer (assuming 0 for extra timesteps)
    std::vector< double >& longer = ( from.size() > to.size() ) ? from : to ;
    for( ; i < longer.size(); ++i )
    {
        totDist += longer[i]*longer[i];
    }
    return totDist;
}

///
double 
Else::StrategyFitnessFunc
::strategyDistance( std::vector< double >& from, std::vector< double >& to )
{
    return (*myDistanceFunc)( from, to );
}



Else::StrategyFitnessFunc
::StrategyFitnessFunc( PopulationFitnessPtr argParent, const std::string& argFitnessType )
: FitnessFunc( argParent ),
  myFitnessType( argFitnessType ),
  myNoveltyWeight( 0.1 ),
  myContainedFitnessFunc( (FitnessFunc*)0 ),
  myDistanceFunc( &eulerDistance )
{
    setFitnessFunc( argFitnessType );
}

Else::StrategyFitnessFunc
::~StrategyFitnessFunc()
{
   // noop 
}

void 
Else::StrategyFitnessFunc
::setFitnessFunc( const std::string& argFitnessType )
{
    myContainedFitnessFunc = myParent->getFactory().make( myParent, argFitnessType );
}

void 
Else::StrategyFitnessFunc
::setDoubleParam( const std::string& argName, double argValue )
{
    if( argName == "NoveltyWeight" )
    {
        myNoveltyWeight = argValue;
        return;
    }
    if( argName == "DistanceMetric" && isNear( argValue, 0.0 ) )
    {
        myDistanceFunc = &pointDistance;
        return;
    }
    if( argName == "DistanceMetric" && isNear( argValue, 1.0 ) )
    {
        myDistanceFunc = &eulerDistance;
        return;
    }
    myContainedFitnessFunc->setDoubleParam( argName, argValue );
}

void
Else::StrategyFitnessFunc
::evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
{
    myContainedFitnessFunc->evaluate( argSim, argBody );
}

double
Else::StrategyFitnessFunc
::finalFitness( void )
{
    std::vector< FitnessFuncPtr >& currGen = myParent->getCurrGenerationFitnessFuncs();
    // accumulate our distance factor to each
    double totDist = 0.0;
    for( size_t i = 0; i < currGen.size(); ++i )
    {
        double d = strategyDistance( getHistory(), currGen[i]->getHistory() );
        totDist += d;
    }

    // Choose a random selection of 
    std::vector< FitnessFuncPtr >& archive = myParent->getHistoricalFitnessFuncs();
    const size_t archiveTargetSize = std::min<size_t>( archive.size(), currGen.size() * 2 );
    if( archiveTargetSize == archive.size() )
    {
        // get them all, not random
        for( size_t i = 0; i < archiveTargetSize; ++i )
        {
            double d = strategyDistance( getHistory(), archive[i]->getHistory() );
            totDist += d;
        }
    }
    else
    {
        std::vector< bool > archiveIsUsed( archive.size(), false );
        size_t archiveUsedCount = 0;
        laetoli::Entropy ent;
        while( archiveUsedCount < archiveTargetSize )
        {
            size_t i = ent.uniform( 0, archive.size()-1 );
            if( archiveIsUsed[i] )
            {
                continue;
            }
            double d = strategyDistance( getHistory(), archive[i]->getHistory() );
            totDist += d;
            archiveIsUsed[i] = true;
            archiveUsedCount++;
        }
    }
    
    // totDist is the sum-of-squared distances between this strategy and all others
    const double w_s = 1.0;// / ( currGen.size() + archiveTargetSize );
    
    const double f = std::max( 0.0, (1.0 - myNoveltyWeight) * myContainedFitnessFunc->finalFitness() + myNoveltyWeight * w_s * totDist );
    
    if( ((double)rand()/(double)RAND_MAX) > 0.98 )
    {
        std::cout << "FinalFitness: (" << (1.0 - myNoveltyWeight) << " * " << myContainedFitnessFunc->finalFitness()
        << ") + (" << w_s << " * " << myNoveltyWeight << " * " << totDist << ") = (" 
        << (1.0 - myNoveltyWeight) * myContainedFitnessFunc->finalFitness() << ") + (" 
        << myNoveltyWeight * w_s * totDist << ") = "
        << f << std::endl;
    }
    return f;
}

double
Else::StrategyFitnessFunc
::finalRawFitness( void )
{
    return myContainedFitnessFunc->finalFitness();
}

bool
Else::StrategyFitnessFunc
::shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody  )
{
    return myContainedFitnessFunc->shouldStopRun( argSim, argBody );
}


