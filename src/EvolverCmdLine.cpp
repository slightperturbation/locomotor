/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

// Internal Includes
#define ELSE_CONSOLE_BUILD

#include <Else.hpp>
#include <neat/neat.h>
#include <Tardis.hpp>
#include <PhysicsSimulator.hpp>
#include <FitnessFuncFactory.hpp>
#include <EvolverFactory.hpp>
#include <Evolver.hpp>
#include <NeatEvolver.hpp>
#include <Renderer.hpp>
#include <RibRenderer.hpp>
#include <FileEvoReporter.hpp>
#include <string>
#include <sstream>

Else::CumulativeTimer* Else::gblTotalTimer;
Else::CumulativeTimer* Else::gblPhysicsTimer;
Else::CumulativeTimer* Else::gblControlTimer;
Else::CumulativeTimer* Else::gblEmbryogenesisTimer;
Else::CumulativeTimer* Else::gblIOTimer;

struct ProgramOptions
{
	std::string fitnessFuncName;
	int genLimit;
	float harnessStrength; 
	std::string neatParamFilename;
	std::string bodyFilename;
	std::string initialGenomeFilename;
	bool useHarness;
	// ADD_OPTION: typed value

	ProgramOptions() : 
	fitnessFuncName( "Walk" ),
		genLimit( 2000 ),
		harnessStrength( 1.0 ),
		neatParamFilename( "default.ne" ),
		bodyFilename( "default.body" ),
		initialGenomeFilename( "" ), /* blank for a random one */
		useHarness( true )
	// ADD_OPTION: default value
		{ }
};

std::ostream& operator << ( std::ostream& out, const ProgramOptions& po )
{
	out << "fitnessFunc          = " << po.fitnessFuncName << "\n"
		<< "genLimit             = " << po.genLimit << "\n"
		<< "harnessStrength      = " << po.harnessStrength << "\n"
		<< "neatParamFilename    = " << po.neatParamFilename << "\n"
		<< "bodyFilename         = " << po.bodyFilename << "\n"
		<< "initialGenomeFilename = " << po.initialGenomeFilename << "\n"
		<< "useHarness           = " << po.useHarness << "\n";
	return out;
}

void parseOptions( int argc, char *argv[], ProgramOptions& po )
{
	for( int i = 0; i < argc; ++i )
	{
		std::string opt( argv[i] );
		// Process one-part options (eg, simple flags, param files id'd by extension)
		if( std::string::npos != opt.rfind( ".ne" ) )
		{
			po.neatParamFilename = opt;
			std::cout << "NEAT parameter file specified as " << po.neatParamFilename << "\n";
		}
		if( std::string::npos != opt.rfind( ".body" ) )
		{
			po.bodyFilename = opt;
			std::cout << "Body parameter file specified as " << po.bodyFilename << "\n";
		}
		if( std::string::npos != opt.rfind( ".genome" ) )
		{
			po.initialGenomeFilename = opt;
			std::cout << "Initial genome file specified as " << po.initialGenomeFilename << "\n";
		}

		if( (i+1) >= argc )
		{
			continue;
		}
		std::istringstream val( argv[i+1] );
		// Process two-part options (eg, "-g filename")
		if( "-f" == opt )
		{
			val >> po.fitnessFuncName;
			std::cout << "Fitness function specficied as " << po.fitnessFuncName << " from (" << val.str() << ")\n";
			++i;
		}
		if( "-g" == opt )
		{
			val >> po.genLimit;
			std::cout << "Gen Limit specficied as " << po.genLimit << " from (" << val.str() << ")\n";
			++i;
		}
		if( "-harnessStrength" == opt )
		{
			val >> po.harnessStrength;
			std::cout << "Harness Strength specficied as " << po.harnessStrength << " from (" << val.str() << ")\n";
			++i;
		}
		if( "-harness" == opt )
		{
			val >> po.useHarness;
			std::cout << "Use-harness specficied as " << po.useHarness << " from (" << val.str() << ")\n";
			++i;
		}
		// ADD_OPTION: parser
	}
}


