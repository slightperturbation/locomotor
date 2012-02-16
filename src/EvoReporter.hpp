#ifndef _ELSE_EVOREPORTER_H_
#define _ELSE_EVOREPORTER_H_

/*
 *  EvoReporter.h
 *  else
 *
 *  Created by vector on 7/21/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */
#include <string>
#include <vector>


namespace Else
{
    /// Abstract class for recording/reporting progress of an evolutionary run.
    class EvoReporter
    {
    public:
        virtual ~EvoReporter() { }
        virtual void startRun( int runId ) = 0;
        virtual void generation(int runNumber, 
                                int generationNumber, 
                                float bestFitness,
                                float bestRawFitness,
                                int bestNodeCount,
                                int bestLinkCount,
                                float meanFitness,
                                float sdFitness,
                                int maxNodeCount,
                                int maxLinkCount ) = 0;
        virtual void newChampion(int runNumber, 
                                 int generation,
                                 float fitness,
                                 const std::string& genomeString, 
                                 const std::string& genomeXML,
                                 const std::vector< std::string >& stateAnim ) = 0;   
        virtual void endRun( void ) = 0;
    };
}
#endif
