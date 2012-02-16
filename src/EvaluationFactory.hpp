#ifndef EVALUATIONFACTORY_HPP
#define EVALUATIONFACTORY_HPP

#include <Else.hpp>

class Genome; // for neat/genetics.h

namespace Else
{
	/// Abstract interface class for creating Evaluations
	/// see @EvaluationFactoryDB
    class EvaluationFactory
    {
    public:
        EvaluationFactory( void );

        /// Create a functor for the evaluation of the given genome.
        /// argPopulationIndex is required to be the index of the given genome
        /// within the population.  This is needed to index into the
        /// population fitness.
        virtual EvaluationPtr createEvaluation( int argPopulationIndex,
                                                Genome* argGenome,
                                                FitnessFuncPtr argFitnessFunc,
                                                int argGeneration ) = 0;
    };
}
#endif // EVALUATIONFACTORY_HPP

