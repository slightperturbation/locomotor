#ifndef ELSE_STRATEGYFITNESSFUNC_HPP
#define ELSE_STRATEGYFITNESSFUNC_HPP

/*
 *  StrategyFitnessFunc.hpp
 *  else
 *
 *  Created by vector on 10/15/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#include <Else.hpp>
#include <FitnessFunc.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace Else
{
    class StrategyFitnessFunc : public FitnessFunc, public boost::enable_shared_from_this< StrategyFitnessFunc >
    {
    public:
        /// Argument specifies the underlying fitness function.
        StrategyFitnessFunc( PopulationFitnessPtr argParent, const std::string& argFitnessType = "Walk" );
        virtual ~StrategyFitnessFunc();
        
        virtual void setFitnessFunc( const std::string& argType );
        virtual void setDoubleParam( const std::string& argName, double argValue );
        virtual void evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody );
        virtual double finalFitness( void );
        virtual double finalRawFitness( void );
        virtual bool shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody  );
        
        virtual void reset( void ) { myContainedFitnessFunc->reset(); }
        std::vector< double >& getHistory() { return myContainedFitnessFunc->getHistory(); }
        double last( void ) { return myContainedFitnessFunc->last(); }
        
    private:
        double strategyDistance( std::vector< double >& from, std::vector< double >& to );
        StrategyFitnessFunc& operator=( StrategyFitnessFunc& ); //< disable
        std::string myFitnessType;
        double myNoveltyWeight;
        FitnessFuncPtr myContainedFitnessFunc; //< Owned.
        double(*myDistanceFunc)(std::vector<double>&, std::vector<double>&);
    };
}


#endif

