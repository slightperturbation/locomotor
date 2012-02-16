/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

// Copyright 2009, Brian F. Allen, 
//
// Contact: vector@acm.org
//

// Project Includes
#include <NeatEvolver.hpp>
#include <Tardis.hpp>
#include <Utils.hpp>
#include <EvoReporter.hpp>
#include <PopulationFitness.hpp>
#include <ControllerFactory.hpp>
#include <EvaluationFactory.hpp>

#include <neat/genetics.h>

// External Includes
#include <sstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

#define INITIAL_LINK_PROB (0.25)
#define SAVE_POP_EVERY_GENERATIONS (200)
#define SAVE_BEST_EVERY_GENERATIONS (50)

//////////////////////////////////////////////////////////////////////////
// CTOR
Else::NeatEvolver
:: NeatEvolver( EvaluationFactoryPtr argEvaluationFactory,
                ControllerFactoryPtr argControllerFactoryPtr,
                PopulationFitnessPtr argPopulationFitnessPtr,
                NEAT* argConfig )
:   Evolver( argEvaluationFactory ),
    myPop(              0 ),
    myPopFitness( argPopulationFitnessPtr ),
    myStepsPerEval(     DEFAULT_STEPS_PER_EVAL ),
    myNeatConfig(		argConfig ),
    myControllerFactory( argControllerFactoryPtr )
{
    //std::cout << "NEW NEAT EVOLVER\n";
}

//////////////////////////////////////////////////////////////////////////
// DTOR
Else::NeatEvolver
:: ~NeatEvolver()
{
    delete myPop; myPop = 0;
}

//////////////////////////////////////////////////////////////////////////
// go()
void 
Else::NeatEvolver
:: go( EvoReporter* reporter )
{
    // flag that were in a run
    myIsRunning = true;

    initPop( myNeatConfig->pop_size );

    singleRun( reporter );
    myIsRunning = false;
}


////////////////////////////////////////////////////////////////////////////
//// runGenome()
//void
//Else::NeatEvolver
//::runIndividualGenome( Genome* genome, FitnessFuncPtr argFitnessFunc, PhysicsSimulatorPtr argSim, bool argUseHarness )
//{
//    assert( v );

//    Evaluation* eval = myEvaluationFactory->createEvaluation( genome, 0 );


//#ifdef ENABLE_TIMERS
//    gblEmbryogenesisTimer->start();
//#endif

//    ControllerPtr controller
//            = ControllerPtr( new Controller( *genome ) );

//#ifdef ENABLE_TIMERS
//    gblEmbryogenesisTimer->stop();
//#endif
//    assert( argFitnessFunc );
//    assert( argSim );

//    Evaluation eval ( myStepsPerEval,
//                      controller,
//                      argFitnessFunc,
//                      argSim,
//                      myInitialState,
//                      myDisplayRate,
//                      myUpdateRate,
//                      myNeatConfig->harness_generations );
    
//    // TODO clean up harness
//    int startingGen = 0;
//    if( !argUseHarness ) startingGen = 1000;
//    FitnessFuncPtr fitness = eval( startingGen, 500 );

//    /// Dump activations
//#ifdef ENABLE_TIMERS
//    gblIOTimer->start();
//#endif
//    std::vector< double > activations;
//    //int networkStride;
//    //double time = 0;
//    //double deltaT = myUpdateRate * argSim->getTimeStep();
//    /* controller->acquireActivationHistory( activations, networkStride );  // only gets right side activations
//	std::ofstream tmpActivations( "activationHistory.dat" );
//	for( int i = 0; i < activations.size(); ++i )
//	{
//		if( !(i%networkStride) )
//		{
//			tmpActivations << std::endl << time << '\t';
//			time += deltaT;
//		}
//		tmpActivations << activations[i] << '\t';
//	}
//	tmpActivations.close();
//    */
//#ifdef ENABLE_TIMERS
//	gblIOTimer->stop();
//#endif
//}

