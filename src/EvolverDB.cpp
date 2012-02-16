/*
 *  EvolverDB.cpp
 *  else
 * 
 *  Provides a cmd-line executable that interacts with a mysql database
 *  to retrieve parameters and store execution data.
 *
 *  Created by vector on 7/20/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */



// Internal Includes
#define ELSE_CONSOLE_BUILD

#include <Else.hpp>
#include <neat/neat.h>
#include <Tardis.hpp>
#include <PhysicsSimulator.hpp>
#include <FitnessFuncFactory.hpp>
#include <Evolver.hpp>
#include <NeatEvolver.hpp>
#include <Renderer.hpp>
#include <RibRenderer.hpp>
#include <MySqlDB.hpp>
#include <MySqlDBEvoReporter.hpp>
#include <ParamsFromDB.hpp>
#include <EvaluationFactoryDB.hpp>

#include <string>
#include <sstream>

#include <QProcess>

Else::CumulativeTimer* Else::gblTotalTimer;
Else::CumulativeTimer* Else::gblPhysicsTimer;
Else::CumulativeTimer* Else::gblControlTimer;
Else::CumulativeTimer* Else::gblEmbryogenesisTimer;
Else::CumulativeTimer* Else::gblIOTimer;


#include <ArticulatedBody.hpp>

//TODO this should be from the info.plist
const double CURRENT_PROJECT_VERSION = 1.3;

struct DBProgramOptions
{
    std::string databaseServer;
    int databasePort;
    int experId;
    std::string label;
    
    DBProgramOptions() :
            databaseServer( "evocomp.org" ),
            databasePort( 31415 ),
            experId( 0 ),
            label( "" )
    { }
};

std::ostream& operator << ( std::ostream& out, const DBProgramOptions& po )
{
	out 
    << "databaseServer       = " << po.databaseServer << "\n"
    << "databasePort         = " << po.databasePort << "\n"
    << "experId              = " << po.experId << "\n"
    //<< "morphId              = " << po.morphId << "\n"
    //<< "fitnessFuncId        = " << po.fitnessFuncId << "\n"
    << "label                = " << po.label << "\n"
    ;
	return out;
}

void parseDBOptions( int argc, char *argv[], DBProgramOptions& po )
{
	for( int i = 0; i < argc; ++i )
	{
		std::string opt( argv[i] );
        
        //
        // No one-part options
        //
        
        // Process two-part options (eg, "-g filename")
        if( (i+1) >= argc )
		{
			continue;
		}
        std::istringstream val( argv[i+1] );
        if( "-db" == opt )
		{
			val >> po.databaseServer;
			std::cout << "Database server specficied as " << po.databaseServer << " from (" << val.str() << ")\n";
			++i;
		}
        if( "-port" == opt )
		{
			val >> po.databasePort;
			std::cout << "Database port specficied as " << po.databasePort << " from (" << val.str() << ")\n";
			++i;
		}
        if( "-evo" == opt )
		{
			val >> po.experId;
			std::cout << "Evolve params ID specficied as " << po.experId << " from (" << val.str() << ")\n";
			++i;
		}
        /*
         if( "-morph" == opt )
         {
         val >> po.morphId;
         std::cout << "Morphology ID specficied as " << po.morphId << " from (" << val.str() << ")\n";
         ++i;
         }
         if( "-fitness" == opt )
         {
         val >> po.fitnessFuncId;
         std::cout << "Fitness Function ID specified as " << po.fitnessFuncId << " from (" << val.str() << ")\n";
         ++i; 
         }
         */
        if( "-label" == opt )
        {
            val >> po.label;
            std::cout << "Trial label specified as " << po.label << " from (" << val.str() << ")\n";
            ++i;
        }
	}
}        

// TEMP for verifying the DB-set params
void writeParameters( const std::string& outFilename, int argc, char* argv[], const DBProgramOptions& po )
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

void runWithOptions( int argc, char* argv[], DBProgramOptions& po );

