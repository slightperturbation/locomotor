#ifndef ELSE_POPULATIONFITNESS_HPP
#define ELSE_POPULATIONFITNESS_HPP
//
// Copyright (C) 2009, Brian F. Allen
//
// Contact: vector@acm.org
//

#include <Else.hpp>
#include <FitnessFuncFactory.hpp>
#include <FitnessFunc.hpp>

#include <boost/enable_shared_from_this.hpp>

#include <string>
#include <vector>
#include <map>


namespace Else
{
    /// Holds a fitness function for each member of the population.
    /// Note that all instances must be held by boost::shared_ptrs.
    class PopulationFitness : public boost::enable_shared_from_this< PopulationFitness >
    {
    public:
        /// Create a new fitness tracker, specifying the string
        /// that denotes the type, e.g., "Walk"
        PopulationFitness( const std::string& argFitnessType );
        
        /// Each param set here will be passed to the created fitness
        /// functions.
        void setDoubleParam( const std::string& argName, double argValue );
        
        /// Create an instance of the FitnessFunc suitable for evaluation
        /// of argPopSize individuals, indexed from 0 to argPopSize - 1
        /// Any archive updates must have already taken place by 
        /// calling endGeneration prior to calling this.
        void resetToPopSize( size_t argPopulationSize );
        size_t popSize( void ) ;
        
        /// Retrieve individual fitness func by index
        FitnessFuncPtr operator[] ( size_t argIndex );

        /// Record the end of the generation.
        void endGeneration( void );
        
        std::vector< FitnessFuncPtr >& getCurrGenerationFitnessFuncs( void )
        {
            return myInstances;
        }
        std::vector< FitnessFuncPtr >& getHistoricalFitnessFuncs( void )
        {
            return myHistoricalFitnessFuncs;
        }
        
        FitnessFuncFactory& getFactory( void ) { return myFactory; }
        
    private:
        FitnessFuncPtr createInstance( void );
        
        FitnessFuncFactory myFactory;
        
        // Archive of historical strategies
        std::vector< FitnessFuncPtr > myHistoricalFitnessFuncs;
        
        std::string myFitnessType;
        std::map< std::string, double > myDoubleParams;
        
        /// Aggregate values will be accumulated for the generation
        /// by interating over the instances.
        /// It is assumed that these are all of the single correct type.
        std::vector< FitnessFuncPtr > myInstances;
    };
}

#endif
