/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_FITNESSFUNC_HPP
#define ELSE_FITNESSFUNC_HPP
//
// $Id: FitnessFunc.hpp,v 1.6 2004/06/28 00:08:05 dexabraxis Exp $
// $Date: 2004/06/28 00:08:05 $
//
// Copyright (C) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//

// Project Includes
#include <Else.hpp>

// External Includes
#include <string>

namespace Else 
{
    /// 
    /// Use.  One instance of the function per 
	class FitnessFunc
	{
	public:
        FitnessFunc( PopulationFitnessPtr argParent ) 
        : myParent( argParent )
        {}
        
        virtual ~FitnessFunc() {}
        
        virtual void reset( void ) = 0;
        
        PopulationFitnessPtr getParent( void ) { return myParent; }
        
        void setParent( PopulationFitnessPtr argParent ) { myParent = argParent; }
        
        virtual void evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody ) = 0;
        
        /// Returns the final fitness to be passed to neat.  This
        /// may be adjusted by population statistics and may not equal last()
        virtual double finalFitness( void ) = 0;
        virtual double finalRawFitness( void ) { return finalFitness(); }
        
        virtual bool shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody ) = 0;
        
        virtual void setDoubleParam( const std::string& argName, double argValue ) 
        {
            std::cerr << "Value \"" << argName << "\" ignored.\n";
        }
        
        virtual std::vector< double >& getHistory() = 0;
        
        /// Returns the last sampled *raw* fitness value.
        virtual double last( void ) = 0;
        
    protected:
        PopulationFitnessPtr myParent;  //< Non-owning.
	};
    
    class HistoricalFitnessFunc : public FitnessFunc
    {
    public:
        HistoricalFitnessFunc( PopulationFitnessPtr argParent )
        : FitnessFunc( argParent )
        { }
        virtual void reset( void ) { myHistory.clear(); }
        virtual double finalFitness( void ) { return last(); }
        std::vector< double >& getHistory() { return myHistory; }
        double last( void ) { return ( myHistory.empty() ? 0 : myHistory.back() ); }
    protected:
        std::vector< double > myHistory;
    };

}

#endif
