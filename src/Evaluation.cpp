#include "Evaluation.hpp"



Else::Evaluation
::Evaluation( int argSimulationStepLimit,
              ControllerPtr argController,
              FitnessFuncPtr argFitnessFunc,
              PhysicsSimulatorPtr argSim,
              ArticulatedBodyPtr argBody,
              const std::vector<BodyState>& argInitialState,
              int argDisplayRate,
              int argUpdateRate,
              int argHarnessGens )
: mySimSteps( argSimulationStepLimit ),
 myController( argController ),
 mySim( argSim ),
 myBody( argBody ),
 myFitnessFunc( argFitnessFunc ),
 myInitialState( argInitialState ),
 myDisplayRate( argDisplayRate ),
 myUpdateRate( argUpdateRate ),
 myHarnessGens( argHarnessGens )
{
    myDisplayRate = (myDisplayRate < 1) ? 1 : myDisplayRate;
    myUpdateRate = (myUpdateRate < 1) ? 1 : myUpdateRate;

    assert( myFitnessFunc.get() );
    assert( mySim.get() );
    assert( myBody.get() );
    assert( myBody->isInSimulator( mySim->getWorldID() ) );
}

/// Execute this evaluation
Else::FitnessFuncPtr
Else::Evaluation
::operator() ( int argGeneration, int argTotalGenerations )
{

    std::vector< double > sensors;
    std::vector< double > actuators;

    myFitnessFunc->reset(); // ensure history is zerod'

    myStateData.clear();
    mySensorData.clear();
    myActuatorData.clear();
    myNeuronActivationData.clear();
    myJointAngleData.clear();
    //std::cout << myController->getDebugLabel() << ": ";

    // Force Repeatable-random
    //srand( trial );

    mySim->begin( argGeneration );
    myBody->acquireState( myStateData );

    if( myStateData.size() == myInitialState.size() )
    {
        myBody->setState( myInitialState );
    }

    for( int i = 0; i < mySimSteps; ++i )
    {
#ifdef ENABLE_TIMERS
        gblControlTimer->start();
#endif
        double deltaT = mySim->getTimeStep()
                        * myUpdateRate
                        * ((mySim->getUseFastStep())
                           ? mySim->getFastSubSteps()
                               : 1) ;
        sensors.clear();

        myBody->getSensors( myController, deltaT );
        myController->activate();
        myBody->setActuators( myController );

        myActuatorData.push_back( actuators );
        //assert( actuators.size() == mySim->getBody()->getActuatorCount() );


#ifdef ENABLE_TIMERS
        gblControlTimer->stop();
#endif
        double harnessStr = 0;
        //int harnessGens = 100;
        for( int j = 0; j < myUpdateRate; ++j )
        {
            // TODO Clean up harness

            if( argGeneration < myHarnessGens )
            {
                double fraction = (double)argGeneration / (double)myHarnessGens;
                harnessStr = (1.0 - sin( fraction * M_PI * 0.5) );
                //harnessStr = 1.18 * sin( 1.0 - (fraction*fraction) );
                //harnessStr = pow( cos( (double)argGeneration / (double)myHarnessGens ), 2 );

                //if( ((double)mySimSteps / 2.0) > (double)i )
                //{
                //    harnessStr = 0.7;
                //} else {
                //   harnessStr = cos(
                //        ((double)mySimSteps/2.0 - (double)i)
                //        / ((double) mySimSteps / 2.0 )
                //        );
                // }
            }
            else
            {
                harnessStr = 0;
            }

            myBody->addHarnessForce( harnessStr, mySim->getTimeStep() );

#ifdef ENABLE_TIMERS
            gblPhysicsTimer->start();
#endif
            mySim->step();
#ifdef ENABLE_TIMERS
            gblPhysicsTimer->stop();
#endif
            myBody->acquireState( myStateData );

            /// DISABLED ACTIVATION DATA FOR SPEED
            if( false )
            {
                myController->acquireNeuronActivationData( myNeuronActivationData );
                std::vector< double > jointData;
                myBody->acquireJointAngleData( jointData );
                myJointAngleData.push_back( jointData );
            }

            if( j % myDisplayRate == 0 )
            {
#ifdef ELSE_GUI
                Fl::check();
#endif
            }
        }

        if( ! myFitnessFunc.get() )
        {
            std::cerr << "No fitness func!";
            assert( false );
        }
        myFitnessFunc->evaluate( mySim, myBody );

        // TEMP__STATE
        {
            //std::ofstream outFile( "stateAnim.state", std::ios_base::app );
            //outFile << myStateData << '\n';
            //outFile.close();
        }
        // END TEMP__STATE

        if( myFitnessFunc->shouldStopRun( mySim, myBody ) )
        {
            break;
        }
    } // Sim steps

    mySim->end();
    //finalFitness += myFitnessFunc->last();
    //#ifdef ELSE_GUI
    //std::cout.width( 4 );
    //std::cout << " " << myFitnessFunc->last();
    //#endif
    //myFitnessFunc->getHistory().push_back( finalFitness / myTrialCount );

#ifdef ELSE_GUI
    std::cout << "\tTOTAL: "
              << myFitnessFunc->last() << std::endl;
#endif
    return myFitnessFunc;
}


int
Else::Evaluation
::getDisplayRate( void )
{
    return myDisplayRate;
}

void
Else::Evaluation
::setDisplayRate( int arg )
{
    myDisplayRate = arg;
}

const std::vector< std::vector< Else::BodyState > >&
Else::Evaluation
::getStateData( void )
{
    return myStateData;
}

const std::vector< std::vector< double > >&
Else::Evaluation
::getSensorData( void )
{
    return mySensorData;
}

const std::vector< std::vector< double > >&
Else::Evaluation
::getActuatorData( void )
{
    return myActuatorData;
}

const std::vector< std::vector< double > >&
Else::Evaluation
::getNeuronActivationData( void )
{
    return myNeuronActivationData;
}

const std::vector< std::vector< double > >&
Else::Evaluation
::getJointAngleData( void )
{
    return myJointAngleData;
}

Else::ControllerPtr
Else::Evaluation
::getController( void )
{
    return myController;
}

