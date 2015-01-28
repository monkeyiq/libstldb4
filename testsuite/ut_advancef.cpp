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

    $Id: ut_advancef.cpp,v 1.2 2010/09/24 05:15:37 ben Exp $

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

const string PROGRAM_NAME = "ut_advancef";

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

void assert_string( const std::string& msg,
                    std::string expected,
                    std::string actual )
{
    if( expected != actual )
    {
        E() << msg << endl
            << " expected:" << expected
            << " actual:" << actual
            << endl;
    }
}

void runtest()
{
    fh_database db = 0;
    Database::advance_functor_t f = Database::makeAdvanceFunctorBytes();
    string expected;
    string s;
    string raw;
    const char twofivefive = (char)255;
    
    expected = "free";
    s = f( db, "fred" );
    assert_string( "non overflow failed", expected, s );

    expected = "boat";
    s = f( db, "boas" );
    assert_string( "non overflow failed", expected, s );

    expected = "hoi";
    expected += twofivefive;
    raw = "hoh";
    raw += twofivefive;
    s = f( db, raw );
    assert_string( "single overflow failed", expected, s );

    expected = "words";
    expected += twofivefive;
    expected += twofivefive;
    raw = "wordr";
    raw += twofivefive;
    raw += twofivefive;
    s = f( db, raw );
    assert_string( "double overflow failed", expected, s );

    expected = (char)1;
    raw = "";
    s = f( db, raw );
    assert_string( "null input string failed", expected, s );

    expected = "";
    expected += twofivefive;
    expected += twofivefive;
    expected += twofivefive;
    expected += twofivefive;
    expected += (char)1;
    raw = "";
    raw += twofivefive;
    raw += twofivefive;
    raw += twofivefive;
    raw += twofivefive;
    s = f( db, raw );
    assert_string( "four overflows followed by append requirement failed", expected, s );
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
