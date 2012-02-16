#ifndef ACROBOTFITNESSFUNC_HPP
#define ACROBOTFITNESSFUNC_HPP

#include <cmath>

#include "FitnessFunc.hpp"
#include "AcrobotArticulatedBody.hpp"
#include "PhysicsSimulator.hpp"

namespace Else
{
    class AcrobotFitnessFunc : public HistoricalFitnessFunc
    {
    public:
        AcrobotFitnessFunc( PopulationFitnessPtr argParent )
            : HistoricalFitnessFunc( argParent ),
              myMaxTime( 15.0 )
        {

        }
        virtual ~AcrobotFitnessFunc() {}
        virtual void setDoubleParam( const std::string& argName, double argValue )
        {
          if( argName == "TimeLimit" )
          {
            myMaxTime = argValue;
            return;
          }
          // Don't know it, so pass to parent class
          FitnessFunc::setDoubleParam( argName, argValue );
        }

        virtual void evaluate( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
        {
            AcrobotArticulatedBody* body = dynamic_cast<AcrobotArticulatedBody*>( argBody.get() );
            double angle = body->currentAngle();
            //std::cerr << "angle: " << angle << '\n';
            double score = std::fabs(angle) * argSim->getTimeStep();
            //std::cerr << "Score: " << score << '\n';
            score += myHistory.empty() ? 100 : myHistory.back();
            myHistory.push_back( score );
        }
        virtual double finalRawFitness( void )
        {
            return myHistory.back();
        }
        virtual bool shouldStopRun( PhysicsSimulatorPtr argSim, ArticulatedBodyPtr argBody )
        {
            return argSim->getElapsedTime() > myMaxTime;
        }
    protected:
        double myMaxTime;
    };
}

#endif // ACROBOTFITNESSFUNC_HPP
