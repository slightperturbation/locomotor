/*
 *  queueDesignatorEvolves.cpp
 *  else
 *
 *  Created by vector on 9/23/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org). All rights reserved.
 *
 */

#include "queueDesignatorEvolves.h"
#include "DBExperiment.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <MySqlDB.hpp>


struct DesignatorEvolvesOptions
{
    std::string databaseServer;
    int databasePort;
    long int perfId;
    long int experId;
    int step;
    int trials;
    std::string label;

	DesignatorEvolvesOptions() 
    : databaseServer( "evocomp.org" ),
      databasePort( 31416 ),
      perfId( 0 ),
      experId( 0 ),
      step( 10 ),
      trials( 1 ),
      label( "Unknown" )
    {
        
    }
};

std::ostream& operator << ( std::ostream& out, const DesignatorEvolvesOptions& po )
{
	out 
    << "databaseServer       = " << po.databaseServer << "\n"
    << "databasePort         = " << po.databasePort << "\n"
    << "perfId               = " << po.perfId << "\n"
    << "experId              = " << po.experId << "\n"
    << "step                 = " << po.step << "\n"
    << "trials               = " << po.trials << "\n"
    << "label                = " << po.label << "\n"
    ;
	return out;
}

void parseDesignatorEvolvesOptions( int argc, char *argv[], DesignatorEvolvesOptions& po )
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
        if( "-exper" == opt )
		{
			val >> po.experId;
			std::cout << "Experiment ID specficied as " << po.experId << " from (" << val.str() << ")\n";
			++i;
		}
        if( "-step" == opt )
        {
            val >> po.step;
            std::cout << "Step size specified sa " << po.step << " from (" << val.str() << ")\n";
            ++i;
        }
        if( "-trials" == opt )
        {
            val >> po.trials;
            std::cout << "Number of trials specified sa " << po.trials << " from (" << val.str() << ")\n";
            ++i;
        }
	}
}        

bool idExists( MySqlDB& mysql, long int id, const std::string& tableName )
{
    std::stringstream sql;
    sql << "SELECT COUNT(id) AS count FROM " << tableName << " WHERE id = " << id;
    Rows rows;
    mysql.query( sql.str(), rows );
    if( rows.empty() )
    {
        // failure
        std::cerr << "Failure to check id " << id << " on table " << tableName << ".  Check database connection.\n";
        return false;
    }
    int count = rows.front()["count"].as<int>();
    if( count == 0 )
    {
        std::cerr << "Didn't find id " << id << " on table " << tableName << ".\n";
        return false;
    }
    return true;
}



int main( int argc, char* argv[] )
{
    //using namespace Else;
    //using namespace laetoli;
    using namespace std;
    
    if( argc == 2 && !std::strcmp( argv[1], "-h" ) )
	{
		DesignatorEvolvesOptions def;
		cerr << "\nUsage: evolverDB [-db databaseServer] [-port databasePort] [-evo EvoParamsID] [-morph MorphID] [-label \"label string\"]\n\n"
		<< "\tAll parameters are optional and can occur in any order with the following meanings (defaults shown in parens):\n"
		<< "\t[-db DatabaseServer] - IP or DNS name for host running mysql database (" << def.databaseServer << ")\n"
		<< "\t[-port DatabasePort] - port number to tunnel to databaseServer (" << def.databasePort << ")\n"
		<< "\t[-perf PerformanceID] - ID of row in performance table specifying the animation to render. (" << def.perfId << ")\n"
        << "\t[-exper ExperID] - ID of the base experiment id to use as a template.  (" << def.experId << ")\n"
        << "\t[-step StepSize] - number of frames to skip between initial states for evolves (0 makes an evolve for every frame in performance).  (" << def.step << ")\n"
        << "\t[-trials TrialCount] - the number of trials to run per initial state (the single best champ will be taken from all trials) (" << def.trials << ")\n"
        << "\t[-label DescriptiveLabel] - a label to attach to this designator (" << def.label << ")\n"
		<< "\nAn example call would be:\n\n\t\t"
		<< "queueDesignatorEvolves -db evocomp.org -port 3999 -perf 488 -exper 100 -step 10 -trials 5 -label \"Creating first test Designator Controller.\"\n\n"
		;
		return 0;
	}
    
    std::ios_base::sync_with_stdio(true);

    DesignatorEvolvesOptions po;
    parseDesignatorEvolvesOptions( argc, argv, po );
    
    MySqlDB::port( po.databasePort );
    MySqlDB::server( po.databaseServer );
    MySqlDB& mysql = MySqlDB::instance();
    

    if( !idExists( mysql, po.perfId, "performance" ) || !idExists( mysql, po.experId, "experiment" ) )
    {
        std::cerr << "Check ID parameters.  Aborting.\n";
        return -11;
    }

    // step through the states in the performance
    // for each state, create a new experiment
    vector< long int > stateIds;
    {
        stringstream sql;
        sql << "SELECT id FROM state WHERE performance_id = " << po.perfId;
        Rows rows;
        mysql.query( sql.str(), rows );
        if( rows.empty() )
        {
            cerr << "Specified performance_id=" << po.perfId << " has no associated states!  (or possibly a problem connecting the DB)\n";
            return -12;
        }
        for( Rows::const_iterator i = rows.begin(); i != rows.end(); ++i )
        {
            const Row& r = *i;
            stateIds.push_back( r["id"].as<long int>() );
        }
    }
    
    // load up all the experiment data, since we'll copy it over
    DBExperiment baseExper( po.experId );
    
    for( int s = 0; s < stateIds.size(); s += po.step )
    {
        long int stateId = stateIds[s];
        // add a new experiment for each stateIds
        DBExperiment newExper( baseExper );
        newExper.initialStateId = stateId;
        std::stringstream name;
        name << "DesignatorEvolve - copy from exper " << po.experId;
        newExper.name = name.str();
        newExper.trials = po.trials;
        newExper.desc = po.label;
        // Write to DB
        newExper.store();
        std::cout << "Created exper id = " << newExper.id << "\n";
    }
    return 0;
}
