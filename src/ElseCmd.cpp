#include "ElseCmd.hpp"
#include "FitnessFuncFactory.hpp"

#include <iostream>

Else::CumulativeTimer* Else::gblTotalTimer;
Else::CumulativeTimer* Else::gblPhysicsTimer;
Else::CumulativeTimer* Else::gblControlTimer;
Else::CumulativeTimer* Else::gblEmbryogenesisTimer;
Else::CumulativeTimer* Else::gblIOTimer;

using namespace Else;

int main( int argc, char* argv[] )
{

	if( argc != 3 )
	{
		//std::cerr << "Build: " << __TIMESTAMP__ ;
		std::cerr << "\n\nUsage: " << argv[0] << " <genomeFile> <bodyDescFile>\n\n";
		return 0;
	}

	// profilers
	gblTotalTimer = new Else::CumulativeTimer;
	gblPhysicsTimer = new Else::CumulativeTimer;
	gblControlTimer = new Else::CumulativeTimer;
	gblEmbryogenesisTimer = new Else::CumulativeTimer;
	gblIOTimer = new Else::CumulativeTimer;

	ElseCmd* theElse = new ElseCmd( argv[2] );
	double fitness = theElse->eval( argv[1] );   // cerr cause cout is polluted... :-(

	std::cout << fitness << "\n";

	delete gblTotalTimer;
	delete gblPhysicsTimer;
	delete gblControlTimer;
	delete gblEmbryogenesisTimer;
	delete gblIOTimer;

	return 0;
	// not deleting theElse due to destruction order bug... 
}


ElseCmd::ElseCmd( char* argBodyDescFilename )
{
	mySim = PhysicsSimulatorPtr( new PhysicsSimulator() );//"physicsSettings.txt" ) );
	ArticulatedBody* lclBody 
		= new ArticulatedBody( mySim->getWorldID(), mySim->getSpaceID(), argBodyDescFilename );
	mySim->setBody( lclBody );

	FitnessFuncFactory lclFFFactory;
	myFitnessFunc = FitnessFuncPtr( lclFFFactory.make( "Walk" ) );
	myEvolver = EvolverPtr( new NeatEvolver( myNeat ) );
	myEvolver->setUpdateRate( 2 ); // the 1.0 - 1.6 default value

}

double 
ElseCmd::eval( char* argGenomeFilename )
{
	gblEmbryogenesisTimer->start();
	myEvolver->createStartingGenome( mySim->getBody(), argGenomeFilename );
	gblEmbryogenesisTimer->stop();

	if( !myFitnessFunc )
	{
		FitnessFuncFactory lclFFFactory;
		myFitnessFunc = FitnessFuncPtr( lclFFFactory.make( "Walk" ) );
	}

	bool hasHarness = false; //mySingleRunHasHarness->value();
	myEvolver->runIndividualGenome( myFitnessFunc, mySim, hasHarness );

	return myFitnessFunc->last();
}
