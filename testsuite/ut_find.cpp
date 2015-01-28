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

    $Id: ut_find.cpp,v 1.2 2010/09/24 05:15:38 ben Exp $

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

const string PROGRAM_NAME = "ut_find";

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

void runtest( string KEY = "harry" )
{
    fh_database db = new Database();
    typedef list< pair< string, string > > kvlist_t;
    kvlist_t kvlist;

    kvlist.push_back( make_pair( "fred", "12345" ));
    kvlist.push_back( make_pair( "harry", "hvalue" ));
    kvlist.push_back( make_pair( "sam", "I am sam" ));
    
    cerr << "runtest() creating database" << endl;
    db->create( DB_BTREE, "/tmp/play.db" );

    cerr << "populate the database with a sequence of key,value data." << endl;
    for( kvlist_t::iterator ki = kvlist.begin(); ki != kvlist.end(); ++ki )
    {
        db->set( ki->first, ki->second );
    }

    cerr << "get iterator to key:" << KEY << endl;
    Database::iterator di = db->find( KEY );
    
    cerr << "Iterate the database from key forward:" << KEY << endl;
    for( ; di != db->end(); ++di )
    {
        string k, v;

        cerr << "ut...get key" << endl;
        di.getKey( k );
        cerr << "ut...get value" << endl;
        di.getValue( v );
        
        cerr << "ut...key:" << k << endl;
        cerr << "ut...value:" << v << endl;
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

        runtest( argv[1] ? argv[1] : "harry");
    }
    catch( exception& e )
    {
        cerr << "cought error:" << e.what() << endl;
        exit(1);
    }

    return exit_status;
}
