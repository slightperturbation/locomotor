/*
 *  DBExperiment.h
 *  Wraps the Experiment table, esp for loading and copying
 *
 *  Created by vector on 9/24/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */
#include <string>

typedef long int dbid;

class DBExperiment
{
public:
    
    /// Load from the database
    DBExperiment( dbid argExperimentId );
    
    /// Copy the source experiment, but generating unique ID.
    DBExperiment( const DBExperiment& src );
    
    /// Write this exper to DB.  If not in DB already, insert; otherwise, update.
    void store( void );
    
    dbid id;
    dbid evolveId;
    dbid fitnessFuncId;
    dbid morphId;
    dbid physicsId;
    dbid initialStateId;
    std::string name;
    std::string desc;
    int physics_updates_per_neural_updates;
    int trials;
    double priority;
    double min_version;
};