/*
 *  MySqlDB.cpp
 *  else
 *
 *  Created by vector on 7/21/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */

#include <string>
#include "MySqlDB.hpp"

std::string MySqlDB::_server;
int MySqlDB::_port;

std::ostream& operator<< ( std::ostream& out, const Field& field )
{
    out << field.as<std::string>();
    return out;
}
