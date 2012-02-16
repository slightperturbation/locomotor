#ifndef _ELSE_PARAMSFROM_DB_H
#define _ELSE_PARAMSFROM_DB_H

/*
 *  ParamsFromDB.h
 *  else
 *
 *  Created by vector on 7/31/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */
// Internal Includes
#include <Else.hpp>
#include <MySqlDB.hpp>
#include <BodyState.hpp>

#include <string>
#include <sstream>

class NEAT;

void loadNEATParamsFromDB( MySqlDB& mysql, int experId, NEAT* neat );
void loadEvolverParamsFromDB( MySqlDB& mysql, int experId, Else::EvolverPtr evolver );
Else::PhysicsSimulatorFactoryPtr loadPhysicsParamsFromDB( MySqlDB& mysql, int experId );

Else::ArticulatedBodyFactoryPtr loadMorphForExperimentFromDB( MySqlDB& mysql, int experId );
Else::ArticulatedBodyFactoryPtr loadBodyFromDB( MySqlDB& mysql, int morphId );

bool loadInitialStateFromDB( MySqlDB& mysql, int experId, std::vector< Else::BodyState >& initState );
Else::PopulationFitnessPtr loadFitnessFuncFromDB( MySqlDB& mysql, int experId );
Else::ControllerFactoryPtr loadControllerFactoryFromDB( MySqlDB& mysql, int experId );

void loadPerformance( MySqlDB& mysql,
                      long int argPerformanceId,
                      std::vector< std::vector< Else::BodyState > >& argStates );

#endif