void writeParameters( const std::string& outFilename, int argc, char* argv[], const ProgramOptions& po )
{
	std::ofstream settingsFile( outFilename.c_str(), std::ios::app );
	settingsFile << "\n-----------------------------------\n";
	for(int i = 0; i < argc; ++i )
	{
		settingsFile << argv[i] << " ";
	}	
	settingsFile << "\n\n-----------------------------------\n"
		<< po 
		<< "\n-----------------------------------\n"
	//	  << "Build: " << __TIMESTAMP__ << "\n"
		<< "\n DEFAULT_FORCE_SCALE\t" << DEFAULT_FORCE_SCALE
		<< "\n DEFAULT_USE_FAST_STEP\t" << DEFAULT_USE_FAST_STEP
		<< "\n DEFAULT_TIME_STEP\t" << DEFAULT_TIME_STEP
		<< "\n DEFAULT_TIME_SUBSTEPS\t" << DEFAULT_TIME_SUBSTEPS
		<< "\n DEFAULT_ERP\t" << DEFAULT_ERP
		<< "\n DEFAULT_CFM\t" << DEFAULT_CFM
		<< "\n DEFAULT_CONTROLLER_UPDATE_RATE\t" << DEFAULT_CONTROLLER_UPDATE_RATE
		<< "\n DEFAULT_ALLOW_INTERNAL_COLLISIONS\t" << DEFAULT_ALLOW_INTERNAL_COLLISIONS
		<< "\n DEFAULT_STEPS_PER_EVAL\t" << DEFAULT_STEPS_PER_EVAL
		<< "\n DEFAULT_HARNESS_HEIGHT\t" << DEFAULT_HARNESS_HEIGHT
		<< "\n DEFAULT_HARNESS_GENERATIONS\t" << DEFAULT_HARNESS_GENERATIONS
		<< "\n DEFAULT_HARNESS_FORWARD_VEL\t" << DEFAULT_HARNESS_FORWARD_VEL
		<< "\n DEFAULT_HARNESS_FORWARD_FACTOR\t" << DEFAULT_HARNESS_FORWARD_FACTOR
		<< "\n DEFAULT_HARNESS_VERTICAL_FACTOR\t" << DEFAULT_HARNESS_VERTICAL_FACTOR
		<< "\n DEFAULT_WALK_REQUIRED_HEIGHT\t" << DEFAULT_WALK_REQUIRED_HEIGHT
		<< "\n DEFAULT_STEP_REQUIRED_HEIGHT\t" << DEFAULT_STEP_REQUIRED_HEIGHT
		<< "\n DEFAULT_WALK_TIME_VALUE\t" << DEFAULT_WALK_TIME_VALUE
		<< "\n DEFAULT_GRAVITY\t" << DEFAULT_GRAVITY
		<< "\n DEFAULT_ANKLE_STRENGTH\t" << DEFAULT_ANKLE_STRENGTH
		<< "\n DEFAULT_COLLISION_ERP\t" << DEFAULT_COLLISION_ERP
		<< "\n DEFAULT_COLLISION_CFM\t" << DEFAULT_COLLISION_CFM
		<< "\n DEFAULT_STABILITY_THRESHOLD\t" << DEFAULT_STABILITY_THRESHOLD
		<< "\n KNEE_TORQUE_SCALE\t" << KNEE_TORQUE_SCALE
		<< "\n HIP_TORQUE_SCALE\t" << HIP_TORQUE_SCALE
		<< "\n";
	settingsFile << "\n-------------------------------\nNEAT PARAMETERS:\n\n";
	NEAT::write_neat_params( settingsFile );
	settingsFile.close();
}

