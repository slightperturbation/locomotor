#include "NeatController.hpp"

#include "neat/genetics.h"
#include "neat/networks.h"

Else::NeatController
        ::NeatController( Genome* argGenome,
                          int argSymmetryK,
                          int argModuleInputCount,
                          int argModuleOutputCount,
                          int argSharedInputCount,
                          int argSharedOutputCount,
                          int argCrossoverCount,
                          int argBiasCount,
                          bool argIsRecurrent,
                          int argMaxHiddenNodes )
: myGenome( argGenome ),
  mySymmetry( argSymmetryK ),
  myModuleInputCount( argModuleInputCount ),
  myModuleOutputCount( argModuleOutputCount ),
  mySharedInputCount( argSharedInputCount ),
  mySharedOutputCount( argSharedOutputCount ),
  myCrossoverCount( argCrossoverCount ),
  myBiasCount( argBiasCount )
{
    // First, verify that the totals add up to what the genome expects
    int totalModuleInputCount  =   myModuleInputCount
                                 + mySharedInputCount
                                 + myCrossoverCount
                                 + myBiasCount;
    int totalModuleOutputCount =   myModuleOutputCount
                                 + mySharedOutputCount
                                 + myCrossoverCount;

    argGenome->verifyOutputCount( totalModuleOutputCount );

    myInputsPerNetwork.resize( mySymmetry );
    myOutputsPerNetwork.resize( mySymmetry );

    // Build k networks
    for( int net = 0; net < mySymmetry; ++net )
    {
        NetworkPtr np( myGenome->genesis( myGenome->genome_id ) );
        myNetworkArray.push_back( np );
        myActivationHistoryArray.push_back( std::vector<double>() );

        myInputsPerNetwork[net].resize( totalModuleInputCount );
        myOutputsPerNetwork[net].resize( totalModuleOutputCount );
    }
}


Else::NeatController::~NeatController()
{
    // nada
}

void Else::NeatController
        ::setInput( int argSymmetry, int argIndex, double argValue )
{
    assert( myInputsPerNetwork.size() == myNetworkArray.size() );
    // To map to the network module, use order:
    // - module inputs
    // - shared inputs
    // - crossover inputs

    if( !argSymmetry )
    {
        // shared.  shared inputs come *after* the module inputs
        int firstShared = myModuleInputCount;
        for( int i = 0; i < myNetworkArray.size(); ++i )
        {
            myInputsPerNetwork[i][ firstShared + argIndex ] = argValue;
        }
    }
    else
    {
        int module = argSymmetry - 1;
        myInputsPerNetwork[module][argIndex] = argValue;
    }
}

double Else::NeatController
        ::getOutput( int argSymmetry, int argIndex )
{
    assert( myOutputsPerNetwork.size() == myNetworkArray.size() );
    if( !argSymmetry )
    {
        double val = 0;
        // shared output, take the mean of the outputs of each network
        int firstShared = myModuleOutputCount;
        for( int i = 0; i < myNetworkArray.size(); ++i )
        {
            val += myOutputsPerNetwork[i][ firstShared + argIndex ];
        }
        val /= (double)myNetworkArray.size();
        return val;
    }
    else
    {
        int module = argSymmetry - 1;
        return myOutputsPerNetwork[module][argIndex];
    }
}

void Else::NeatController
        ::activate( void )
{
    for( int i = 0; i < myNetworkArray.size(); ++i )
    {
        double* sensors = &(myInputsPerNetwork[i][0]);
        myNetworkArray[i]->load_sensors( sensors );
        myNetworkArray[i]->activate( myActivationHistoryArray[i] );

        std::list< NNode* >::const_iterator node = myNetworkArray[i]->outputs.begin();
        int j = 0;
        while( node != myNetworkArray[i]->outputs.end() )
        {
            myOutputsPerNetwork[i][j++] = (*node)->activation;
            node++;
        }
        assert( j == myOutputsPerNetwork[i].size() );
    }
}

void Else::NeatController
        ::acquireNeuronActivationData( std::vector< std::vector< double > >& outActivations )
{
    //Todo implement acquireNeuronActivationData
}

std::string Else::NeatController
        ::getDebugLabel( void ) const
{
    return "NeatController";
}
