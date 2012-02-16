/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef ELSE_CONTROLLER_HPP
#define ELSE_CONTROLLER_HPP

// Copyright (c) 2004, Brian F. Allen, 
//
// Contact: vector@acm.org
//


/** \file 
 \author Brian F. Allen (vector@acm.org)
 */

// Internal Includes
#include <Else.hpp>

// External Includes


namespace Else 
{    
    /// Interface class handling the evolved controller.
    /// A Controller generally encapsulates one or more neural nets or
    /// other control sub-systems.  The controller is responsible for
    /// dispatching the inputs to the various sub-systems used for control,
    /// and then marshalling their outputs into a single output which
    /// gets directed to the motor control.
    /// Responsible for ensuring input/output sizes match up.
    class Controller
    {
    public:
        /// Make a new random controller
        Controller( void );
        
        /// Abstract base class.
        virtual ~Controller( void ) {}
        
        /// Set the value of the input on the controller to argValue.
        /// The element set is the argIndex of "arm" argSymmetry.
        /// Symmetry 0 is always reserved for shared elements, even if none are used.
        /// For example, a bilaterally symmetric controller has valid argSymmetry
        /// values of 0, 1 and 2.  0 for the shared inputs, 1 and 2 for the 
        /// individual inputs.
        virtual void setInput( int argSymmetry, int argIndex, double argValue ) = 0;

        /// Returns the controller-output value for the specified output.
        /// The element set is the argIndex of "arm" argSymmetry.
        /// Symmetry 0 is always reserved for shared elements, even if none are used.
        /// For example, a bilaterally symmetric controller has valid argSymmetry
        /// values of 0, 1 and 2.  0 for the shared inputs, 1 and 2 for the 
        /// individual inputs.
        /// Output is between 0 and 1.
        virtual double getOutput( int argSymmetry, int argIndex ) = 0;

        /// Returns the controller-output value for the specified output.
        /// The element set is the argIndex of "arm" argSymmetry.
        /// Symmetry 0 is always reserved for shared elements, even if none are used.
        /// For example, a bilaterally symmetric controller has valid argSymmetry
        /// values of 0, 1 and 2.  0 for the shared inputs, 1 and 2 for the
        /// individual inputs.
        /// Output is between -1 and 1.
        virtual double getSignedOutput( int argSymmetry, int argIndex )
        {
            double x = (2.0 * getOutput( argSymmetry, argIndex )) - 1.0;
            x = std::min( 1.0, x );
            x = std::max( -1.0, x );
            return x;
        }

        /// Use provided sensor data to compute outputs.
        virtual void activate( void ) = 0;
        
        /// Fills the provided vectors with the current neuron activations.  The number 
        /// of vectors added is equal to the controller's symmetry, so if symmetry is 
        /// two (bilateral), then two vectors of double are added.
        virtual void acquireNeuronActivationData( std::vector< std::vector< double > >& outActivations )
        {
            return;
        }

        /// Returns a debugging label for this controller.
        virtual std::string getDebugLabel( void ) const = 0;
    };
}

#endif