//////////////////////////////////////////////////////////////////////////
// initPop()
// PRE: myStartingGenome must be a valid geonme (with appropriate input and output counts)
void
Else::NeatEvolver
::initPop( size_t argPopSize )
{
    std::cout << "Spawning population of size " << myNeatConfig->pop_size << "\n";
    //assert( myStartingGenome && ( myStartingGenome->genes.size() > 0 ) );
    //assert( myStartingGenome && myStartingGenome->genesis(0)->outputs.size() == mySim->getBody()->getActuatorCount() );

    delete myPop;

    myPop = new Population( argPopSize,
                            myControllerFactory->getNetworkInputCount(),
                            myControllerFactory->getNetworkOutputCount(),
                            myControllerFactory->getMaxHiddenNodeCount(),
                            myControllerFactory->getIsRecurrencyAllowed(),
                            myNeatConfig->initial_link_prob //myInitLinkProb // init link prob
                            );
    verifyPopulation();
}


//////////////////////////////////////////////////////////////////////////
/// singleRun does one evolutionary run of argGenerations generations.
/// note not population size has already been specified to Evolver.
/// This is the *key* function -- runs the generations.  
void 
Else::NeatEvolver
::singleRun( EvoReporter* reporter )
{
    using namespace std;
    reporter->startRun( 1 );

    Tardis timer, runTimer;
    Time physicsElapsedTime;
    Time otherElapsedTime;
    
    // Allocate one giant chunk of memory for activation history.
    // This block gets over-written with each generation

//    const size_t numThreads =
//#ifdef USE_OPENMP
//    omp_get_max_threads();
//#else
//    1;
//#endif

    //int runHarnessGens = myNeatConfig->harness_generations; //myHarnessGens;
	runTimer.setTime( 0 );
    
    stringstream fitlog;
    
    /////////////////////////////////////////////////////////////////
    /// EACH GENERATION
    myBestFitness = -1000.0;
    const float myLogMinImprovement = 0.0;
    const int myLogMinGenerations = 50;
    int myLastLogGeneration = -myLogMinGenerations;
    
    for( myCurrGen = 0; myCurrGen <  myNeatConfig->generation_limit; ++myCurrGen )
    {
        timer.setTime( 0 );
        if( !isRunning() ) 
        {
            std::cerr << "Aborting the run due to not \"isRunning\".\n";
            break;
        }
        // One "Epoch"
        Organism* bestOrg = 0;

        // Evaluate Fitness -- this is the big function!
        bestOrg = evaluatePopulation( myCurrGen, myNeatConfig->generation_limit, reporter );//, runHarnessGens );
        
        assert( bestOrg );
        std::auto_ptr<Network> bestnet = std::auto_ptr<Network>( bestOrg->gnome->genesis( bestOrg->gnome->genome_id ) );
        assert( bestnet.get() );
        int bestLinkCount = bestnet->linkcount();
        int bestNodeCount = bestnet->nodecount();

        physicsElapsedTime = timer.update();

        int largestLinkCount = 0;
        int largestNodeCount = 0;
        //double meanFitness = 0.0;
        //double sdFitness = 0.0;
        
        // This loop is primarily doing the link counts
        // This is actually really expensive for big networks and could (should?) be removed for performance
        
        
        /// OPTIMIZATION - 09 AUG 31 -- Removed tracking of aggregate link/node count stats.
//        for( std::list< Organism* >::iterator i = myPop->organisms.begin(); i != myPop->organisms.end(); ++i )
//        {
//            Organism* org = *i;
//            assert( org );
//            std::auto_ptr<Network> net = std::auto_ptr<Network>( org->gnome->genesis( org->gnome->genome_id ) );
//            assert( net.get() );
//            if( net->linkcount() > largestLinkCount )
//            {
//                largestLinkCount = net->linkcount();
//            }
//
//            if( net->nodecount() > largestNodeCount )
//            {
//                largestNodeCount = net->nodecount();
//            }
//            //meanFitness += org->fitness;
//        }
        /// END OPTIMIZATION - 09 AUG 31
        
        
        // Computing the mean & sd moved
        //double N = (double) myPop->organisms.size();
        //meanFitness /= N;

        //double totError2 = 0.0;
        //for( std::list< Organism* >::iterator i = myPop->organisms.begin(); i != myPop->organisms.end(); ++i )
        //{
        //    Organism* org = *i;
            //double err = (org->fitness - meanFitness);
            //totError2 += err*err;
        //}
        //sdFitness = sqrt( totError2 / N );  // not a sample, so N, not N-1
        
        runTimer.update();
        timer.update();
        
        reporter->generation( 1, 
                              myCurrGen, 
                              (*myPopFitness)[bestOrg->index]->finalRawFitness(),
                              myBestFitnessCurrGen,
                              //bestOrg->orig_fitness,
                              bestNodeCount,
                              bestLinkCount,
                              myMeanFitnessCurrGen,
                              mySDFitnessCurrGen,
                              largestNodeCount,
                              largestLinkCount );
                              
        /*
        runLog << 1 << '\t' << myCurrGen << '\t'
            << bestOrg->fitness << '\t'
            << bestNodeCount << '\t'
            << bestLinkCount << '\t'
            << meanFitness << '\t'
            << bestOrg->pop_size << '\t'
            << largestLinkCount << '\t'
            << largestNodeCount << '\t'
            << timer.getTime() << '\t'
            << runTimer.getTime()
            << std::endl;
        */
        // New best!
        if( bestOrg 
            && ( 
                   ( (*myPopFitness)[bestOrg->index]->finalRawFitness() > ( myBestFitness + myLogMinImprovement ) ) 
                || ( myCurrGen > ( myLastLogGeneration + myLogMinGenerations ) )
                )
           )
        {
            myLastLogGeneration = myCurrGen;
            Genome* g = bestOrg->gnome; 	 
            ostringstream genomeStream;
            g->print_to_stream( genomeStream );
              
            myBestFitness = (*myPopFitness)[bestOrg->index]->finalRawFitness();
            myBestAnim = myBestAnimOfGen;
            
            std::vector< std::string > animStates;
            for( size_t i = 0; i < myBestAnim.size(); ++i )
            {
                ostringstream animStream;
                animStream << myBestAnim[i];
                animStates.push_back( animStream.str() );
            }
            
            ostringstream genomeXMLStream;
            addNetworkXML( *g, genomeXMLStream );
            
            reporter->newChampion( 1,
                                   myCurrGen, 
                                   (*myPopFitness)[bestOrg->index]->finalRawFitness(),
                                   genomeStream.str(),
                                   genomeXMLStream.str(),
                                   animStates ); 
        }
        /*
		if( myPop && (!(myCurrGen % SAVE_POP_EVERY_GENERATIONS )) )
		{
			ostringstream filename; 	 
			filename << DATA_DIR "pop_of_generation_" << myCurrGen << ".population"; 	 
			myPop->print_to_file( filename.str().c_str() ); 	 
		}
         */
        
        //verifyPopulation();
        //runLog.flush();
        myPop->epoch( myCurrGen );
        
        //runLog.flush();
        //verifyPopulation();
        otherElapsedTime = timer.update();
        
        //delete bestOrg;
        //std::cerr << "Generation Time: " << timer.getTime() << " = "
         //   << "Evaluation Time( " << physicsElapsedTime  << ") + "
         //   << "I/O Time ( " << otherElapsedTime  << ")\n";
    }
    reporter->endRun();
}

