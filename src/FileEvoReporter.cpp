/*
 *  FileEvoReporter.cpp
 *  else
 *
 *  Created by vector on 7/21/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */

#include <FileEvoReporter.hpp>
#include <sstream>
#include <fstream>

using namespace Else;
using namespace std;

FileEvoReporter::FileEvoReporter( const std::string& filename )
: _filename( filename )
{
}

void FileEvoReporter::startRun( int runId )
{
    std::stringstream filename;
    filename << "Fitness_" << _filename << "_run_" << runId << ".txt";
    _file.open( filename.str().c_str(), ios::app );
    _file 
    << "#RunNumber Gen BestOrgFit "
    << "BestOrgsNodeCount BestOrgsLinkCount MeanFitness PopSize LargestLinkCount LargestNodeCount GenerationTime TotTime\n";
    
    
}

void FileEvoReporter::generation(int runNumber, 
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
    _file << runNumber 
    << '\t' << generationNumber
    << '\t' << bestFitness
    << '\t' << bestRawFitness
    << '\t' << bestNodeCount
    << '\t' << bestLinkCount
    << '\t' << meanFitness
    << '\t' << sdFitness
    << '\t' << maxNodeCount
    << '\t' << maxLinkCount;
}

void FileEvoReporter
::newChampion( int runNumber, 
               int gen,
               float fitness,
               const std::string& genomeString, 
               const std::string& genomeXML,
              const std::vector< std::string >& stateAnimStrings )
{
    
    // GENOME
    ostringstream genomeFilename; 	 
    genomeFilename <<  "best_organism_of_run_" << runNumber 	 
    << "_in_generation_" << gen << ".genome"; 	
    ofstream genomeFile( genomeFilename.str().c_str() );
    genomeFile << genomeString;
    genomeFile.close();
    
    // GENOME XML
    ostringstream filenameXML;
    filenameXML <<  "best_organism_of_run_" << runNumber 	 
    << "_in_generation_" << gen << ".xml";
    std::ofstream networkXML;
    networkXML.open( filenameXML.str().c_str() , ios::trunc );
    networkXML << genomeXML;
    networkXML.close();    

    // STATE ANIM
    std::stringstream animName;
    animName << "bestByGen_" << gen << ".state";
    ofstream stateAnim( animName.str().c_str() );
    for( size_t i = 0; i < stateAnimStrings.size(); ++i )
    {
        stateAnim << stateAnimString << "\n";
    }
    stateAnim.close();    
    
}
