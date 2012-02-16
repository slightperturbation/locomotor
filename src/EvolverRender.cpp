/*
 *  EvolverDB.cpp
 * 
 *  Main file for evolverRenderServer
 *  Provides a cmd-line executable that interacts with a mysql database
 *  to get animations to render, writes the RIB file to disk, and 
 *  optionally spawns the render.
 *
 *  Created by vector on 9/03/10.
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
#include <PhysicsSimulatorFactory.hpp>
#include <ArticulatedBodyFactory.hpp>

#include <ArticulatedBody.hpp>

#include <string>
#include <sstream>
#include <QDir>
#include <QString>
#include <QProcess>
#include <QProcessEnvironment>


// Argh-- this should be from the info.plist
const double CURRENT_PROJECT_VERSION = 1.3;

struct DBProgramOptions
{
    std::string databaseServer;
    int databasePort;
    int experId;
    std::string label;
    std::string renderDir;
    
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
        if( "-renderDir" == opt )
        {
            val >> po.renderDir;
            std::cout << "RIB Render directory specified as " << po.renderDir << " from (" << val.str() << ")\n";
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
		std::cerr << "\nUsage: evolverRender [-db databaseServer] [-port databasePort] \n\n"
		<< "\tAll parameters are optional and can occur in any order with the following meanings (defaults shown in parens):\n"
		<< "\t[-db DatabaseServer] - IP or DNS name for host running mysql database (" << def.databaseServer << ")\n"
		<< "\t[-port DatabasePort] - port number to tunnel to databaseServer (" << def.databasePort << ")\n"
        << "\t[-label DescriptiveLabel] - a label to attach to this trial run (" << def.label << ")\n\n"
        << "\t[-renderDir RenderDirectory] - directory to place rib files (" << def.renderDir << ")\n\n"
        << "An example call would be:\n\n\t\t"
        << "evolverRender -db evocomp.org -port 3999 -renderDir ribs \n\n"
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
    
    /// isServer is a flag that says that we should act as 
    /// a server, polling the render_queue table for new commands.
    bool isServer = ( po.experId == 0 );

    std::string renderDir( po.renderDir );
    long int renderId = 0;
    long int performanceId = 0;
    long int experId = po.experId;
    do
    {
        bool haveWork = false;
        while( isServer && !haveWork )
        {
            // add a random sleep timer to space-out processes
            int ms = 1000 + rand() % 3000;
            usleep( ms );
            {
                mysql.query( "BEGIN;" );
                std::stringstream ssql;                
                ssql 
                << " SELECT id, performance_id, experiment_id "
                << " FROM render_queue "
                << " WHERE start_time IS NULL "
                << " ORDER BY priority DESC ";
                Rows rows;
                std::string sql = ssql.str();
                mysql.query( sql, rows );
                if( rows.empty() )
                {
                    mysql.query( "COMMIT;" );
                    sleep( 60 + rand() % 30 ); // wait sixty-ninty seconds to check again
                    continue;
                }
                // must have something by here
                Row r = rows.front();
                
                renderId = r["id"].as<int>();
                performanceId = r["performance_id"].as<int>();
                experId = r["experiment_id"].as<int>();
                
                if( renderId < 1 || experId < 1 )
                {
                    std::cerr << "Got Render Job ID " << renderId << " from DB"
                              << " for exper ID " << experId << "\n"; 
                    std::cerr << "Ignoring render job with id " << renderId << "\n";
                    haveWork = false;
                    continue;
                }
            }
            {
                std::stringstream sql;
                sql << "UPDATE render_queue SET start_time = NOW() WHERE id = " << renderId ;
                mysql.query( sql.str() );
            }
            mysql.query( "COMMIT;" );
            haveWork = true;
        }
        // Report beginning
        std::cerr << "Starting Render Job ID " << renderId << " from DB.\n";
        

                
        //  INITIALIZE PHYSICS	
        std::cerr << " done.\nInitializing PhysicsSimulator...";
        PhysicsSimulatorFactoryPtr myPhysicsSimFactory = loadPhysicsParamsFromDB( mysql, experId );
        ArticulatedBodyFactoryPtr myArticulatedBodyFactory = loadMorphForExperimentFromDB( mysql, experId );
        PhysicsSimulatorPtr sim = myPhysicsSimFactory->create();
        ArticulatedBodyPtr body = myArticulatedBodyFactory->create( sim );

        std::vector< BodyState > initialState;
        loadInitialStateFromDB( mysql, experId, initialState );
        
        std::stringstream ss;
        ss << "performanceId_" << performanceId;

        QDir dir(".");
        if( !renderDir.empty() )
        {
            renderDir = ss.str();
        }
        dir = QDir( renderDir.c_str() );
        if( dir.exists() )
        {
            std::cerr << "Making directory: " << dir.absolutePath().toStdString() << " -> " << ss.str().c_str() << "\n";
            dir.mkdir( ss.str().c_str() );
            dir.cd( ss.str().c_str() );
            std::cerr << "Now in dir: " << dir.absolutePath().toStdString() << "\n";
        }

        ss << ".rib";
        QString fileName = ss.str().c_str();
        QString filePath = QDir::cleanPath( dir.filePath( fileName ) );

        // INITIALIZE RIB RENDERER
        std::cerr << " done.\nInitializing RIB Renderer to file: " << filePath.toStdString() << "...";
        laetoli::OfflineRenderer* renderer = new laetoli::FileRibRenderer( filePath.toStdString() );

        sim->addRenderer( renderer );
        renderer->addSceneRenderable( body );

        // Load performance
        std::cerr << " done.\nLoading states...";
        std::vector< std::vector< Else::BodyState > > states;
        loadPerformance( mysql, performanceId, states );
        std::cerr << " done.\nSetting up renderer...";

        // Setup render
        renderer->setName( ss.str() );
        renderer->beginAnimation();

        // replay each state
        std::vector< std::vector< Else::BodyState > >::iterator iter = states.begin();
        std::vector< std::vector< Else::BodyState > >::iterator end = states.end();

        std::cerr << " done.\nRendering frames...\n";
        int myFrame = 1;
        for( ; iter != end; ++iter )
        {
            std::cerr << "Rendering frame: " << myFrame << "\n";
            renderer->beginFrame( myFrame++ );

            std::vector< Else::BodyState >& state = *iter;
            body->setBodyState( state );
            renderer->renderScene();

            renderer->endFrame();
        }
        renderer->endAnimation();
        std::cerr << "\nRIB saved.\n";

        // Do render
        QStringList rndrArgs;
        rndrArgs << "-t:12" << fileName;
        QProcess rndrProcess;
        rndrProcess.setWorkingDirectory( dir.absolutePath() );
        bool isComplete = false;
        bool isError  = false;
        while( !isComplete && !isError )
        {
            std::cerr << "In dir: " << rndrProcess.workingDirectory().toStdString() << "\n"
                      << "\t starting renderer: \"rndr ";
            foreach( QString s, rndrArgs )
            {
                std::cerr << s.toStdString() << " ";
            }
            std::cerr << "\"\n\n";
            
            rndrProcess.start( "rndr", rndrArgs );
            if( !rndrProcess.waitForStarted(10000) )  // ten seconds
            {
                std::cerr << "Can't start render process!  Returning to render queue.\n";
                // can't start, give to somebody else
                std::stringstream sql;
                sql << "UPDATE render_queue SET start_time = NULL WHERE id = " << renderId ;
                mysql.query( sql.str() );
                isError = true;
            }
            else
            {
                if( !rndrProcess.waitForFinished(8*60*60*1000) ) // eight hours
                {
                    std::cerr << "Render failed!  retrying\n";
                    //TODO -- look for last file rendered

                    // for now, just error out
                    isError = true;
                }
                else
                {
                    std::cerr << "\nRender finished!\n";
                    // Started && Finished!
                    isComplete = true;
                }
            }
        }
        if( isComplete && !isError )
        {
            std::cerr << "Finished with render queue item, updated database...\n";
            std::stringstream sql;
            sql << "UPDATE render_queue SET completion_time = NOW() WHERE id = " << renderId ;
            mysql.query( sql.str() );
        }
    } while( isServer );
	
    return;
}