int main( int argc, char* argv[] )
{
	using namespace Else;
	
	if( argc == 2 && !std::strcmp( argv[1], "-h" ) )
	{
		ProgramOptions def;
		std::cerr << "\nUsage: evolver [*.ne] [*.body] [*.genome] [-f FitnessFunc] [-g GenLimit] [-harness bool]\n\n"
		<< "\tAll parameters are optional and can occur in any order with the following meanings (defaults shown in parens):\n"
		<< "\t[*.ne] - NEAT parameters file (" << def.neatParamFilename << ")\n"
		<< "\t[*.body] - body (height, weight, etc) file (" << def.bodyFilename << ")\n"
		<< "\t[*.genome] - starting genome to base pop from, otherwise random (actually ignored now)\n"
		<< "\t[-f FitnessFunc] - name of the fitness func, eg, Balance, Walk, or Jump (" << def.fitnessFuncName << ")\n"
		<< "\t[-g GenLimit] - number of generations to run evolution, should be an int (" << def.genLimit << ")\n"
		<< "\t[-harness bool] - true of false to indicate if the harness should be used for early generations (" << def.useHarness << ")\n\n"
		<< "An example call would be:\n\n\t\t"
		<< "evolver largeMale.body -f Walk -g 4000 -harness true\n\n"
		;
		return 0;
	}

	std::cerr << "Configuring Options...\n";
	std::ios_base::sync_with_stdio(true);

	ProgramOptions po;
	parseOptions( argc, argv, po );
    
    std::srand( (unsigned)( std::time( NULL ) + (unsigned)getpid() ));

//	INITIALIZE NEAT
	std::cerr << "Initializing NEAT...\n";
	NEAT* neat; 
	neat = new NEAT();
	std::string filename( po.neatParamFilename );
	std::ifstream testFile( filename.c_str() );
	bool isGood = (bool)testFile;
	testFile.close();
	if( isGood )
	{
		neat->load_neat_params( filename.c_str() );
	}

//  INITIALIZE EVOLVER
	std::cerr << "Initializing Evolver...";
	Else::EvolverPtr evolver = EvolverPtr( new NeatEvolver( neat ) );
    
//  INITIALIZE FITNESS FUNC
	Else::FitnessFuncFactory fitnessFuncFactory;
	Else::FitnessFuncPtr fitnessFunc = fitnessFuncFactory.make( po.fitnessFuncName );

//  INITIALIZE PHYSICS	
	std::cerr << " done.\nInitializing PhysicsSimulator...";
	Else::PhysicsSimulatorPtr sim = PhysicsSimulatorPtr( new PhysicsSimulator() );
	std::cerr << " done.\nInitializing ArticulatedBody(using body file: " << po.bodyFilename.c_str() << ")... ";
	ArticulatedBody* body = new ArticulatedBody( sim->getWorldID(), sim->getSpaceID(), po.bodyFilename.c_str() );
	std::cerr << " done.\nAttaching ArticulatedBody to Simulator...\n";
	sim->setBody( body );

// INITIALIZE RIB RENDERER
    std::cerr << " done.\nInitializing RIB Renderer...";
    laetoli::OfflineRenderer* renderer = new laetoli::TmpRibRenderer();
    //renderer->init();
    sim->addRenderer( renderer );
    std::cerr << " done.\n";

	writeParameters( "startingParameters.txt", argc, argv, po );

	// profilers
	std::cerr << "Initializing Profilers...\n";
	gblTotalTimer = new Else::CumulativeTimer;
	gblPhysicsTimer = new Else::CumulativeTimer;
	gblControlTimer = new Else::CumulativeTimer;
	gblEmbryogenesisTimer = new Else::CumulativeTimer;
	gblIOTimer = new Else::CumulativeTimer;
    std::cerr << " done.\n";

	gblTotalTimer->start();

// INITIALIZE REPORTER
    EvoReporter* reporter = new FileEvoReporter( "evoreport.txt" );

	// Do evolution!

	std::cerr << "Starting Evolution...\n";
	evolver->go( fitnessFunc, sim, reporter );
	//evolver->go( neat->pop_size, po.genLimit, fitnessFunc, sim, po.initialGenomeFilename.c_str(), po.useHarness );
	std::cerr << "Finished Evolution...\n";


	std::ofstream timingFile( "profileTimes.txt", std::ios::app );
	timingFile << "-----------------------------\n"
	// << Build: " << __TIMESTAMP__
		<< "\nTotal Time:   " << gblTotalTimer->get() 
		<< "\nPhysics Time: " << gblPhysicsTimer->get() 
		<< "\nControl Time: " << gblControlTimer->get() 
		<< "\nEmbryo Time:  " << gblEmbryogenesisTimer->get()
		<< "\nIO Time:      " << gblIOTimer->get() 
		<< "\n";
	timingFile.close();

	delete gblTotalTimer;
	delete gblPhysicsTimer;
	delete gblControlTimer;
	delete gblEmbryogenesisTimer;
	delete gblIOTimer;
	return 0;
}

