#ifndef ELSE_CONTROLLERFACTORY_HPP
#define ELSE_CONTROLLERFACTORY_HPP

#include <Else.hpp> // forward decl of ControllerPtr

#include <ParamsFromDB.hpp>

#include <string>

class Genome;

namespace Else
{
    /// Holds the info required to build controllers, including
    /// the type of controller to build.
    /// This factory only builds NEAT-based controllers.
    /// To support other subclasses of controllers, either turn this to an interface
    /// and make subclasses for each type of controller, or add a flag for controller
    /// type to this constructor.
    class ControllerFactory
    {
    public:
        ControllerFactory( const std::string& argName,
                           int argPerSideInputCount,
                           int argPerSideOutputCount,
                           int argSymmetryNumber = 1,
                           int argSharedInputCount = 0,
                           int argSharedOutputCount = 0,
                           int argCrossoverCount = 0,
                           int argBiasCount = 1,
                           bool argIsRecurrencyAllowed = true,
                           int argNetworkMaxHiddenNodes = 100
                );

        /// Provides the number of inputs of each contained neural network in the
        /// created controllers.
        int getNetworkInputCount( void ) { return myNetworkInputs; }

        /// Provides the number of outputs of each contained neural network in the
        /// created controllers.
        int getNetworkOutputCount( void ) { return myNetworkOutputs; }

        int getMaxHiddenNodeCount( void ) { return myMaxHiddenNodes; }

        bool getIsRecurrencyAllowed( void ) { return myIsRecurrencyAllowed; }

        /// Creates a controller by instancing SymmetryNumber copies of the neural network
        /// described by argGenome.
        ControllerPtr create( Genome* argGenome );

    private:
        std::string myName;
        int mySymmetryNumber;
        int myPerSideInputCount;
        int myPerSideOutputCount;
        int mySharedInputCount;
        int mySharedOutputCount;
        int myCrossoverCount;
        int myBiasCount;

        //////////////////////
        /// Genome specification variables
        /// inputs into the single neural network, used for genome creation
        int myNetworkInputs;
        /// outputs from single neural network, used for genome creation
        int myNetworkOutputs;
        /// max number of hidden nodes allowed in genome
        int myMaxHiddenNodes;
        /// if true, then allows recurrent connections (a neuron's output providing input to itself)
        bool myIsRecurrencyAllowed;
        //////////////////////

    };
    

}

#endif // ELSE_CONTROLLERFACTORY_HPP
