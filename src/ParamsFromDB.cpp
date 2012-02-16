/*
 *  ParamsFromDB.cpp
 *  else
 *
 *  Created by vector on 7/31/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */

#include <ParamsFromDB.hpp>

#define ELSE_CONSOLE_BUILD

#include <Else.hpp>
#include <neat/neat.h>
#include <Tardis.hpp>
#include <HumanoidArticulatedBody.hpp>
#include <AcrobotArticulatedBody.hpp>
#include <PhysicsSimulator.hpp>
#include <FitnessFuncFactory.hpp>
#include <PopulationFitness.hpp>
#include <Evolver.hpp>
#include <NeatEvolver.hpp>
#include <Renderer.hpp>
#include <RibRenderer.hpp>
#include <ControllerFactory.hpp>
#include <MySqlDB.hpp>
#include <MySqlDBEvoReporter.hpp>
#include <PhysicsSimulatorFactory.hpp>
#include <ArticulatedBodyFactory.hpp>

#define FROMDB( field, type )  (neat->field = param[#field].as<type>())


void loadNEATParamsFromDB( MySqlDB& mysql, int experId, NEAT* neat )
{
    using namespace std;
    using namespace Else;
    
    Rows rows;
    std::stringstream sql;
    sql << "SELECT evolve.* FROM evolve, experiment WHERE experiment.id = " << experId 
    << " AND experiment.evolve_id = evolve.id";
    mysql.query( sql.str(), rows );
    if( rows.size() < 1 )
    {
        std::cerr << "Error loading NEAT params: " << "\n";
        return;
    }
    Row param = rows.back();
    //neat->weight_mut_power = param["weight_mut_power"].as<float>();
    FROMDB( weight_mut_power, float );
    FROMDB( recur_prob, float );
    FROMDB( mutdiff_coeff, float );
    FROMDB( compat_threshold, float );
    FROMDB( num_species_target, int );
    FROMDB( initial_link_prob, float );
    FROMDB( max_initial_node_count, int );
    FROMDB( new_genes_severe_mutate, double );
    FROMDB( age_significance, float );
    FROMDB( survival_thresh, float );
    FROMDB( mutate_only_prob, float );
    FROMDB( mutate_link_weights_prob, float );
    FROMDB( mutate_toggle_enable_prob, float );
    FROMDB( mutate_gene_reenable_prob, float );
    FROMDB( mutate_add_node_prob, float );
    FROMDB( mutate_add_link_prob, float );
    FROMDB( interspecies_mate_rate, float );
    FROMDB( mate_multipoint_prob, float );
    FROMDB( mate_multipoint_avg_prob, float );
    FROMDB( mate_singlepoint_prob, float );
    FROMDB( mate_only_prob, float );
    FROMDB( recur_only_prob, float );
    FROMDB( pop_size, int );
    FROMDB( dropoff_age, int );
    FROMDB( newlink_tries, int );
    FROMDB( babies_stolen, int );
    FROMDB( harness_generations, int );
    FROMDB( generation_limit, int );
}

void loadEvolverParamsFromDB( MySqlDB& mysql, int experId, Else::EvolverPtr evolver )
{
    using namespace std;
    using namespace Else;
    Rows rows;
    stringstream sql;
    sql << "SELECT e.physics_updates_per_neural_updates as neural_timesteps"
    << " FROM experiment as e "
    << " WHERE "
    << " e.id = " << experId;
    mysql.query( sql.str(), rows );
    Row& row = rows.back();
    evolver->setUpdateRate( row["neural_timesteps"].as<int>() );    
}

Else::PhysicsSimulatorFactoryPtr loadPhysicsParamsFromDB( MySqlDB& mysql, int experId )
{
    using namespace std;
    using namespace Else;
    Rows rows;
    stringstream sql;
    sql << "SELECT p.timestep as timestep "
    << " FROM physics as p, experiment as e "
    << " WHERE "
    << " e.id = " << experId
    << " AND e.physics_id = p.id ";
    mysql.query( sql.str(), rows );
    if( rows.empty() )
    {
        std::cerr << "FAILURE.  No physics record for experiment id " << experId << "\n";
        assert( false );
    }
    Row& row = rows.back();
    return PhysicsSimulatorFactoryPtr(
            new PhysicsSimulatorFactory( row["timestep"].as<double>() )
            );
}


