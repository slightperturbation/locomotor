#include "ControllerFactory.hpp"

#include <NeatController.hpp>

Else::ControllerFactory
::ControllerFactory( const std::string& argName,
                     int argPerSideInputCount,
                     int argPerSideOutputCount,
                     int argSymmetryNumber,
                     int argSharedInputCount,
                     int argSharedOutputCount,
                     int argCrossoverCount,
                     int argBiasCount,
                     bool argIsRecurrencyAllowed,
                     int argNetworkMaxHiddenNodes )
: myName( argName ),
  mySymmetryNumber( argSymmetryNumber ),
  myPerSideInputCount( argPerSideInputCount ),
  myPerSideOutputCount( argPerSideOutputCount ),
  mySharedInputCount( argSharedInputCount ),
  mySharedOutputCount( argSharedOutputCount ),
  myCrossoverCount( argCrossoverCount ),
  myBiasCount( argBiasCount ),
  myMaxHiddenNodes( argNetworkMaxHiddenNodes ),
  myIsRecurrencyAllowed( argIsRecurrencyAllowed )
{
    myNetworkInputs =   myBiasCount
                      + myPerSideInputCount
                      + mySharedInputCount
                      + myCrossoverCount;
    myNetworkOutputs =   myPerSideOutputCount
                       + mySharedOutputCount
                       + myCrossoverCount;
}

Else::ControllerPtr
Else::ControllerFactory
::create( Genome* argGenome )
{
    return ControllerPtr( new NeatController( argGenome,
                                              mySymmetryNumber,
                                              myPerSideInputCount,
                                              myPerSideOutputCount,
                                              mySharedInputCount,
                                              mySharedOutputCount,
                                              myCrossoverCount,
                                              myBiasCount,
                                              myIsRecurrencyAllowed,
                                              myMaxHiddenNodes )
                          );
}
