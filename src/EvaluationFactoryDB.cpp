#include "EvaluationFactoryDB.hpp"
#include "Evaluation.hpp"
#include "ControllerFactory.hpp"
#include "PopulationFitness.hpp"
#include "PhysicsSimulatorFactory.hpp"
#include "ArticulatedBodyFactory.hpp"
#include <ParamsFromDB.hpp>

Else::EvaluationFactoryDB
::EvaluationFactoryDB( MySqlDB& argSql, int argExperId )
    : mySql( argSql ),
      mySimStepLimit( 10000 ),
      myDisplayRate( 1 ),
      myUpdateRate( 5 ),
      myHarnessGens( 300 )  // TODO -- pull constants from ???
{
    // connect to DB and read everything necessary to later createEvalautions()

    //  INITIALIZE PHYSICS & BODY
    //  this instance acts as a prototype, being duplicated to create
    //  instances of evalutions.
    std::cerr << "Initializing PhysicsSimulator and ArticulatedBody...";
    myPhysicsSimFactory = loadPhysicsParamsFromDB( mySql, argExperId );
    myArticulatedBodyFactory = loadMorphForExperimentFromDB( mySql, argExperId );
    loadInitialStateFromDB( mySql, argExperId, myInitialState );
    std::cerr << "done.\n";

    //  INITIALIZE FITNESS FUNC
    //std::cerr << "Initializing Fitness Function population...";
    //myPopulationFitness = loadFitnessFuncFromDB( mySql, argExperId );
    //assert( myPopulationFitness );
    //std::cerr << "done.\n";

    // INITIALIZE CONTROLLER FACTORY
    myControllerFactory = loadControllerFactoryFromDB( mySql, argExperId );

}


Else::EvaluationPtr
Else::EvaluationFactoryDB
::createEvaluation( int argPopulationIndex,
				    Genome* argGenome,
                    FitnessFuncPtr argFitness,
                    int argGeneration )
{
    // access DB for given experiment id
    // pull out:
    //  controller type
    //  fitness func
    //  morph/articulated body (part of the simulator)
    //  initial state
    //  physics sim
    //
    // build controller
	
    PhysicsSimulatorPtr sim = myPhysicsSimFactory->create();
    ArticulatedBodyPtr body = myArticulatedBodyFactory->create( sim );
	ControllerPtr controller = myControllerFactory->create( argGenome );

    return EvaluationPtr( new Evaluation( mySimStepLimit,
                                          controller,
                                          argFitness,
                                          sim,
                                          body,
                                          myInitialState,
                                          myDisplayRate,
                                          myUpdateRate,
                                          myHarnessGens ) );

}
