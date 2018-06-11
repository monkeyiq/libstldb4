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

    $Id: ut_multidata.cpp,v 1.2 2010/09/24 05:15:38 ben Exp $

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
using namespace Ferris;

const string PROGRAM_NAME = "ut_multidata";

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

void
runtest_with_one_entry()
{
    fh_database db = new Database( Environment::getDefault() );
    db->set_flags( DB_DUP );
    db->create( DB_BTREE, "/tmp/play2.db" );

    string k = "harry";
    string v = "sally";

//    db["aaa"] = "av";    
//    db["zzz"] = "zv";    
    db[ k ] = v;
    db->sync();

    cerr << "getting lower_bound()" << endl;
    Database::iterator lb = db->lower_bound( k );

    cerr << "getting upper_bound()" << endl;
    Database::iterator ub = db->upper_bound( k );

//    cerr << "lb: " << lb->second << endl;
//    cerr << "ub: " << ub->second << endl;

    cerr << "lb==end():" << (lb == db->end()) << endl;
    cerr << "ub==end():" << (ub == db->end()) << endl;
    cerr << "lb==ub:" << (lb==ub) << endl;
    cerr << "number of items in runtest_with_one_entry() range " << k
         << " is:" << distance( lb, ub ) << endl;

    if( distance( lb, ub ) != 1 )
        E() << "ERROR incorrect distange for single value range"
            << " expected:" << 1
            << " got:" << distance( lb, ub )
            << endl;

    if( lb->second != v )
        E() << "ERROR incorrect value for single value range"
            << " expected:" << v
            << " got:" << lb->second
            << endl;

    for( Database::iterator di = lb; di != ub; ++di )
    {
        cerr << "di... key:" << di->first << " value:" << di->second << endl;
    }
}


void runtest()
{
    // turn off transaction and logging
    STLdb4::Environment::setDefaultOpenFlags( DB_CREATE | DB_INIT_MPOOL );
    
    runtest_with_one_entry();
    
    char c = 0;
    fh_database db = new Database( Environment::getDefault() );
    db->set_flags( DB_DUP );
    db->create( DB_BTREE, "/tmp/play.db" );
    
    typedef list< string > stringlist_t;
    stringlist_t stringlist;

    string country = "france";
    stringlist.push_back( "charles_aznavour" );
    stringlist.push_back( "edith_piaf" );

    cerr << "populating the database" << endl;
    for( stringlist_t::iterator si = stringlist.begin(); si != stringlist.end(); ++si )
    {
        db->insert( make_pair( country, *si ) );
    }
    db->sync();
    
    {
        cerr << "Performing a raw dump of records using find() as base" << endl;
        Database::iterator di = db->find( country );
        cerr << "di:" << di->first << "  " << di->second << endl;
        ++di;
        cerr << "di:" << di->first << "  " << di->second << endl;
    }
    
    cerr << "getting lower_bound()" << endl;
    Database::iterator lb = db->lower_bound( country );

    cerr << "getting upper_bound()" << endl;
    Database::iterator ub = db->upper_bound( country );

    cerr << "lb==end():" << (lb == db->end()) << endl;
    cerr << "ub==end():" << (ub == db->end()) << endl;
    cerr << "lb==ub:" << (lb==ub) << endl;
    cerr << "number of singers in db for country " << country
         << " is:" << distance( lb, ub ) << endl;
    
    cerr << "showing lower to upper bound" << endl;
    for( Database::iterator di = lb; di != ub; ++di )
    {
        cerr << "di:" << di->first << "  " << di->second << endl;
    }
    
    
    
//     cerr << "reading from the database on single key" << endl;
//     pair< Database::iterator, Database::iterator > p = db->equal_range( country );
//     for( Database::iterator iter = p.first; iter != p.second; ++iter )
//     {
//         cerr << "Found multi data item for country:" << country
//              << " data:" << iter->second << endl;
//     }
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