//////////////////////////////////////////////////////////////////////////
// verifyPopulation()
void
Else::NeatEvolver
::verifyPopulation( void )
{
    assert( myPop );
    std::list< Organism* >::iterator currOrg = (myPop->organisms).begin();
    std::list< Organism* >::iterator endOrg = (myPop->organisms).end();
    /*
    int count = 0;
    while( currOrg != endOrg )
    {
        if( (*currOrg)->net->outputs.size() != mySim->getBody()->getOutputCountPerNetwork() )
        {
            std::cerr << "Critical Failure:  Network output count for org #"
                << count << " is "
                <<  (*currOrg)->net->outputs.size()
                << " but expected " 
                << mySim->getBody()->getActuatorCount()
                << "\n";
        }

        if( (*currOrg)->net->inputs.size() != ( mySim->getBody()->getInputCountPerNetwork() + 1 ) )
        {
            std::cerr << "Critical Failure:  Network input count for org #"
                << count << " is "
                <<  (*currOrg)->net->inputs.size()
                << " but expected " 
                << mySim->getBody()->getSensorCount() + 1
                << "\n";
        }
        assert( (*currOrg)->net->outputs.size() == mySim->getBody()->getOutputCountPerNetwork() ); 
        assert( (*currOrg)->net->inputs.size() == mySim->getBody()->getInputCountPerNetwork() + 1 ); // +1 for bias
        ++currOrg; ++count;
    }
    */
    //assert( myPop->verify() );

    std::cerr << "VerifyPopulation OK.  size=" 
        << myPop->organisms.size() << ", "
        << myPop->pop_size << ".\n";
}

