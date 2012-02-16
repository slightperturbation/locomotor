#ifndef ACROBOTARTICULATEDBODY_HPP
#define ACROBOTARTICULATEDBODY_HPP

#include "ArticulatedBody.hpp"
#include <ode/ode.h>

namespace Else
{
    class AcrobotArticulatedBody : public ArticulatedBody
    {
    public:
        AcrobotArticulatedBody( dWorldID argWorldID,
                                dSpaceID argSpaceID,
                                double   argScale );

        ~AcrobotArticulatedBody();

        virtual void createBody( void ) { }

        /// Fills outSensors with the data gathered from the sensors.
        virtual void getSensors( ControllerPtr argController, double argDeltaTime );

        /// Provides new values that the body should use to control it's actuators.
        virtual void setActuators( const ControllerPtr argController );

        virtual void writeCurrJointTraces( double argT );
        virtual void acquireJointAngleData( std::vector< double >& out );

        friend class AcrobotFitnessFunc;
    private:
        double scaleTorque( double argTorque );
        void displayMass( void );
        double currentAngle( void );
        double currentVel( void );

        dJointGroupID myJointGroupID;
        double myRadius;
        double myLength;
    };
}

#endif // ACROBOTARTICULATEDBODY_HPP