int main( int argc, char* argv[] )
{
	using namespace Else;
	
	if( argc == 2 && !std::strcmp( argv[1], "-h" ) )
	{
		DBProgramOptions def;
		std::cerr << "\nUsage: evolverDB [-db databaseServer] [-port databasePort] [-evo EvoParamsID] [-morph MorphID] [-label \"label string\"]\n\n"
		<< "\tAll parameters are optional and can occur in any order with the following meanings (defaults shown in parens):\n"
		<< "\t[-db DatabaseServer] - IP or DNS name for host running mysql database (" << def.databaseServer << ")\n"
		<< "\t[-port DatabasePort] - port number to tunnel to databaseServer (" << def.databasePort << ")\n"
		<< "\t[-evo EvoParamsID] - ID of row in evolve table specifying the evolutionary params, or 0 to act as an on-demand server (" << def.experId << ")\n\n"
		//<< "\t[-morph MorphID] - ID of row in morph table specifying the morphology (" << def.morphId << ")\n"
        //<< "\t[-fitness FitFuncID] - ID of row in fitness_func table specifying the fitness function (" << def.fitnessFuncId << ")\n"
        << "\t[-label DescriptiveLabel] - a label to attach to this trial run (" << def.label << ")\n\n"
		<< "An example call would be:\n\n\t\t"
		<< "evolverDB -db evocomp.org -port 3999 -evo 12 -morph 15 -label \"Pop size test - 512\"\n\n"
		;
		return 0;
	}
    
	std::cerr << "Configuring Options...\n";
	std::ios_base::sync_with_stdio(true);
    
	DBProgramOptions po;
	parseDBOptions( argc, argv, po );
    runWithOptions( argc, argv, po );
}

void runWithOptions( int argc, char* argv[], DBProgramOptions& po )
{
 	using namespace Else;
    
    unsigned int randomSeed = (unsigned)( std::time( NULL ) + (unsigned)getpid() );
    std::srand( randomSeed );
    
    //  INITIALIZE DATABASE
    MySqlDB::server( po.databaseServer );
    MySqlDB::port( po.databasePort );
    MySqlDB& mysql = MySqlDB::instance();
    
    bool isServer = ( po.experId == 0 );
    
    do
    {
        bool haveWork = false;
        while( isServer && !haveWork )
        {
            // add a random sleep timer to space-out processes
            // this is in lieu of transaction support on the DB
            int ms = 1000 + rand() % 3000;
            usleep( ms );
            
            
            int remainingTrials = 0;
            // TODO -- NEED TRANSACTION HERE
            // 0 is the special case to act like a server, getting the tasks from the experiment 
            {
                mysql.query( "BEGIN;" );
                std::stringstream ssql;                
                ssql 
                << " SELECT id, remaining_trials "
                << " FROM experiment "
                << " WHERE remaining_trials > 0 "
                << " AND target_trials > 0 "
                << " AND min_version >= " << CURRENT_PROJECT_VERSION
                << " ORDER BY PRIORITY DESC ";
                Rows rows;
                std::string sql = ssql.str();
                mysql.query( sql, rows );
                if( rows.empty() )
                {
                    mysql.query( "COMMIT;" );
                    sleep( 90 ); // wait ninty seconds to check again
                    continue;
                }
                Row r = rows.front();
                po.experId = r["id"].as<int>();
                remainingTrials = r["remaining_trials"].as<int>();
            }
            {
                std::stringstream sql;
                if( remainingTrials > 1 )
                {
                    sql << "UPDATE experiment SET remaining_trials = " << ( remainingTrials - 1 )
                    << " WHERE id = " << po.experId ;                    
                }
                else
                {
                    sql << "UPDATE experiment SET remaining_trials = 0, completion_time = NOW() WHERE id = " << po.experId ;
                }
                mysql.query( sql.str() );
            }
            mysql.query( "COMMIT;" );
            haveWork = true;
        }
        // Report beginning
        std::cerr << "Reading experimentID " << po.experId << " from DB.\n";
        
        EvaluationFactoryPtr evaluationFactory( new EvaluationFactoryDB( mysql, po.experId ) );

        //	INITIALIZE NEAT
        std::cerr << "Initializing NEAT...\n";
        NEAT* neat;
        neat = new NEAT();
        loadNEATParamsFromDB( mysql, po.experId, neat );

        // INITIALIZE CONTROLLER FACTORY
        ControllerFactoryPtr controllerFactory( loadControllerFactoryFromDB( mysql, po.experId ) );

        // INITIALIZE POPULATION FITNESS (need pop-wide fitness to support novelty search)
        PopulationFitnessPtr popFitness = loadFitnessFuncFromDB( mysql, po.experId );

        //  INITIALIZE EVOLVER
        std::cerr << "Initializing Evolver...";
        Else::EvolverPtr evolver
                = EvolverPtr( new NeatEvolver( evaluationFactory,
                                               controllerFactory,
                                               popFitness,
                                               neat ) );
        
        // INITIALIZE RIB RENDERER
        //std::cerr << " done.\nInitializing RIB Renderer...";
        //laetoli::OfflineRenderer* renderer = new laetoli::TmpRibRenderer();
        //sim->addRenderer( renderer );
        //std::cerr << " done.\n";
        
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
        
        // Do evolution!
        EvoReporter* reporter = new MySqlDBEvoReporter( mysql, po.experId, po.label );
        
        std::cerr << "Starting Evolution...\n";
        evolver->go( reporter );
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
    } while( isServer );
	
    return;
}