std::ostream& dataToCSV( std::ostream& out, const std::vector< std::vector< double > >& data )
{
    for( std::vector< std::vector< double > >::const_iterator i = data.begin(); i != data.end(); ++i )
    {
        std::copy( i->begin(), i->end(), std::ostream_iterator<double>( out, ", " ) );
        out << std::endl;
    }
    return out;
}

void dataToCSVFile( const std::string& name, int gen, const std::vector< std::vector< double > >& data )
{
    std::stringstream filename;
    filename << name << "_" << gen << ".csv";
    std::ofstream dataFile( filename.str().c_str() );
    dataToCSV( dataFile, data );        
}

//////////////////////////////////////////////////////////////////////////
// evaluateFitness()
/// Evaluates the fitness of every organism in the current population (for 
/// a single generation).
/// Returns the best organism, and outAvgFitness is the average raw fitness over the whole pop.
Organism* 
Else::NeatEvolver
::evaluatePopulation( int argGen, int argGenTotal, EvoReporter* /*reporter*/ )
{
    //std::cerr << "NeatEvolver::evaluateFitness( " << argGen << ", " << argGenTotal << ")\n";
    
    double totFitness = 0.0;
    double totSqFitness = 0.0;
        
    Organism* lclBestOrg = (myPop->organisms).back();

    const size_t popSize = myPop->organisms.size();

    // keep stringstreams around for the best performance for this generation
    // an evaluated organism with best fitness so far will overwrite the stringstreams of performance data
    
    std::ostringstream bestInGenRIB;
    std::ostringstream bestInGenActivations;
    std::ostringstream bestInGenJointAngles;
    
    myBestAnimOfGen.clear();    
    
    //std::cerr << "NeatEvolver::evaluateFitness -- Building per-organism vectors... ";

    // Build parallel vectors to all easy openmp parallelization
    std::vector< Organism* > organisms;
    //std::vector< ControllerPtr > controllers;
    //std::vector< FitnessFuncPtr > fitnessFuncs;
    
    std::vector< EvaluationPtr > evals( popSize );
    
    // delay the "reset" of the fitness functions
    // because we need to preserve the raw fitness
    // data to record it outside of this function.
    myPopFitness->endGeneration();
    myPopFitness->resetToPopSize( popSize );

    {
        // make an array of organisms for later use
        std::list< Organism* >::iterator currOrg = (myPop->organisms).begin();
        std::list< Organism* >::iterator endOrg = (myPop->organisms).end();
        size_t index = 0;
        while( currOrg != endOrg )
        {
            if( !(*currOrg) || !((*currOrg)->gnome) )
            {
                std::cerr << "Bad organism reference.\n";
                break;
            }
            // Make a NEAT controller based on the currOrg's network
            Organism* op = *currOrg;
            op->index = index++;
            organisms.push_back( op );
            currOrg++;
        }
    }
    
    //assert( controllers.size() == popSize );
    assert( organisms.size() == popSize );
    
//    int threadCount = 
//#ifdef USE_OPENMP
//    omp_get_max_threads();
//#else
//    1;
//#endif
    //
//    //std::cerr << "NeatEvolver::evaluateFitness -- Building per-thread vectors (" << numThreads << ")\n";
//    for( size_t i = 0; i < threadCount; ++i )
//    {
//        sims.push_back( mySim->deepCopy() ); 
//    }
//    std::cout << "Using " << threadCount << " threads, one physics simulator per thread.\n";
    
    size_t pid;
    // PARALLELIZE
#ifdef USE_OPENMP
#pragma omp parallel for private( pid )
#endif
    for( size_t orgIndex = 0; orgIndex < popSize; ++orgIndex )
    {
//        pid =
//#ifdef USE_OPENMP
//        omp_get_thread_num();
//#else
//        0;
//#endif


        EvaluationPtr eval
                = myEvaluationFactory->createEvaluation( orgIndex,
                                                         organisms[orgIndex]->gnome,
                                                         (*myPopFitness)[orgIndex],
                                                         argGen );

//        EvaluationPtr eval( new Evaluation(myStepsPerEval,
//                                           controllers[orgIndex],
//                                           (*myPopFitness)[orgIndex],
//                                           mySims[pid],
//                                           myInitialState,
//                                           myDisplayRate,
//                                           myUpdateRate,
//                                           myNeatConfig->harness_generations )
//                           );

        evals[orgIndex] = eval;
        (*eval)( argGen, argGenTotal );
        //std::stringstream out;
        //out << pid << ": " << orgIndex << " org has fitness = " << fitnessFuncs[orgIndex]->last() << "\n";
        //std::cerr << out.str();
    }
    // End OpenMP for block
    
    // Analyze results in main thread only
    {
        myBestFitnessCurrGen = -99999.9;  // fitness is guaranteed to be positive
        //myMeanFitnessCurrGen = 0.0;
        //mySDFitnessCurrGen = 0.0;
        for( size_t orgIndex = 0; orgIndex < popSize; ++orgIndex )
        {
            EvaluationPtr eval = evals[orgIndex];        
            FitnessFuncPtr fitness = (*myPopFitness)[orgIndex];
            // Use the "raw" (unadjusted by strategy) fitness for this
            if( myBestFitnessCurrGen < fitness->last() )
            {
                // save this as the best individual so far
                lclBestOrg = organisms[orgIndex];
                myBestFitnessCurrGen = fitness->last();
                
                myBestAnimOfGen = eval->getStateData();
                //myBestAnimOfGen.clear();
                //std::copy( eval.getStateData().begin(), eval.getStateData().end(), std::back_inserter( myBestAnimOfGen ) );
                
                mySensorData.clear();
                std::copy( eval->getSensorData().begin(), eval->getSensorData().end(), std::back_inserter( mySensorData ) );
                
                myActuatorData.clear();
                std::copy( eval->getActuatorData().begin(), eval->getActuatorData().end(), std::back_inserter( myActuatorData ) );
                
                //myNeuronActivationData.clear();
                //std::copy( eval->getNeuronActivationData().begin(), eval->getNeuronActivationData().end(), std::back_inserter( myNeuronActivationData ) );
                
                //myJointAngleData.clear();
                //std::copy( eval->getJointAngleData().begin(), eval->getJointAngleData().end(), std::back_inserter( myJointAngleData ) );
                
                myAnimations.clear();
//                BOOST_FOREACH( laetoli::OfflineRenderer* r, mySim->getRenderers() )
//                {
//                    std::stringstream tmp;
//                    r->write( tmp );
//                    myAnimations.push_back( tmp.str() );
//                }
            }
            double fitVal = fitness->last();
            totFitness += fitVal;
            totSqFitness += fitVal*fitVal;
        }        
    } 

    // Assign fitness to organism (can only do once all pop's fitnesses have been computed)
    for( size_t orgIndex = 0; orgIndex < popSize; ++orgIndex )
    {
        FitnessFuncPtr fitness = (*myPopFitness)[orgIndex];
        organisms[orgIndex]->fitness = fitness->finalFitness();
        assert( organisms[orgIndex]->index == orgIndex );
    }
    
    // write out best here
    
    /*
    dataToCSVFile( "NeuronActivation_", argGen, myNeuronActivationData );
    dataToCSVFile( "Sensor_", argGen, mySensorData );
    dataToCSVFile( "Actuator_", argGen, myActuatorData );
    dataToCSVFile( "JointAngles_", argGen, myJointAngleData );
    for( int i = 0; i < myAnimations.size(); ++i )
    {
        const std::string& anim = myAnimations[i];
        std::stringstream filename;
        filename << "BestAtGen_" << argGen << "_Renderer-" << i << "_anim.rib";
        std::ofstream animFile( filename.str().c_str() );
        animFile << anim;
    }
     */
    
    //std::cerr << "\n--------------------------------------------------------------Done with generation " << argGen << "\n\n";
    
    double N = (double)popSize;
    myMeanFitnessCurrGen = totFitness / N;
    //1/N ( SUM( x_i - m ) ^2 ) = 
    //1/N ( x_i^2 - 2 x_i m + m^2
    //1/N ( Sum(x_i^2) - 2 m Sum(x_i) + N m^2 )
    double variance = 1.0/N * ( totSqFitness - 2.0 * myMeanFitnessCurrGen * totFitness +  N * myMeanFitnessCurrGen * myMeanFitnessCurrGen );
    mySDFitnessCurrGen = ( variance < 0.00001 ) ? 0.0 : sqrt( variance );
                                  
    return lclBestOrg;
}



