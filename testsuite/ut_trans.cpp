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

    $Id: ut_trans.cpp,v 1.3 2010/09/24 05:15:39 ben Exp $

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

const string PROGRAM_NAME = "ut_trans";

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
char* BasePath = "/tmp";

fh_ostream& E()
{
    ++errors;
    static fh_ostream ret = Factory::fcerr();
    return ret;
}

void runtest()
{
    string key   = "fred";
    string data  = "1234567890";
    string data2 = "AAABBBCCCDDDEEE";

    fh_trans trans = new Transaction();
    fh_database db = new Database( trans, DB_BTREE, (string)BasePath + "/play.db" );

    db->start();
    db[ key ] = data;
    cerr << "Initial value:" << db[ key ] << endl;
    db->commit();


    db->start();
    db[ key ] = data2;
    cerr << "set new value to value:" << db[ key ] << endl;
    cerr << "aborting that change." << endl;
    db->abort();

    cerr << "Final value:" << db[ key ] << endl;

    if( db[ key ] != data )
    {
        E() << "ERROR Abort() didn't work as expected." << endl
            << " expected:" << data
            << " got:" << db[key] << endl;
    }
    cerr << "done..." << endl;
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

                { "basepath", '1', POPT_ARG_STRING, &BasePath, 0,
                  "path to use for db files and db env.", "" },
                
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

        Environment::setDefault( new Environment( BasePath ) );

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
