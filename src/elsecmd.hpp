
#define ELSE_CONSOLE_BUILD

#include <PhysicsSimulator.hpp>
#include <EvolverFactory.hpp>
//#include <GLTools.h>
#include <Evolver.hpp>
#include <NeatEvolver.hpp>


//int main( int argc, char* argv[] );

struct ElseCmd
{
	NEAT* myNeat;
	Else::PhysicsSimulatorPtr mySim;
	Else::EvolverPtr myEvolver;
	Else::FitnessFuncPtr myFitnessFunc;

	ElseCmd( char* argBodyDescFilename );

	double eval( char* argGenomeFilename );
};