Else::ArticulatedBodyFactoryPtr loadBodyFromDB( MySqlDB& mysql, int morphId )
{
    using namespace std;
    using namespace Else;

    Rows rows;
    stringstream sql;
    sql << "SELECT morph.type, morph.name, morph.type_id FROM morph WHERE morph.id = " << morphId;
    mysql.query( sql.str(), rows );
    Row& morphRow = rows.back();
    std::cerr << "Loading morph type: " << morphRow["name"].as<string>() << '\n';
    // TODO -- make a functor for other morphologies
    if( morphRow["type"].as<string>() == "morph_biped" )
    {
        sql.str("");
        sql << "SELECT height, weight, hip_breadth, "
            << "       ankle_strength, foot_width_per_height, foot_length_per_height "
            << "FROM morph_biped WHERE id = " << morphRow["type_id"];
        rows.clear();
        mysql.query( sql.str(), rows );
        Row& bipedRow = rows.back();

        ArticulatedBodyFactoryPtr bodyFactory(
                new HumanoidArticulatedBodyFactory( bipedRow["height"].as<float>(),
                                                    bipedRow["weight"].as<float>(),
                                                    bipedRow["hip_breadth"].as<float>(),
                                                    bipedRow["ankle_strength"].as<float>(),
                                                    bipedRow["foot_length_per_height"].as<float>(),
                                                    bipedRow["foot_width_per_height"].as<float>()
                 )
               );
       return bodyFactory;
    }
    else if( morphRow["type"].as<string>() == "morph_acrobot" )
    {
        ArticulatedBodyFactoryPtr bodyFactory(
                new AcrobotArticulatedBodyFactory( 1.0 ) );
        return bodyFactory;
    }
    else
    {
        fprintf( stderr, "FATAL ERROR: Failed to load morphology.\n" );
    }

    fprintf( stderr, "Done loading morphology.\n" );
}


Else::ArticulatedBodyFactoryPtr loadMorphForExperimentFromDB( MySqlDB& mysql, int experId )
{
    using namespace std;
    using namespace Else;

    Rows rows;
    stringstream sql;
    sql << "SELECT experiment.morph_id as mid FROM experiment WHERE experiment.id = " << experId;
    mysql.query( sql.str(), rows );
    Row& morphRow = rows.back();
    return loadBodyFromDB( mysql, morphRow["mid"].as<int>() );
}

bool loadInitialStateFromDB( MySqlDB& mysql, int experId, std::vector< Else::BodyState >& initState )
{
    using namespace std;
    using namespace Else;
    Rows rows;
    stringstream sql;
    sql << "SELECT body_states FROM state as bs, experiment as e WHERE e.initial_state_id = bs.id AND e.id = " << experId;
    mysql.query( sql.str(), rows );
    if( rows.empty() )
    {
        initState.clear();
        return false;
    }
    Row& statesRow = rows.back();
    
    stringstream ss;
    string s = statesRow["body_states"].text();
    ss.str( s );
    ss >> initState;

    fprintf( stderr, "Done loading initial state (with %d elements)\n", initState.size() );
    return true;
}


