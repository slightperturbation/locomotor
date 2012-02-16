/*
 *  DbPerformance.cpp
 *  else
 *
 *  Created by vector on 8/6/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#include <DbPerformance.hpp>
#include <MySqlDB.hpp>
#include <BodyState.hpp>
#include <iostream>
#include <sstream>
#include <stdio.h>

using namespace Else;
using namespace std;

DbPerformance
::DbPerformance( void )
: _timeStep( 0.0 )
{
    // NOOP
}

long int 
DbPerformance
::queryBestPerformance( MySqlDB& mysql )
{
    Rows rows;
    std::stringstream sql;
    sql << "SELECT c.performance_id as perfId FROM champion c WHERE c.raw_fitness = ( SELECT MAX( c2.raw_fitness ) FROM champion as c2 )";
    std::cerr << "About to query for best performance\n";
    mysql.query( sql.str(), rows );  
    return rows.front()["perfId"].as<long int>();
}

/// Returns 0 on success
int
DbPerformance
::load( MySqlDB& mysql, long int& argId )
{
    std::cerr << "Loading performance id " << argId << "\n";
    using namespace std;
    using namespace Else;
    if( argId == 0 )
    {
        argId = queryBestPerformance( mysql );
    }
    
    {
        Rows rows;
        std::stringstream sql;
        sql << "SELECT frame, body_states FROM state WHERE performance_id = " << argId 
        << " ORDER BY frame ASC ";
        std::cerr << "About to query for states as:\n" << sql.str() << "\n";
        mysql.query( sql.str(), rows );
        std::cerr << " done, loaded " << rows.size() << " rows.\n";
        if( rows.empty() )
        {
            //NSString* msg = [NSString stringWithFormat:@"No states for performance.id = %d", _performanceId] ; 
            //[_view logMsg:msg];
            //NSLog( msg );
            //sprintf(_lastErr, "No states with performance id=%ld", argId );
            _states.clear();
            return 1;
        }
        for( Rows::const_iterator i = rows.begin(); i != rows.end(); ++i )
        {
            const Row& r = *i;
            vector< BodyState > state;
            stringstream stateRow;
            std::string s = r["body_states"].text();
            stateRow.str( s );
            stateRow >> state;
            _states.push_back( state );
        }
        std::cerr << "Finished loading " << _states.size() << " states.\n";
    }
    {    
        std::stringstream sql;
        // neural_activations
        sql << "SELECT timestep, physics_updates_per_neural_updates FROM performance WHERE id = " << argId;
        Rows rows;
        std::cerr << "About to query for timestep...";
        mysql.query( sql.str(), rows );
        std::cerr << "done\n";
        if( rows.empty() )
        {
            //NSString* msg = [NSString stringWithFormat:@"No performance with id = %d", _performanceId] ; 
            //[_view logMsg:msg];
            //NSLog( msg );
            _timeStep = 0.0165;
            //sprintf( _lastErr, "No performance with id=%d.  Using default timestep.", argId );
            return 1;
        }
        _timeStep = rows.front()["timestep"].as<double>();
        std::cerr << "Timestep = " << _timeStep << "\n";
    }   
    return 0;
}

std::vector< std::vector< Else::BodyState > >& 
DbPerformance
::getStates( void )
{
    return _states;
}

double 
DbPerformance
::getTimeStep( void )
{
    return _timeStep;
}

char* 
DbPerformance
::getLastErr( void )
{
    return _lastErr;
}
