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

    $Id: ut_secondary.cpp,v 1.2 2010/09/24 05:15:38 ben Exp $

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

const string PROGRAM_NAME = "ut_secondary";

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


/*
 * getname -- extracts a secondary key (the last name) from a primary
 * 	key/data pair
 * this simply builds a secondary index on the enitre data from the primary
 * index.
 */
int
getname(DB* dbp, const DBT* pkey, const DBT* pdata, DBT* skey)
{
	memset(skey, 0, sizeof(DBT));
	skey->data = pdata->data;
	skey->size = pdata->size;
	return (0);
}

void runtest()
{
    unlink( "/tmp/play.db" );
    unlink( "/tmp/play.sec.db" );

    fh_database db    = new Database( DB_BTREE, "/tmp/play.db" );
    Database::sec_idx_callback f = getOffsetLengthSecIdx<0,2>();
    fh_database secdb = Database::makeSecondaryIndex( db, f, DB_BTREE, "/tmp/play.sec.db" );

    db->insert( make_pair( "fred", "501" ) );
    db->insert( make_pair( "foo", "60" ) );
    db->insert( make_pair( "bar", "6532" ) );
    db->insert( make_pair( "snafu", "502" ) );

    pair< Database::iterator, Database::iterator > p = secdb->equal_range( "50" );

    for( Database::iterator di = p.first; di != p.second; ++di )
    {
        string prim;
        di.getPrimaryKey( prim );
        
        cerr << "di... "
             << " primary:" << prim
             << " first:" << di->first
             << " second:" << di->second << endl;
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
