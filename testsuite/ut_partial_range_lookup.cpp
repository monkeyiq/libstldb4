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

    $Id: ut_partial_range_lookup.cpp,v 1.2 2010/09/24 05:15:38 ben Exp $

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
using namespace Ferris;
using namespace STLdb4;

const string PROGRAM_NAME = "ut_partial_range_lookup";

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

fh_ostream& E()
{
    ++errors;
    static fh_ostream ret = Factory::fcerr();
    return ret;
}

typedef list< string > stringlist_t;

void test( stringlist_t& stringlist, string expected )
{
    unlink( "/tmp/play.db" );
    fh_database db = new Database();
    db->create( DB_BTREE, "/tmp/play.db", "", DB_CREATE );

    string partial_k = "150";

    cerr << "populating the database" << endl;
    int postfix = 100;
    for( stringlist_t::iterator si = stringlist.begin(); si != stringlist.end(); ++si )
    {
        db->insert( make_pair( string("80,")    + tostr(postfix), *si ) );
        db->insert( make_pair( partial_k + ","  + tostr(postfix), *si ) );
        db->insert( make_pair( string("200,")   + tostr(postfix), *si ) );
        postfix += 5;
    }
    db->sync();

    cerr << "Doing a equal_range_partial()" << endl;
    std::pair< Database::iterator, Database::iterator > p = db->equal_range_partial( partial_k );
    
    cerr << "lb==end():" << (p.first == db->end()) << endl;
    cerr << "ub==end():" << (p.second == db->end()) << endl;
    cerr << "distance:" << distance( p.first, p.second ) << endl;

    if( (int)distance( p.first, p.second ) != stringlist.size() )
        E() << "equal_range_partial() doesn't have the correct number of items." << endl
            << " expecting:" << stringlist.size()
            << " got:" << distance( p.first, p.second )
            << endl;

    stringlist_t remaining = stringlist;
    
    for( Database::iterator di = p.first; di != p.second; ++di )
    {
        cerr << "di... k:" << di->first << " v:" << di->second << endl;
        remaining.erase( find( remaining.begin(), remaining.end(), di->second ) );
    }

    if( !remaining.empty() )
        E() << "Didnt find all the values for the equal_range_partial()." << endl
            << " the following were not found" << endl;
    copy( remaining.begin(), remaining.end(), ostream_iterator<string>(cerr) );
}


void runtest()
{
    {
        stringlist_t stringlist;
        stringlist.push_back( "ABC" );
        test( stringlist, "ABC" );
    }

    {
        stringlist_t stringlist;
        stringlist.push_back( "ABC" );
        stringlist.push_back( "DEF" );
        test( stringlist, "ABCDEF" );
    }

    {
        stringlist_t stringlist;
        stringlist.push_back( "ABC" );
        stringlist.push_back( "DEF" );
        stringlist.push_back( "GHI" );
        stringlist.push_back( "JKL" );
        stringlist.push_back( "MNO" );
        test( stringlist, "ABCDEFGHIJKLMNO" );
    }
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
    if( !errors )
        cerr << "Success" << endl;
    return exit_status;
}
