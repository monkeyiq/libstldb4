/******************************************************************************
*******************************************************************************
*******************************************************************************

    Copyright (C) 2003 Ben Martin

    This file is part of libstldb4.

    libstldb4 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libstldb4 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libstldb4.  If not, see <http://www.gnu.org/licenses/>.

    For more details see the COPYING file in the root directory of this
    distribution.

    $Id: ut_setget.cpp,v 1.2 2010/09/24 05:15:39 ben Exp $

*******************************************************************************
*******************************************************************************
******************************************************************************/

/*
 * return 0 for success
 * return 1 for generic error
*/

#include <STLdb4/stldb4.hh>

#include <popt.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <iterator>
#include <iostream>

using namespace std;
using namespace STLdb4;

const string PROGRAM_NAME = "ut_setget";

void usage(poptContext optCon, int exitcode, char *error, char *addl)
{
    poptPrintUsage(optCon, stderr, 0);
    if (error) fprintf(stderr, "%s: %s0", error, addl);
    exit(exitcode);
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

int errors = 0;

// void raw()
// {
//     DB_ENV* m_env;
//     DB* m_db;
//     u_int32_t flags = 0;
//     int rc;
    
//     int default_open_flags = DB_CREATE;
//     int default_open_mode  = 0;

// //    default_open_flags = (DB_CREATE | DB_INIT_TXN | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL);

    
//     int open_flags = default_open_flags;
//     int mode = default_open_mode;

//     rc = db_env_create( &m_env, 0 );
//     if( rc != 0 )
//         cerr << "db_env_create() rc:" << rc << endl;
// //    m_env->set_errfile( m_env, stderr);
//     m_env->set_errpfx(  m_env, "stldb4 env");

// //    rc = m_env->open( m_env, "/home/ben/tmp", open_flags, mode );
//     rc = m_env->open( m_env, "/tmp", open_flags, mode );
//     cerr << "raw() rc:" << rc << endl;
    
//     rc = db_create( &m_db, 0, flags );
//     if( rc != 0 )
//         cerr << "db_create() rc:" << rc << endl;
    
//     m_db->set_errfile( m_db, stderr);
//     m_db->set_errpfx(  m_db, "xxx");

//     rc = m_db->open( m_db,
//                      0,
//                      "/tmp/play.db",
//                      0,
//                      DB_BTREE,
//                      DB_CREATE,
//                      0 );
    
//     if( rc != 0 )
//     {
//         cerr << "db_create() rc:" << rc << endl;
//     }
    
//     cerr << "done." << endl;
// }


void runtest()
{
//    raw();
    
    fh_database db = new Database();
    string data = "fred's value";

    cerr << "runtest() creating database" << endl;
    db->create( DB_BTREE, "/tmp/play.db" );

//     cerr << "b == e:" << ( db->begin() == db->end() ) << endl;
    
    cerr << "runtest() setting key to value:" << data << endl;
    db->set( "fred", data );

    cerr << "reading back value" << endl;
    string v;
    db->get( "fred", v );

    if( v != data )
    {
        cerr << "ERROR should have read back:" << data << " len:" << data.length()
             << " but got:" << v << " len:" << v.length() << endl;
        ++errors;
    }

    if( !errors )
        cerr << "Success" << endl;
}


int main( int argc, char** argv )
{
    int exit_status = 0;
    
    try
    {
        unsigned long Verbose              = 0;

        struct poptOption optionsTable[] =
            {
                { "verbose", 'v', POPT_ARG_NONE, &Verbose, 0,
                  "show what is happening", "" },

                POPT_AUTOHELP
                POPT_TABLEEND
            };
        poptContext optCon;

        optCon = poptGetContext(PROGRAM_NAME.c_str(), argc, (const char**)argv, optionsTable, 0);
        poptSetOtherOptionHelp(optCon, "[OPTIONS]*  ...");

        /* Now do options processing */
        char c=-1;
        while ((c = poptGetNextOpt(optCon)) >= 0)
        {}

        runtest();
    }
    catch( exception& e )
    {
        cerr << "cought error:" << e.what() << endl;
        exit(1);
    }
    return exit_status;
}
