#ifndef DB_PERFORMANCE_TO_RIB_HPP
#define DB_PERFORMANCE_TO_RIB_HPP
/*
 *  dbPerformanceToRIB.h
 *  else
 *
 *  Created by vector on 8/6/09.
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
#include <EvolverFactory.hpp>
#include <Evolver.hpp>
#include <NeatEvolver.hpp>
#include <Renderer.hpp>
#include <RibRenderer.hpp>
#include <MySqlDB.hpp>
#include <MySqlDBEvoReporter.hpp>
#include <ParamsFromDB.hpp>
#include <DbPerformance.hpp>
#include <RibRenderer.hpp>

#include <string>
#include <sstream>

Else::CumulativeTimer* Else::gblTotalTimer;
Else::CumulativeTimer* Else::gblPhysicsTimer;
Else::CumulativeTimer* Else::gblControlTimer;
Else::CumulativeTimer* Else::gblEmbryogenesisTimer;
Else::CumulativeTimer* Else::gblIOTimer;


#include <ArticulatedBody.hpp>


struct DBProgramOptions
{
    std::string databaseServer;
    int databasePort;
    long int perfId;
    std::string label;
    int camera;
    bool isComposite;
    int compositeSteps;
    
	DBProgramOptions() : 
    databaseServer( "evocomp.org" ),
    databasePort( 19737 ),
    perfId( 0 ),
    label( "Unknown" ),
    camera( 0 ),
    isComposite( false ),
    compositeSteps( 10 )
    { }
};

std::ostream& operator << ( std::ostream& out, const DBProgramOptions& po )
{
	out 
    << "databaseServer       = " << po.databaseServer << "\n"
    << "databasePort         = " << po.databasePort << "\n"
    << "perfId               = " << po.perfId << "\n"
    << "label                = " << po.label << "\n"
    << "camera               = " << po.camera << "\n"
    << "isComposite          = " << po.isComposite << "\n"
    << "compositeSteps       = " << po.compositeSteps << "\n"
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
        if( "-perf" == opt )
		{
			val >> po.perfId;
			std::cout << "Performance ID specficied as " << po.perfId << " from (" << val.str() << ")\n";
			++i;
		}
        if( "-label" == opt )
        {
            val >> po.label;
            std::cout << "Label specified as " << po.label << " from (" << val.str() << ")\n";
            ++i;
        }
        if( "-camera" == opt )
        {
            val >> po.camera;
            std::cout << "Camera specified as " << po.camera << " from (" << val.str() << ")\n";
            ++i;
        }
        if( "-composite" == opt )
        {
            val >> po.compositeSteps;
            if( po.compositeSteps > 0 )
            {
                po.isComposite = true;
            }
            std::cout << "IsComposit specified as " << po.isComposite << " with steps " << po.compositeSteps << " from (" << val.str() << "\)\n";
            ++i;
        }
	}
}        


int main( int argc, char* argv[] )
{
	using namespace Else;
    using namespace laetoli;
	
	if( argc == 2 && !std::strcmp( argv[1], "-h" ) )
	{
		DBProgramOptions def;
		std::cerr << "\nUsage: evolverDB [-db databaseServer] [-port databasePort] [-evo EvoParamsID] [-morph MorphID] [-label \"label string\"]\n\n"
		<< "\tAll parameters are optional and can occur in any order with the following meanings (defaults shown in parens):\n"
		<< "\t[-db DatabaseServer] - IP or DNS name for host running mysql database (" << def.databaseServer << ")\n"
		<< "\t[-port DatabasePort] - port number to tunnel to databaseServer (" << def.databasePort << ")\n"
		<< "\t[-perf PerformanceID] - ID of row in performance table specifying the animation to render. (" << def.perfId << ")\n"
        << "\t[-camera CameraID] - ID of the camera setting to use for the render.  (" << def.camera << ")\n"
        << "\t[-composite compositeSteps] - number of frames to skip for each composite frame, zero for an animation. (" << def.isComposite << ", " << def.compositeSteps << ")\n"
		//<< "\t[-morph MorphID] - ID of row in morph table specifying the morphology (" << def.morphId << ")\n"
        //<< "\t[-fitness FitFuncID] - ID of row in fitness_func table specifying the fitness function (" << def.fitnessFuncId << ")\n"
        << "\t[-label DescriptiveLabel] - a label to attach to this trial run (" << def.label << ")\n"
		<< "\nAn example call would be:\n\n\t\t"
		<< "evolverDB -db evocomp.org -port 3999 -perf 488 -label \"pop_size_test_512\"\n\n"
		;
		return 0;
	}
    
	std::ios_base::sync_with_stdio(true);
    
	DBProgramOptions po;
	parseDBOptions( argc, argv, po );
    
    std::cerr << "Program Options: " << po << "\n";
    
    gblTotalTimer = new Else::CumulativeTimer;
	gblPhysicsTimer = new Else::CumulativeTimer;
	gblControlTimer = new Else::CumulativeTimer;
	gblEmbryogenesisTimer = new Else::CumulativeTimer;
	gblIOTimer = new Else::CumulativeTimer;
    
    std::cerr << "Initializing DB\n";
    MySqlDB::port( po.databasePort );
    MySqlDB::server( po.databaseServer );
    MySqlDB& mysql = MySqlDB::instance();
    DbPerformance dbPerf;
    if( dbPerf.load( mysql, po.perfId ) )
    {
        std::cerr << "Error loading performance id " << po.perfId << ".  Aborting.\n";
        return -1;
    }
    vector< vector< BodyState > >& anim = dbPerf.getStates();
    
    std::stringstream filename;
    filename << po.label << "-" << po.camera << "_camera-" << po.perfId << "_perfId";
    if( po.isComposite )
    {
        filename << "-composite";
    }
    filename << ".rib";
    
    PhysicsSimulator sim;
    FileRibRenderer* rib = new FileRibRenderer( filename.str(), po.camera );
    ArticulatedBody* body = new ArticulatedBody( sim.getWorldID(), sim.getSpaceID() /*, body desc filename */ );
    sim.setBody( body );
    sim.addRenderer( rib );
    sim.begin();
    
    int frame = 1;
    if( po.isComposite ) 
    {
        rib->beginFrame( 1 );
        //rib.enableTransparency();
    }
    
    for( vector< vector< BodyState > >::const_iterator iter = anim.begin(); iter != anim.end(); ++iter )
    {
        ++frame;
        if( po.isComposite && ( frame % po.compositeSteps ) )
        {
            continue;
        }
        std::cerr << frame << "\t";
        body->setBodyState( *iter );
        if ( !po.isComposite ) rib->beginFrame( frame );
        rib->render( body );
        if ( !po.isComposite ) rib->endFrame();
    }
    std::cerr << "\ndone.\n";
    if( po.isComposite ) 
    {
        //rib.disableTransparency();
        rib->endFrame();
    }
    
    sim.end();
    //delete gblTotalTimer;
	//delete gblPhysicsTimer;
	//delete gblControlTimer;
	//delete gblEmbryogenesisTimer;
	//delete gblIOTimer;
    return 0;
}

#endif