#ifndef _Else_MySqlDB_hpp_
#define _Else_MySqlDB_hpp_

/*
 *  MySqlDB.hpp
 *  MySQLTest
 *
 *  Created by vector on 7/19/09.
 *  Copyright 2009 Brian F. Allen (vector@acm.org) All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <mysql.h>

#include <exception>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

class MySqlException : public std::exception
{
    // TODO
};

class Field;
std::ostream& operator<< ( std::ostream& out, const Field& field );


/// Holds a single field (one column of one row) in a result set.
/// Stores data as stringstream, can parse out to other types.
class Field
    {
    public:
        Field( const std::string& data = "" )
        {
            _data.str( data );
        }
        Field( const Field& other )
        {
            _data.str( other._data.str() );
        }
        Field& operator= ( const Field& other )
        {
            _data.str( other._data.str() );
            return *this;
        }
        template< typename outT > outT as( void ) const
        {
            outT out;
            _data >> out;
            return out;
        }
        std::string text( void ) const 
        {
            std::string out;
            out = _data.str();
            return out;
        }
        friend std::ostream& operator<< ( const std::ostream& out, const Field& field );
    private:
        mutable std::stringstream _data;
    };



/// Holds a full row of information from a database query.
class Row
    {
    public:
        Row()
        { }
        
        Row( const Row& arg )
        : _fields( arg._fields ), _labels( arg._labels )
        { }

        Row( MYSQL_RES * resultSet, const MYSQL_ROW& row )
        {
            MYSQL_FIELD *fields;
            int numFields = mysql_num_fields( resultSet );
            _labels.resize( numFields, "" );
            fields = mysql_fetch_fields( resultSet ); 
            unsigned long* lens = mysql_fetch_lengths( resultSet );
            for( int i = 0; i < numFields; ++i )
            {
                std::string tmp;
                if( row[i] )
                {
                    tmp.append( row[i], lens[i] );
                } 
                else 
                {
                    tmp = "NULL";
                }
                std::string fieldName( fields[i].name );
                _fields[ fieldName ] = Field( tmp );
                _labels[i] = fieldName;
            }
            
        }
        Field operator[] ( const char* fieldName )
        {
            std::string name( fieldName );
            std::map< std::string, Field >::iterator i = _fields.find( name );
            if( i == _fields.end() )
            {
                std::cerr << "Failed to find key: " << name << "\n";
                std::cerr << "Possible keys:\n";
                for( i = _fields.begin(); i != _fields.end(); ++i )
                {
                    std::cerr << "\t" << (*i).first << " -> " << (*i).second << "\n";
                }
                return Field("NA");
            }
            else
            {
                return _fields[ name ];
            }
        }

        Field operator[] ( const char* fieldName ) const
        {
            std::string name( fieldName );
            std::map< std::string, Field >::const_iterator i = _fields.find(name);
            if( i == _fields.end() )
            {
                std::cerr << "Failure.  No field with name \"" 
                          << fieldName << "\".\n"
                          << "Choices are:";
                for( i = _fields.begin(); i != _fields.end(); ++i )
                {
                    std::cerr << "\t" << (*i).first << " -> " 
                              << (*i).second << "\n";
                }
                return Field();
            }
            return _fields.find( name )->second;
        }
        
        Field operator[] ( const int& index )
        {
            
            return _fields[ _labels[index] ];
        }

        Field operator[] ( const int& index ) const
        {
            return _fields.find( _labels[index] )->second;
        }
        
        size_t numColumns( void ) const 
        {
            return _fields.size();
        }
        
        const std::vector< std::string >& columnLabels( void ) const
        {
            return _labels;
        }
        
    private:
        std::map< std::string, Field > _fields;
        std::vector< std::string > _labels;
    };
typedef std::vector< Row > Rows;

/// Singleton providing access to the mysql database.
/// Typical usage is:
///     MySqlDB& mySql = MySqlDB::instance();
///     Rows rows;
///     mySql.query( "SELECT id, name, somefloat FROM testdata", rows );    
///     for( Rows::const_iterator r = rows.begin(); r != rows.end(); ++r )
///     {
///         Row row = *r;
///         std::cout << "id -> " << row["id"].as<string>() << "\n";
///     }
///
class MySqlDB 
    {
    public:
        static void server( const std::string& arg )
        {
            _server = arg;
        }
        static void port( int arg )
        {
            _port = arg;
        }
        
        /// Provides access to the singleton object.
        static MySqlDB& instance( void )
        {
            static MySqlDB theDB( _server, _port );
            return theDB;
        }
        
        bool ping( void )
        {
            return mysql_ping( &_mySql );
        }
        
        void escape( std::string& dest, const std::string& src )
        {
            if( (2*src.length()) < _escapeBufLen )
            {
                escapeShort( dest, src );
            }
            else
            {
                escapeLong( dest, src );
            }
            dest = "'" + dest + "'";
        }
        
        std::string escape( const std::string& src )
        {
            std::string dest;
            escape( dest, src );
            return dest;
        }
                     
        
        /// Run a query and add the results to the outResult vector.
        void query( const std::string& argQueryString, Rows& outResult )
        {
            if( !_isConnected )
            {
                std::cerr << "FAILED TO RUN QUERY, NOT CONNECTED.  Query:\n" << argQueryString << "\n";
                return;
            }
            using namespace std;
            try {
                query( argQueryString );
                MYSQL_RES *queryResult; 
                queryResult = mysql_store_result( &_mySql ); // use?
                if( queryResult )
                {
                    while( MYSQL_ROW row = mysql_fetch_row( queryResult ) )
                    {
                        Row r( queryResult, row );
                        outResult.push_back( r );
                    }
                }
                mysql_free_result( queryResult );
            }
            catch (std::exception& e) 
            {
                std::cerr << "Exception in MySqlDB::query: " << e.what();
            }
        }
        
        /// Run a query where only the first row of output is used.
        void query( const std::string& argQueryString, Row& row )
        {
            Rows rows;
            query( argQueryString, rows );
            row = rows.front();            
        }
        
        /// Run a query that does not return any results (or ignore the results)
        void query( const std::string& argQueryString )
        {
            if( !_isConnected )
            {
                std::cerr << "FAILED TO RUN QUERY, NOT CONNECTED.  Query:\n" << argQueryString << "\n";
                return;
            }
            try {
                int retval = mysql_query( &_mySql, argQueryString.c_str() );
                if( retval )
                {
                    fprintf( stderr, "MySqlDB::query() error %d on SQL: \"%s\"", retval, argQueryString.c_str() );
                    fprintf( stderr, "MySQL Error: %s\n", mysql_error( &_mySql ));
                    throw MySqlException();
                }
            }
            catch (std::exception& e) 
            {
                std::cerr << "Exception in MySqlDB::query: " << e.what();
            }
        }
        
        /// Get ID of last auto-increment insert.
        long int lastId( void )
        {
            return mysql_insert_id( &_mySql );
        }
        
    private:
        MySqlDB( const std::string& argServer, int argPort )
        : _localPortNumber( argPort ),
          _serverName( argServer ),
          _isConnected( false )
        {
            try {
                establishSSHTunnel();
                connect();
                _mySql.reconnect = true;
            }
            catch (std::exception& e) 
            {
                std::cerr << "Exception in MySqlDB::ctor: " << e.what();
            }
        }
        
        /// NOT IMPLEMENTED to avoid inadvertent copying.
        MySqlDB( const MySqlDB& other );
        
        /// NOT IMPLEMENTED to avoid inadvertent copying.
        MySqlDB& operator= ( const MySqlDB& other );

        ~MySqlDB()
        {
            mysql_close( &_mySql );
            kill( _sshPID, SIGHUP );
        }
        
        /// Create ssh tunnel using public-key authentication
        void establishSSHTunnel( void )
        {
            _sshPID = fork();
            if( !_sshPID )
            {
                std::stringstream cmd;
                cmd << "ssh -f -n -L " << _localPortNumber << ":localhost:3306 " << _serverName << " sleep 10";
                std::cerr << "Attempting to establish ssh tunnel: " << cmd.str() << "\n";
                system( cmd.str().c_str() );
                sleep(100);
                exit(0);
            }
            sleep(3);
        }
        
        /// Establish a connection to the database.
        /// Assumes that the database is local.  If not, any ssh tunnel 
        /// should already be established.
        void connect( void )
        {
            const int attemptLimit = 500;
            int attempts = 0;
            _isConnected = false;
            while ( attempts < attemptLimit && !_isConnected )
            {
                MYSQL* mysqlRet;
                attempts++;
                if( !mysql_init(&_mySql) )
                {
                    goto errorCondition;
                }
                mysqlRet = mysql_real_connect( &_mySql, 
                                     "127.0.0.1",
                                     "vector",
                                     "sm4ck3r",
                                     "evocomp", // Initial DB
                                     _localPortNumber, // SSH Port
                                     0, 
                                     0);
                if( !mysqlRet )
                {  
                    goto errorCondition;
                }
                _isConnected = true;
                continue;
            errorCondition:
                const int retrySeconds = 4;
                fprintf( stderr, "Error when connecting to MySQL DB: Error: %s\n", mysql_error( &_mySql ));
                fprintf( stderr, "\tPossible causes: ssh tunnel wasn't established (is public-key auth setup correctly?), password on DB is wrong, DB not running, DB on different port locally.\n" );
                fprintf( stderr, "\tRetrying in %d seconds...\n", retrySeconds);
                // Wait before retrying
                sleep( retrySeconds );
            }
            if( !_isConnected )
            {
                fprintf( stderr, "Failed to connect to DB.  Giving up.\n\n" );
                throw MySqlException();
            }
        }

        /// escape a string using the stack
        void escapeShort( std::string& dest, const std::string& src )
        {
            static char tmp[_escapeBufLen];
            //std::assert( (2*src.length()) < _escapeBufLen );
            tmp[0] = 0;
            mysql_real_escape_string( &_mySql, tmp, src.c_str(), src.length() );
            dest = tmp;
        }
        
        /// escape a string using the heap
        void escapeLong( std::string& dest, const std::string& src )
        {
            // need two times cause each letter could potentially become
            // two letters
            char* tmp = new char[ (2 * src.length()) + 1 ];
            tmp[0] = 0;
            mysql_real_escape_string( &_mySql, tmp, src.c_str(), src.length() );
            dest = tmp;
            delete[] tmp;
        }
        
        MYSQL _mySql;
        std::string _serverName;
        int _localPortNumber;
        bool _isConnected;
        int _sshPID;
        static const unsigned int _escapeBufLen = 1048576;  // one meg, can escape upto 500KB

        static std::string _server;
        static int _port;
    };

#endif

