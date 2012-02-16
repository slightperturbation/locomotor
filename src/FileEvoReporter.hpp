#ifndef _ELSE_FILEEVOREPORTER_H
#define _ELSE_FILEEVOREPORTER_H
/*
 *  FileEvoReporter.h
 *  else
 *
 *  Created by vector on 7/21/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */

#include <EvoReporter.hpp>
#include <fstream>

namespace Else
{
    class FileEvoReporter : public EvoReporter
        {
        public:
            FileEvoReporter( const std::string& filename );
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
                                     int generation,
                                     float fitness,
                                     const std::string& genomeString, 
                                     const std::string& genomeXML,
                                     const std::string& stateAnim );
            virtual void endRun( void )
            {
                
            }
        private:
            std::string _filename;
            std::ofstream _file;            
        };
}

#endif
