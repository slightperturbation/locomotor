#ifndef EVALUATIONFACTORYDB_HPP
#define EVALUATIONFACTORYDB_HPP

#include <Else.hpp>
#include <BodyState.hpp>
#include <EvaluationFactory.hpp>
#include <MySqlDB.hpp>
#include <neat/genetics.h>

namespace Else
{
    class EvaluationFactoryDB : public EvaluationFactory
    {
    public:
        EvaluationFactoryDB( MySqlDB& argSql, int argExperimentId );

        /// Create a functor for the evaluation of the given genome.
        virtual EvaluationPtr createEvaluation( int argPopulationIndex,
                                                Genome* argGenome,
                                                FitnessFuncPtr argFitnessFunc,
                                                int argGeneration );
    private:
        MySqlDB& mySql;
        //PopulationFitnessPtr myPopulationFitness;
        PhysicsSimulatorFactoryPtr myPhysicsSimFactory;
        ArticulatedBodyFactoryPtr myArticulatedBodyFactory;
        std::vector< BodyState > myInitialState;
        ControllerFactoryPtr myControllerFactory;

        int mySimStepLimit;
        int myDisplayRate;
        int myUpdateRate;
        int myHarnessGens;
    };
}
#endif // EVALUATIONFACTORYDB_HPP
