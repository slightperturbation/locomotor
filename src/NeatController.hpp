#ifndef NEATCONTROLLER_HPP
#define NEATCONTROLLER_HPP

#include <Else.hpp>

#include <Controller.hpp>

class Genome;
class Network;

namespace Else
{
    /// A controller based on one or more NEAT neural networks.  If
    /// more than one, all are structurally identical.
    class NeatController : public Controller
    {
    public:
        NeatController( Genome* argGenome,
                        int argSymmetryK,
                        int argModuleInputCount,
                        int argModuleOutputCount,
                        int argSharedInputCount,
                        int argSharedOutputCount,
                        int argCrossoverCount,
                        int argBiasCount,
                        bool argIsRecurrent,
                        int argMaxHiddenNodes );
        virtual ~NeatController();

        /// Sets the argIndex'th input for module argSymmetry to argValue.
        /// Generally used by the morphology to convey sensory data.
        void setInput( int argSymmetry, int argIndex, double argValue );

        /// Provides the argIndex'th output of module argSymmetry.
        /// Generally used by the morphology to get motor commands.
        double getOutput( int argSymmetry, int argIndex );

        /// Forward-integrate the contained neural nets.
        virtual void activate( void );

        virtual void acquireNeuronActivationData( std::vector< std::vector< double > >& outActivations );

        virtual std::string getDebugLabel( void ) const;
    private:
        Genome* myGenome;
        int mySymmetry;
        int myModuleInputCount; //< The number of "straight-through" inputs to the module, not shared by all
        int myModuleOutputCount; //< The number of "straight-through" outputs from the module, not shared by all
        int mySharedInputCount;
        int mySharedOutputCount;
        int myCrossoverCount;
        int myBiasCount;

        typedef boost::shared_ptr< Network > NetworkPtr;
        std::vector< NetworkPtr > myNetworkArray;

        /// A vector of length equal to the number of networks held (equal also to symmetry
        /// of controller).  Contains the sensor inputs set by the morphology, and then
        /// given to the neural networks just before activation.
        /// Lenght of each contained network is the total number of explicit inputs:
        /// = myModuleInputCount + mySharedInputCount + myCrossoverCount + myBiasCount
        std::vector< std::vector< double > > myInputsPerNetwork;

        /// A vector of length equal to the number of networks held (equal also to symmetry
        /// of controller).  Each vector is filled with the outputs of the corresponding
        /// neural network.
        /// Lenght of each contained network is the total number of explicit outputs:
        /// = myModuleOutputCount + mySharedOutputCount + myCrossoverCount
        std::vector< std::vector< double > > myOutputsPerNetwork;

        /// Historical record of activation histories
        /// Each vector element is a vector with size equal to the total number
        /// of neurons in the neural networks held by this controller.  Order is preserved.
        std::vector< std::vector< double > > myActivationHistoryArray;
    };
}
#endif // NEATCONTROLLER_HPP
