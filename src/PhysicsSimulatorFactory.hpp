#ifndef PHYSICSSIMULATORFACTORY_HPP
#define PHYSICSSIMULATORFACTORY_HPP


#include "Else.hpp"

namespace Else
{
    /// Responsible for holding the info required to build physics simulators.
    class PhysicsSimulatorFactory
    {
    public:
        PhysicsSimulatorFactory( double argTimeStep );
        PhysicsSimulatorPtr create( void );
    private:
        double myTimeStep;
    };

}

#endif // PHYSICSSIMULATORFACTORY_HPP
