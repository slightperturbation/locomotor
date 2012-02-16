#ifndef _ELSE_MYSQLDBEVOREPORTER_HPP_
#define _ELSE_MYSQLDBEVOREPORTER_HPP_

/*
 *  MySqlDBEvoReporter.hpp
 *  else
 *
 *  Created by vector on 7/21/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */

#include <EvoReporter.hpp>
#include <string>

class MySqlDB;

namespace Else
{
    class MySqlDBEvoReporter : public EvoReporter
    {
    public:
        MySqlDBEvoReporter( MySqlDB& mysql, long int experId, const std::string& argLabel );
        
        virtual void startRun( int runId );
        
        virtual void generation(int runNumber, 
                                int generationNumber, 
                                float bestFitness,
                                float bestRawFitness,
                                int bestNodeCount,
                                int bestLinkCount,
                                float meanFitness,
                                float sdFitness,
                                int maxNodeCount,
                                int maxLinkCount );
        
        virtual void newChampion(int runNumber, 
                                 int gen,
                                 float fitness,
                                 const std::string& genomeString, 
                                 const std::string& genomeXML,
                                 const std::vector< std::string >& stateAnim );
        
        virtual void endRun( void );
    private:
        MySqlDB& _mySql;

        long int _currTrialId;
        long int _experId;
        long int _lastGenId;
        int _lastGen;
        std::string _label;
        double _timestep;
        double _physicsUpdatesPerNeural;
    };
}

#endif
