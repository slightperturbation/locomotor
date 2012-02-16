/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef ELSE_ELSEEXCEPTION_HPP
#define ELSE_ELSEEXCEPTION_HPP

// Copyright (c) 2004, Brian F. Allen,
//
// Contact: vector@acm.org
//


/** \file 
\author Brian F. Allen (vector@acm.org)
*/

// Project Includes
#include <Else.hpp>

// External Includes
#include <exception>

namespace Else
{
  /// Super-class for handling exception info
  class ElseException : public std::exception
  {
  public:
    ElseException( std::string argMsg = "Unspecified",
      std::string argUserFriendlyMsg = "An unknown error occured." )
      : myMsg( argMsg ), myUserFriendlyMsg( argUserFriendlyMsg	)
    {
    }

    virtual ~ElseException() throw() {}

    virtual const char* what( void ) 
    {
      return getMsg().c_str();
    }

		virtual const std::string& getMsg( void )
		{
			return myMsg;
		}

    virtual const std::string& getUserFriendlyMsg( void )
    {
      return myUserFriendlyMsg;
    }

  protected:
    std::string myMsg;
    std::string myUserFriendlyMsg;
  };
}

#endif

//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: ElseException.hpp,v $
// Revision 1.3  2004/06/28 00:08:05  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.2  2004/04/30 23:29:36  dexabraxis
// *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////
