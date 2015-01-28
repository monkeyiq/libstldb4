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

    $Id: ut_reverseiterate.cpp,v 1.2 2010/09/24 05:15:38 ben Exp $

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

const string PROGRAM_NAME = "ut_reverseiterate";

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


void runtest()
{
    fh_database db = new Database( DB_BTREE, "/tmp/play.db" );
    typedef list< pair< string, string > > kvlist_t;
    kvlist_t kvlist;

    kvlist.push_back( make_pair( "fred", "12345" ));
    kvlist.push_back( make_pair( "harry", "hvalue" ));
    kvlist.push_back( make_pair( "sam", "I am sam" ));
    
    cerr << "populate the database with a sequence of key,value data." << endl;
    for( kvlist_t::iterator ki = kvlist.begin(); ki != kvlist.end(); ++ki )
    {
        db->set( ki->first, ki->second );
    }


    
    {
        cerr << "get iterator" << endl;
        Database::iterator di = db->end();
        for( --di; ; --di )
        {
            cerr << " k:" << di->first << " v:" << di->second << endl;
            
            if( di == db->begin() )
                break;
        }
        cerr << "==================== done with raw reverse =========================" << endl;
    }
    
    
    


    cerr << "get iterator" << endl;
    Database::reverse_iterator di = db->rbegin();

//    cerr << "==================== GETTING rbegin() ================================" << endl;

//     cerr << "======1" << endl;
//     {
//         Database::iterator b = di.base();
//         --b;
//         const std::pair< std::string, std::string >& p = *b;
//         const string k = p.first;
//         cerr << "k1:" << k << endl;
//     }

    
//     cerr << "======2" << endl;
//     {
//         const std::pair< std::string, std::string >& p = *di;
//         const string k = p.first;
//         cerr << "k:" << k << endl;
//     }
    
    
//     cerr << "======3" << endl;
    
    
    cerr << "Iterate the database" << endl;
    for( ; di != db->rend(); ++di )
    {
        const std::pair< std::string, std::string >& p = *di;
//        string k = p.first;
        string k;
        di.getKey( k );
        string v = p.second;

        cerr << "ut...key:"   << k << endl;
        cerr << "ut...value:" << v << endl;
    }

    cerr << "===== Dumping out with copy( RAW ) =====" << endl;
    Database::reverse_iterator begin = db->rbegin();
    Database::reverse_iterator end   = db->rend();

    ostream_iterator<string> o(cerr,"\n");
    for( ; begin != end; ++begin, ++o )
        *o = begin->second;
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
