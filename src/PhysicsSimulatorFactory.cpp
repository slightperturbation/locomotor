#include "PhysicsSimulatorFactory.hpp"

#include "PhysicsSimulator.hpp"

Else::PhysicsSimulatorFactory::PhysicsSimulatorFactory( double argTimeStep )
    : myTimeStep( argTimeStep )
{
}

Else::PhysicsSimulatorPtr
Else::PhysicsSimulatorFactory::create( void )
{
    PhysicsSimulatorPtr sim( new PhysicsSimulator() );
    sim->setTimeStep( myTimeStep );
    return sim;
}
