/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_NEATEVOLVER_HPP
#define ELSE_NEATEVOLVER_HPP
//
// Copyright 2009, Brian F. Allen
//
// Contact: vector@acm.org
//

// Project Includes
#include <Else.hpp>
#include <Evolver.hpp>
#include <Evaluation.hpp>
#include <PopulationFitness.hpp>

// External Includes
#include <neat/neat.h>
#include <neat/genetics.h>
#include <neat/networks.h>

#include <string>
#include <sstream>

namespace Else 
{
    
    class NeatEvolver : public Evolver
    {
    public:
        NeatEvolver( EvaluationFactoryPtr argEvaluationFactoryPtr,
                     ControllerFactoryPtr argControllerFactoryPtr,
                     PopulationFitnessPtr argPopulationFitnessPtr,
                     NEAT* argConfig );
        
        ~NeatEvolver();
        
        virtual void go( EvoReporter* reporter );
        
    protected:
        /// Evaluates an individal genome.  Used for debugging.
        void initPop( size_t argPopSize );
        
        ///  Single run does one **evolutionary** run of **many generations**, not just one individual eval! (that's runIndividualGenome)
        void singleRun( EvoReporter* reporter );

        /// Ensure that myPop is valid and populated.
        void verifyPopulation( void );

        /// Evaluates the entire current population (myPop),
        /// saving the results to the reporter argument.
        /// Returns the best organism of the evaluation.
        Organism* evaluatePopulation( int argGen, int argGenTotal, EvoReporter* reporter );
        
    private:
        Population* myPop;  //< Owned
        PopulationFitnessPtr myPopFitness; //< Owned
        int myCurrGen;
        int myStepsPerEval;
        
        int myNumRuns;
        
        NEAT* myNeatConfig;  //< NOT Owned

        ControllerFactoryPtr myControllerFactory; //< NOT Owned
        
        std::vector< std::vector< BodyState > > myBestAnim;
        std::vector< std::vector< BodyState > > myBestAnimOfGen;
        std::vector< double > mySharedActivationHistory;
        
        double myBestFitness;        // across all gens so far
        double myBestFitnessCurrGen; // just current generation
        double myMeanFitnessCurrGen; // just current generation
        double mySDFitnessCurrGen;   // just current generation
    };
    
    
    // Free functions for converting a genome to an XML stream
    void addNetworkXML( Genome& argGenome, std::ostringstream& argOStream );
    
    void addLinkXML( Genome& argGenome, std::ostringstream& argOStream,
                    const Link& argLink );
    
    void addNodeXML( Genome& argGenome, std::ostringstream& argOStream, 
                    const NNode& argNode, 
                    double& argIPos,
                    double& argHPos,
                    int& argHLayer,
                    double& argOPos );
    
}

#endif


