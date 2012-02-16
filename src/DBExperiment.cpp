/*
 *  DBExperiment.cpp
 *  else
 *
 *  Created by vector on 9/24/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#include "DBExperiment.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <MySqlDB.hpp>

using namespace std;

DBExperiment::DBExperiment( dbid argExperId )
: id( argExperId ),
  evolveId( 0 ),
  fitnessFuncId( 0 ),
  morphId( 0 ),
  physicsId( 0 ),
  initialStateId( 0 ),
  name( "UNINITIALIZED" ),
  desc( "" ),
  physics_updates_per_neural_updates( 0 ),
  trials( 0 ),
  priority( 0.0 ),
  min_version( 0.0 )
{
    MySqlDB& mySql = MySqlDB::instance();
    Row row;
    stringstream sql; 
    sql << "SELECT * FROM experiment WHERE id = " << argExperId ;
    mySql.query( sql.str(), row );
    evolveId = row["evolve_id"].as<dbid>();
    fitnessFuncId = row["fitness_func_id"].as<dbid>();
    morphId = row["morph_id"].as<dbid>();
    physicsId = row["physics_id"].as<dbid>();
    initialStateId = row["initial_state_id"].as<dbid>();
    name = row["name"].as<string>();
    desc = row["description"].as<string>();
    physics_updates_per_neural_updates = row["physics_updates_per_neural_updates"].as<int>();
    trials = row["target_trials"].as<int>();
    priority = row["priority"].as<double>();
    min_version = row["min_version"].as<double>();
}
 
DBExperiment::DBExperiment( const DBExperiment& src )
: id( 0 ),
  evolveId( src.evolveId ),
  fitnessFuncId( src.fitnessFuncId ),
  morphId( src.morphId ),
  physicsId( src.physicsId ),
  initialStateId( src.initialStateId ),
  name( src.name ),
  desc( src.desc ),
  physics_updates_per_neural_updates( src.physics_updates_per_neural_updates ),
  trials( src.trials ),
  priority( src.priority ),
  min_version( src.min_version )
{
}

void 
DBExperiment::store( void )
{
    MySqlDB& mySql = MySqlDB::instance();
    stringstream sql; 

    if( id == 0 )
    {
        sql << "INSERT INTO experiment " 
        << " ( evolve_id, fitness_func_id, morph_id, physics_id, "
        << "   initial_state_id, name, description, physics_updates_per_neural_updates, "
        << "   remaining_trials, target_trials, priority, min_version ) VALUES ( "
        << evolveId << ", " 
        << fitnessFuncId << ", " 
        << morphId << ", " 
        << physicsId << ", " 
        << initialStateId << ", " 
        << mySql.escape( name ) << ", "
        << mySql.escape( desc ) << ", "
        << physics_updates_per_neural_updates << ", " 
        << trials << ", " // remaining_trials
        << trials << ", " // target_trials
        << priority << ", " 
        << min_version << " )";
        mySql.query( sql.str() );
        id = mySql.lastId();
    }
    else
    {
        sql << "UPDATE experiment SET " 
        << " evolve_id = " << evolveId << ", " 
        << " fitness_func_id = " << fitnessFuncId << ", " 
        << " morph_id = " << morphId << ", " 
        << " physics_is = " << physicsId << ", " 
        << " initial_state_id = " << initialStateId << ", " 
        << " name = " << mySql.escape( name ) << ", "
        << " description = " << mySql.escape( desc ) << ", "
        << " physics_updates_per_neural_updates = " << physics_updates_per_neural_updates << ", " 
        << " remaining_trials = " << trials << ", "
        << " target_trials = " << trials << ", "
        << " priority = " << priority << ", " 
        << " min_version = " << min_version 
        << " WHERE id = " << id ;
        mySql.query( sql.str() );        
    }
}