///////////////////
//// XML Genome Functions

void Else::addNodeXML( Genome& argGenome, std::ostringstream& argOStream,
             const NNode& argNode,
             double& argIPos,
             double& argHPos,
             int& argHLayer,
             double& argOPos )
{
    argOStream << "<neuron id=\"" << argNode.node_id << "\" type=\"";

    if ( argNode.gen_node_label == INPUT_NODE )
    {
        argOStream << "input\" hint-layer=\"1\" "
        << "hint-position=\"" << argIPos;
        argIPos += 1.0;
    } else if ( argNode.gen_node_label == OUTPUT )
    {
        argOStream << "output\" hint-layer=\"5\" "
        << "hint-position=\"" << argOPos;
        argOPos += 1.0;
    } else if ( argNode.gen_node_label  == HIDDEN )
    {
        if( argHPos > 5 && argHLayer < 4 )
        {
            argHPos = 1.0; argHLayer++;
        }
        argOStream << "hidden\" hint-layer=\"" << argHLayer <<"\" "
        << "hint-position=\"" << argHPos;
        argHPos += 1.0;
    } else if ( argNode.gen_node_label == BIAS )
    {
        argOStream << "input\" hint-layer=\"1\" "
        << "hint-position=\"" << argIPos;
        argIPos += 1.0;
    }
    argOStream << "\" step-pending=\"false\" activation=\"sigmoid\" />\n";
}