Else::PopulationFitnessPtr loadFitnessFuncFromDB( MySqlDB& mysql, int experId )
{
    using namespace std;
    using namespace Else;
    string fitFuncName;
    int fitFuncId;
    {
        Rows rows;
        stringstream sql;
        sql << "SELECT fitness_func.function as funcName, experiment.fitness_func_id as ffId FROM fitness_func, experiment "
        << " WHERE experiment.id = " << experId
        << " AND experiment.fitness_func_id = fitness_func.id ";
        mysql.query( sql.str(), rows );
        fitFuncName = rows.back()["funcName"].as<string>();
        fitFuncId = rows.back()["ffId"].as<int>();
    }
    
    Else::PopulationFitnessPtr fitness( new PopulationFitness( fitFuncName ) );
    
    {
        Rows rows;
        stringstream sql;
        sql << "SELECT name, value FROM fitness_func_param WHERE fitness_func_id = " << fitFuncId;
        mysql.query( sql.str(), rows );
        for( Rows::const_iterator ri = rows.begin(); ri != rows.end(); ++ri )
        {
            const Row& row = *ri;
            fitness->setDoubleParam( row["name"].as<string>(), row["value"].as<double>() );
        }
    }
    return fitness;
}

Else::ControllerFactoryPtr loadControllerFactoryFromDB( MySqlDB& mysql, int experId )
{
    using namespace std;
    using namespace Else;
    std::string name;
    int sym = 1;
    int ins = 0;
    int outs = 0;
    int sharedIns = 0;
    int sharedOuts = 0;
    int crossovers = 0;
    int biases = 0;
    bool recurrent = true;
    int hidden = 0;
    {
        Rows rows;
        stringstream sql;
        sql << "SELECT "
            << " controller.name as c_name, "
            << " controller.symmetry as c_sym, "
            << " controller.module_inputs as c_ins, "
            << " controller.module_outputs as c_outs, "
            << " controller.shared_inputs as c_sharedIns, "
            << " controller.shared_outputs as c_sharedOuts, "
            << " controller.crossovers as c_crossovers, "
            << " controller.biases as c_biases, "
            << " controller.max_hiddens as c_hidden, "
            << " controller.recurrent as c_recurrent "
            << " FROM controller, experiment "
            << " WHERE controller.id = experiment.controller_id and experiment.id = " << experId;
        //std::cerr << "SQL: " << sql.str();
        mysql.query( sql.str(), rows );
        name = rows.back()["c_name"].as<string>();
        sym = rows.back()["c_sym"].as<int>();
        ins = rows.back()["c_ins"].as<int>();
        outs = rows.back()["c_outs"].as<int>();
        sharedIns = rows.back()["c_sharedIns"].as<int>();
        sharedOuts = rows.back()["c_sharedOuts"].as<int>();
        crossovers = rows.back()["c_crossovers"].as<int>();
        biases = rows.back()["c_biases"].as<int>();
        hidden = rows.back()["c_hidden"].as<int>();
        recurrent = rows.back()["c_recurrent"].as<bool>();
        std::cerr << "Loaded controller: " << name << '\n';
    }

    ControllerFactoryPtr controllerFactory( new ControllerFactory( name,
                                                                   ins,
                                                                   outs,
                                                                   sym,
                                                                   sharedIns,
                                                                   sharedOuts,
                                                                   crossovers,
                                                                   biases,
                                                                   recurrent,
                                                                   hidden )
                                            );
    return controllerFactory;
}

void loadPerformance( MySqlDB& mysql,
                      long int argPerformanceId,
                      std::vector< std::vector< Else::BodyState > >& argStates )
{
    using namespace std;
    using namespace Else;
    Rows rows;
    stringstream sql;
    sql << "SELECT body_states FROM state as bs WHERE bs.performance_id = " << argPerformanceId;
    mysql.query( sql.str(), rows );
    if( rows.empty() )
    {
        fprintf( stderr,
                 "FATAL ERROR: Failed to load body states for performance %d.\n",
                 argPerformanceId );
    }
    std::vector< Else::BodyState > state;
    for( Rows::const_iterator ri = rows.begin(); ri != rows.end(); ++ri )
    {
        state.clear();
        const Row& row = *ri;
        stringstream ss;
        string s = row["body_states"].text();
        ss.str( s );
        ss >> state;
        argStates.push_back( state );
    }
    fprintf( stderr, "Done loading %d states.\n", argStates.size() );
}

