#ifndef DB_PERFORMANCE_HPP
#define DB_PERFORMANCE_HPP

/*
 *  DbPerformance.h
 *  else
 *
 *  Created by vector on 8/6/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */
//#include "Performance.h"

#include <vector>
#include <BodyState.hpp>

class MySqlDB;

namespace Else
{
    
    /// Container class for Performance data (TODO- load neural traces).
    class DbPerformance 
    {            
    public:
        DbPerformance( void );
        virtual ~DbPerformance() 
        {
            // noop
        }
        virtual int load( MySqlDB& argMySql, long int& argPerformanceId );
        virtual std::vector< std::vector< Else::BodyState > >& getStates( void );
        virtual double getTimeStep( void );
        char* getLastErr( void );

    private:
        long int queryBestPerformance( MySqlDB& mysql );
        std::vector< std::vector< Else::BodyState > > _states;
        double _timeStep;
        char _lastErr[1024];
    };
}


#endif