void Else::addLinkXML( Genome& argGenome, std::ostringstream& argOStream,
             const Link& argLink )
{
    argOStream << std::boolalpha;
    argOStream << "<connection "
    << "src-id=\"" << argLink.out_node->node_id << "\" "
    << "tgt-id=\"" << argLink.in_node->node_id << "\" "
    << "weight=\"" << argLink.weight << "\" "
    << "recurrence=\"" << argLink.is_recurrent << "\" />\n";
}


void Else::addNetworkXML( Genome& argGenome, std::ostringstream& argOStream )
{
    list<NNode*>::iterator curnode;
    list<Gene*>::iterator curgene;
    list<NNode*> nodes = argGenome.nodes ;
    list<Gene*> genes = argGenome.genes;
    argOStream << "<network>\n";
    double ipos = 1.0;
    double hpos = 1.0;
    double opos = 1.0;
    int hlayer = 2;
    for( curnode=nodes.begin(); curnode!=nodes.end(); ++curnode )
    {
        addNodeXML( argGenome, argOStream, *(*curnode), ipos, hpos, hlayer, opos );
    }
    for(curgene=genes.begin();curgene!=genes.end();++curgene)
    {
        addLinkXML( argGenome, argOStream, *((*curgene)->lnk) );
    }
    argOStream << "</network>\n";
}

