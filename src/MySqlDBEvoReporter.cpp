/*
 *  MySqlDBEvoReporter.cpp
 *  else
 *
 *  Created by vector on 7/21/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */

#include <MySqlDBEvoReporter.hpp>
#include <MySqlDB.hpp>

using namespace Else;
using namespace std;


MySqlDBEvoReporter::MySqlDBEvoReporter(MySqlDB& mysql, 
                                       long int experId,
                                       const std::string& argLabel )
:   _mySql( mysql ),
    _currTrialId( 0 ),
    _experId( experId ),
    _lastGenId( 0 ),
    _lastGen( 0 ),
    _label( argLabel )
{
    // Noop
}

void 
MySqlDBEvoReporter
::startRun( int runId )
{
    if( !(_experId > 0) )
    {
        std::cerr << "\nMySQLDBEvoReporter error -- exper param id not set before startRun.  NO LOGGING AVAILABLE!\n";
        return;
    }
    std::stringstream sql;
    sql << "INSERT INTO trial ( experiment_id, label ) VALUES ( " << _experId << ", " << _mySql.escape( _label ) <<" )";
    _mySql.query( sql.str() );
    _currTrialId = _mySql.lastId();
    
    sql.str("");
    sql << "SELECT timestep FROM physics, experiment WHERE experiment.id = "
    << _experId
    << " AND physics.id = experiment.physics_id ";
    {
        Rows rows;
        _mySql.query( sql.str(), rows );
        Row row = rows.back();
        _timestep = row["timestep"].as<double>();
    }
    
    sql.str("");
    sql << "SELECT physics.timestep as physics_timesteps, experiment.physics_updates_per_neural_updates as neural_updates "
    << " FROM physics, experiment WHERE experiment.id = "
    << _experId
    << " AND physics.id = experiment.physics_id ";
    {
        Rows rows;
        _mySql.query( sql.str(), rows );
        Row row = rows.back();
        _timestep = row["physics_timesteps"].as<double>();
        _physicsUpdatesPerNeural = row["neural_updates"].as<int>();
    }
}

void 
MySqlDBEvoReporter
::generation(int runNumber, 
             int generationNumber, 
             float bestFitness,
             float bestRawFitness,
             int bestNodeCount,
             int bestLinkCount,
             float meanFitness,
             float sdFitness,
             int maxNodeCount,
             int maxLinkCount )
{
    std::stringstream sql;
    sql << "INSERT INTO generation ( "
    << "trial_id, generation, best_fitness, best_raw_fitness, best_node_count, best_link_count, "
    << "mean_fitness, sd_fitness, max_node_count, max_link_count ) VALUES ( "
    << _currTrialId << ", "
    << generationNumber << ", "
    << bestFitness << ", "
    << bestRawFitness << ", "
    << bestNodeCount << ", "
    << bestLinkCount << ", "
    << meanFitness << ", "
    << sdFitness << ", "
    << maxNodeCount << ", "
    << maxLinkCount << " )";
    _mySql.query( sql.str() );
    _lastGenId = _mySql.lastId();
    _lastGen = generationNumber;
}

void 
MySqlDBEvoReporter
::newChampion(int runNumber, 
              int gen,
              float rawFitness,
              const std::string& genomeString, 
              const std::string& genomeXML,
              const std::vector< std::string >& stateAnim )
{
    //assert( gen == _lastGen );
    std::stringstream sql;
    sql << "INSERT INTO performance ( timestep, physics_updates_per_neural_updates ) VALUES ( "
    << _timestep << ", "
    << _physicsUpdatesPerNeural 
    << " )";
    _mySql.query( sql.str() );
    long int lastPerformanceId = _mySql.lastId();
    sql.str("");
    for( size_t i = 0; i < stateAnim.size(); ++i )
    {
        sql.str("");
        sql << "INSERT INTO state ( performance_id, frame, body_states ) VALUES ( "
        << lastPerformanceId << ", "
        << i << ", "
        << _mySql.escape( stateAnim[i] ) 
        << " )";
        _mySql.query( sql.str() );
    }
    sql.str("");
    sql << "INSERT INTO genome ( genome_string, genome_xml ) VALUES ( "
    << _mySql.escape( genomeString ) << ", "
    << _mySql.escape( genomeXML ) << ")";
    _mySql.query( sql.str() );
    long int lastGenomeId = _mySql.lastId();
    
    sql.str("");
    sql << "INSERT INTO champion ( generation_id, genome_id, performance_id, raw_fitness ) VALUES ( "
    << _lastGenId << ", "
    << lastGenomeId << ", "
    << lastPerformanceId << ", "
    << rawFitness << " )";
    _mySql.query( sql.str() );
}

void
MySqlDBEvoReporter
::endRun( void )
{
    {
        std::stringstream sql;
        sql << "UPDATE trial SET finish_time = NOW() WHERE id = " << _currTrialId;
        _mySql.query( sql.str() );
    }
    {
        std::stringstream sql;
        sql << "UPDATE experiment SET completed_trials = completed_trials + 1 WHERE id = " << _experId;
        _mySql.query( sql.str() );
    }
}
