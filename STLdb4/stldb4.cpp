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

    $Id: stldb4.cpp,v 1.16 2010/09/24 05:15:25 ben Exp $

*******************************************************************************
*******************************************************************************
******************************************************************************/

#define BUILDING_STLDB4

#ifdef MAC
extern int errno;
#endif


#include <STLdb4/stldb4.hh>
#include <FerrisLoki/Extensions.hh>
#include <FerrisStreams/Streams.hh>

#include <sigc++/sigc++.h>
#include <sigc++/slot.h>
#include <sigc++/object.h>
#include <sigc++/object_slot.h>

// #ifndef STLDB4_DONT_USE_GLIB
// // only needed for gint32, gint64 etc.
// #include <glib.h>
// #else
// typedef int16_t gint16;
// typedef u_int16_t guint16;
// typedef int32_t gint32;
// typedef u_int32_t guint32;
// typedef int64_t gint64;
// typedef u_int64_t guint64;
//#endif

#include <sstream>
#include <iostream>

using namespace std;


namespace STLdb4
{
    static string AdvanceFunctorBytes( fh_database, const std::string& k );

    
    static void clear( DBT& d )
    {
        memset( &d, 0, sizeof( DBT ) );
    }
    static DBT* setup( DBT& d, const char* data, std::streamsize datalen )
    {
        clear( d );
        d.data = (char*)data;
        d.size = datalen;
        d.ulen = datalen;
        d.flags = DB_DBT_USERMEM;
        return &d;
    }
    DBT* toKDBT( DBT& ret, Database::iterator di )
    {
        string k;
        di.getKey( k );
        setup( ret, k.c_str(), k.length() );
        return &ret;
    }
    
    
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/

    void ThrowFromDB4Error( int ecode )
    {
        stringstream ss;
        ss << "ThrowFromDB4Error ecode:" << ecode << endl;
        throw dbException( ss.str(), ecode );
    }

    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/

    void
    CoreDataChunk::DoubleSize( bool preserve )
    {
//        std::cerr << "DoubleSize()" << std::endl;
        std::streamsize oldsz = m_data.size();

        if( !oldsz ) oldsz = 4096;
//        std::cerr << "DoubleSize() oldsz:" << oldsz << std::endl;
                
        std::string s;
        s.resize( oldsz * 2 );
//        std::cerr << "DoubleSize(2)" << std::endl;

//                 for( int i = 1; i < oldsz * 2; ++i )
//                 {
// //                    std::cerr << "DoubleSize(x)" << std::endl;
//                     s.append( " " );
//                 }
                
//        std::cerr << "DoubleSize(3)" << std::endl;
                
        m_data = s;
    }

    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/


    class FERRISEXP_DLLLOCAL NullEnvironment
        :
        public Environment
    {
//     protected:
//         virtual void commonSetup( std::string db_home,
//                                   u_int32_t create_flags,
//                                   u_int32_t open_flags,
//                                   int mode )
//             {
//                 cerr << "NullEnvironment::commonSetup() " << endl;
//             }
        
        
    public:
        NullEnvironment()
            :
            Environment( 0.0 )
            {
            }
        virtual ~NullEnvironment()
            {
            }
        
        virtual DB_ENV* raw()
            {
                return 0;
            }
    };
    

    void
    Environment::commonSetup( std::string db_home,
                              u_int32_t create_flags,
                              u_int32_t open_flags,
                              int mode )
    {
//         {
//             int rc;

//             rc = db_env_create( &m_env, 0 );
//             cerr << "openA1 rc:" << rc << endl;
//             rc = m_env->open( m_env, 0, 0, 0 );
//             cerr << "openA2 rc:" << rc << endl;
//             m_env->err( m_env, rc, "DB_ENV->open: %s", "");
//         }
        

        
        int rc = db_env_create( &m_env, create_flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        m_env->set_errfile( m_env, stderr);
        m_env->set_errpfx(  m_env, "stldb4 env");

        if (open_flags & (DB_TXN_NOSYNC))
            m_env->set_flags( m_env, DB_TXN_NOSYNC, 1 );
        open_flags &= ~(DB_TXN_NOSYNC);
        
        if( !db_home.empty() )
        {
            db_home = Ferris::CleanupURL( db_home );
//            cerr << "Environment::commonSetup() db_home:" << db_home << endl;
        }

//         cerr << "Environment::commonSetup(4) db_home:" << db_home << endl;
//         cerr << "Environment::commonSetup(4)   flags:" << open_flags << endl;
//         cerr << "Environment::commonSetup(4)    mode:" << mode << endl;
        rc = m_env->open( m_env, db_home.empty() ? (char*)0 : (char*)db_home.c_str(),
                          open_flags, mode );
//        cerr << "Environment::commonSetup(5) rc:" << rc << endl;
//
//
//        cerr << "open rc:" << rc << " db_home:" << db_home << endl;
        if( rc != 0 )
            ThrowFromDB4Error( rc );

    }

    Environment::Environment( double dummy )
        :
        m_env( 0 )
    {
    }
    
    Environment::Environment( const std::string& db_home,
                              u_int32_t flags, int mode )
        :
        m_env( 0 )
    {
        commonSetup( db_home, 0, flags, mode );
    }
    
    Environment::Environment( u_int32_t flags, int mode )
        :
        m_env( 0 )
    {
        commonSetup( "", 0, flags, mode );
    }
    
    
    Environment::~Environment()
    {
//        cerr << "Environment::~Environment()" << endl;
        
        if( m_env )
        {
            int rc = m_env->close( m_env, 0 );
//            cerr << "Environment::~Environment() rc:" << rc << endl;
            m_env = 0;
            if( rc != 0 )
                ThrowFromDB4Error( rc );
        }
    }

    DB_ENV*
    Environment::getRaw()
    {
        return m_env;
    }

    
    u_int32_t
    Environment::get_openflag_create()
    {
        return DB_CREATE;
    }
    
    u_int32_t
    Environment::get_openflag_init_txn()
    {
        return DB_INIT_TXN;
    }
    
    u_int32_t
    Environment::get_openflag_init_lock()
    {
        return DB_INIT_LOCK;
    }
    
    u_int32_t
    Environment::get_openflag_init_log()
    {
        return DB_INIT_LOG;
    }
    
    u_int32_t
    Environment::get_openflag_init_mpool()
    {
        return DB_INIT_MPOOL;
    }
    
     const u_int32_t  environment_default_open_flags = Environment::get_openflag_create()
        | Environment::get_openflag_init_txn()
        | Environment::get_openflag_init_lock()
        | Environment::get_openflag_init_log()
        | Environment::get_openflag_init_mpool()
        | DB_RECOVER
        ;
    
     const u_int32_t environment_default_open_mode  = 0;
    
    u_int32_t
    Environment::s_defaultOpenFlags = environment_default_open_flags;

    void
    Environment::setDefaultOpenFlags( u_int32_t flags )
    {
        s_defaultOpenFlags = flags;
    }

    static fh_env& getDefaultEnv()
    {
        static fh_env ret = new NullEnvironment();
        return ret;
    }

    fh_env
    Environment::getDefault()
    {
        return getDefaultEnv();
    }
    
    
    fh_env
    Environment::getDefaultIfNull( fh_env e )
    {
        if( !e )
        {
            e = getDefault();
        }
        return e;
    }

    fh_env
    Environment::getNull()
    {
        return new NullEnvironment();
    }
    
    void
    Environment::setDefault( fh_env e )
    {
        getDefaultEnv() = e;
    }
    
    
    DB_ENV*
    Environment::raw()
    {
        return getRaw();
    }
    
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/

    Transaction::Transaction( bool WeAreFake )
        :
        m_txn( 0 ),
        m_DefaultDestructionIsAbort( false )
    {
//        cerr << "Transaction(waf:" << WeAreFake << ")" << endl;
    }
    
    
    Transaction::Transaction( fh_trans parent, fh_env env, u_int32_t flags )
        :
        m_txn( 0 ),
        m_DefaultDestructionIsAbort( false )
    {
//        cerr << "Transaction(p,env,flags)" << endl;
        env = Environment::getDefaultIfNull( env );
        DB_ENV* raw = env->getRaw();
        DB_TXN* parent_txn = parent ? parent->m_txn : 0;

//         cerr << "Transaction::Transaction() raw:" << Ferris::toVoid( raw )
//              << " ptxn:" << Ferris::toVoid(parent_txn)
//              << " flags:" << flags
//              << endl;
        
        int rc = raw->txn_begin( raw, parent_txn, &m_txn, flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }
    
    Transaction::~Transaction()
    {
//        cerr << "~Transaction() m_DefaultDestructionIsAbort:" << m_DefaultDestructionIsAbort << endl;
        if( m_DefaultDestructionIsAbort )
        {
            abort();
        }
        else
        {
            commit();
        }
    }

    void
    Transaction::setDefaultDestructionIsAbort( bool v )
    {
        m_DefaultDestructionIsAbort = v;
    }
    
    u_int32_t
    Transaction::id()
    {
        return m_txn->id( m_txn );
    }

    u_int32_t
    Transaction::set_timeout( db_timeout_t timeout, u_int32_t flags )
    {
        u_int32_t rc = m_txn->set_timeout( m_txn, timeout, flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
        return rc;
    }

    
    struct RefCountBumpRAII
    {
        FerrisLoki::Handlable* h;
        RefCountBumpRAII( FerrisLoki::Handlable* h )
            :
            h(h)
            {
                h->ref_count++;
            }
        ~RefCountBumpRAII()
            {
                h->ref_count--;
            }
    };

    /**
     * Note that we have to use RefCountBumpRAII here to make sure there
     * is a reference to this object because commit()/abort() are called
     * from ~Transaction() and we don't want the transaction we create
     * for emitting a signal to call ~Transaction() again.
     */
    void
    Transaction::commit( u_int32_t flags )
    {
        if( m_txn )
        {
//            cerr << "Transaction::commit() txn:" << id() << endl;

            int rc = m_txn->commit( m_txn, flags );
            RefCountBumpRAII _obj(this);
            fh_trans t = this;
            getCommitSig().emit( t, rc != 0 );
            if( rc != 0 )
                ThrowFromDB4Error( rc );
            m_txn = 0;
        }
    }
    
    void
    Transaction::abort()
    {
        if( m_txn )
        {
//            cerr << "Transaction::abort() txn:" << id() << " rc:" << getReferenceCount() << endl;
            
            int rc = m_txn->abort( m_txn );
            RefCountBumpRAII _obj(this);
            fh_trans t = this;
            getAbortSig().emit( t, rc != 0 );
            if( rc != 0 )
                ThrowFromDB4Error( rc );
            m_txn = 0;
        }
    }
    
    DB_TXN*
    Transaction::raw()
    {
        return m_txn;
    }


    class FERRISEXP_DLLLOCAL NullTransaction
        :
        public Transaction
    {
    public:

        NullTransaction()
            :
            Transaction( true )
            {
//                cerr << "NullTransaction()" << endl;
            }
        ~NullTransaction()
            {
            }
        
        u_int32_t id()
            {
                return 0;
            }
        
        u_int32_t set_timeout( db_timeout_t timeout, u_int32_t flags )
            {
                return 0;
            }
        
        void commit( u_int32_t flags )
            {
            }
            
        void abort()
            {
            }

        DB_TXN* raw()
            {
                return 0;
            }
        
    };

    
    fh_trans getNullTransaction()
    {
        static fh_trans ret = new NullTransaction();
        return ret;
    }
    
    

    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/

//     static int& getAllDBCount()
//     {
//         static int x = 0;
//         return x;
//     }
// #include <execinfo.h>
//     static void BackTrace( int fd = 2 )
//     {
//         const int arraysz = 500;
//         void* array[arraysz];
//         size_t size;

//         size = backtrace( array, arraysz );
//         backtrace_symbols_fd( array, size, fd );
//     }

    
    void
    Database::commonSetup( fh_env env )
    {
        m_bt_compare_fcn = 0;

        if( !env )
            env = Environment::getDefault();
        m_env = env;
        
        u_int32_t flags = 0;
        
        int rc = db_create( &m_db, env->raw(), flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        m_db->set_errfile( m_db, stderr);
        m_db->set_errpfx(  m_db, "stldb4");

        setAdvanceFunctor( makeAdvanceFunctorBytes() );

        if( !env->raw() )
        {
            set_cachesize( 1 );
        }
//        set_cachesize( 0, 256*1024, 1 );
        
//         ++getAllDBCount();
//         cerr << "Database::Database( create ) count:" << getAllDBCount() << endl;
//         BackTrace();
    }
    

    Database::Database( fh_env env )
        :
        m_implicitTransaction( 0 )
    {
        commonSetup( env );
    }

    Database::Database( const std::string& filename,
                        const std::string& database,
                        bool read_only )
    {
        u_int32_t flags = 0;
        if( read_only )   flags |= DB_RDONLY;
        else              flags |= 0;
        
        commonSetup();
        impl_open( filename, database, DB_UNKNOWN, flags, 0, 0 );
    }
    

    fh_database
    Database::makeSecondaryIndex( fh_database pdb,
                                  sec_idx_callback f,
                                  DBTYPE type,
                                  const std::string& filename,
                                  const std::string& database,
                                  u_int32_t flags,
                                  int mode )
    {
        return makeSecondaryIndex( 0, pdb, f, type, filename, database, flags, mode );
    }
    
    fh_database
    Database::makeSecondaryIndex( fh_trans trans,
                                  fh_database pdb,
                                  sec_idx_callback f,
                                  DBTYPE type,
                                  const std::string& filename,
                                  const std::string& database,
                                  u_int32_t flags,
                                  int mode,
                                  bt_compare_fcn btcompf )
    {
        fh_database ret = new Database();
        ret->set_flags( DB_DUP | DB_DUPSORT );
        if( btcompf )
            ret->set_bt_compare( btcompf );
        
        ret->create( trans, type, filename, database, flags, mode );
        pdb->associate( ret, f, 0, trans );
        return ret;
    }


    fh_database
    Database::makeSecondaryIndex( fh_env env,
                                  fh_trans trans,
                                  fh_database pdb,
                                  sec_idx_callback f,
                                  DBTYPE type,
                                  const std::string& filename,
                                  const std::string& database,
                                  u_int32_t flags,
                                  int mode,
                                  bt_compare_fcn btcompf )
    {
//        cerr << "Database::makeSecondaryIndex()!" << endl;
        
        fh_database ret = new Database( env );
        ret->set_flags( DB_DUP | DB_DUPSORT );
        if( btcompf )
            ret->set_bt_compare( btcompf );
        
        ret->create( trans, type, filename, database, flags, mode );
        pdb->associate( ret, f, 0, trans );
        return ret;
    }
    

    Database::~Database()
    {
        close();
    }

    void
    Database::close()
    {
        commit();
        
        if( m_db )
        {
//            cerr << "Database::~Database()" << endl;
//            sync();
            
            int rc = m_db->close( m_db, 0 );
            if( rc != 0 )
            {
                cerr << "error closing db4 file:" << rc << endl;
                ThrowFromDB4Error( rc );
            }
            m_db = 0;
        }

//         cerr << "Database::~Database( delete ) count:" << getAllDBCount() << endl;
//         BackTrace();
//         --getAllDBCount();
    }
    
    
    void
    Database::set_flags( u_int32_t flags )
    {
        int rc = m_db->set_flags( m_db, flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }

    void
    Database::set_pagesize( u_int32_t pagesize )
    {
        int rc = m_db->set_pagesize( m_db, pagesize );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }

    int
    Database::set_cachesize( u_int32_t gbytes, u_int32_t bytes, int ncache )
    {
        int rc = m_db->set_cachesize( m_db, gbytes, bytes, ncache );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
        return rc;
    }

    int
    Database::set_cachesize( unsigned long MegaBytes )
    {
        return set_cachesize( 0, MegaBytes * 1024 * 1024, 1 );
    }
    
    
    
    
    fh_trans
    Database::freshenTransaction( fh_trans t )
    {
        if( t )
            return t;
        if( m_implicitTransaction )
            return m_implicitTransaction;

        return getNullTransaction();
    }

    void
    Database::OnImplicitTransactionCommitOrAbort( fh_trans t, bool succ )
    {
        clearImplicitTransaction();
    }
    
    void
    Database::clearImplicitTransaction()
    {
        m_OnCommitConnection.disconnect();
        m_OnAbortConnection.disconnect();
        m_implicitTransaction = 0;
    }
    
    void
    Database::setImplicitTransaction( fh_trans t )
    {
        if( m_implicitTransaction )
        {
            m_implicitTransaction->commit();
            clearImplicitTransaction();
        }

        m_implicitTransaction = t;

        if( m_implicitTransaction )
        {
            m_implicitTransaction->getCommitSig().connect(
                sigc::mem_fun( *this, &Database::OnImplicitTransactionCommitOrAbort ));
            m_implicitTransaction->getAbortSig().connect(
                sigc::mem_fun( *this, &Database::OnImplicitTransactionCommitOrAbort ));
        }
    }
    
    void
    Database::commit()
    {
        setImplicitTransaction( 0 );
    }
    
    void
    Database::abort()
    {
        if( m_implicitTransaction )
        {
            m_implicitTransaction->abort();
            clearImplicitTransaction();
        }
    }

//     typedef map< DB*, Database::m_bt_prefix_functor_t* > DatabasePrefixFunctorsMap_t;
//     DatabasePrefixFunctorsMap_t& getDatabasePrefixFunctorsMap()
//     {
//         static DatabasePrefixFunctorsMap_t ret;
//         return ret;
//     }

    void
    Database::set_bt_prefix( bt_prefix_fcn f )
    {
        int rc = m_db->set_bt_prefix( m_db, f );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
        
//         getDatabasePrefixFunctorsMap()[ m_db ] = &m_bt_prefix_functor;
//         m_bt_prefix_fcn = f;
        
//         int rc = m_db->set_bt_prefix( m_db, m_bt_prefix_fcn );
//         if( rc != 0 )
//             ThrowFromDB4Error( rc );
    }

//     static size_t  bt_prefix_function_from_functor(DB* rawdb, const DBT* a, const DBT* b )
//     {
//         return (*(getDatabasePrefixFunctorsMap()[ rawdb ]))( rawdb, a, b );
//     }
//     void
//     Database::set_bt_prefix( m_bt_prefix_functor_t f )
//     {
//         m_bt_prefix_functor = f;
//         getDatabasePrefixFunctorsMap()[ m_db ] = &m_bt_prefix_functor;
//         m_bt_prefix_fcn = bt_prefix_function_from_functor;
        
//         int rc = m_db->set_bt_prefix( m_db, m_bt_prefix_fcn );
//         if( rc != 0 )
//             ThrowFromDB4Error( rc );
//     }
    
    
    
    typedef map< DB*, Database::m_bt_compare_functor_t* > DatabaseFunctorsMap_t;
    DatabaseFunctorsMap_t& getDatabaseFunctorsMap()
    {
        static DatabaseFunctorsMap_t ret;
        return ret;
    }
    
    void
    Database::set_bt_compare( bt_compare_fcn f )
    {
        getDatabaseFunctorsMap()[ m_db ] = &m_bt_compare_functor;
        m_bt_compare_fcn = f;
        
        int rc = m_db->set_bt_compare( m_db, m_bt_compare_fcn );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }


    static int  bt_compare_function_from_functor(DB* rawdb, const DBT* a, const DBT* b )
    {
//        cerr << "bt_compare_function_from_functor()" << endl;
//        cerr << "FOUND:" << ( getDatabaseFunctorsMap().end() != getDatabaseFunctorsMap().find( rawdb ) ) << endl;
//        cerr << "bt_compare_function_from_functor(2)" << endl;
        
        return (*(getDatabaseFunctorsMap()[ rawdb ]))( rawdb, a, b );
    }
    void
    Database::set_bt_compare( m_bt_compare_functor_t f )
    {
        m_bt_compare_functor = f;
        getDatabaseFunctorsMap()[ m_db ] = &m_bt_compare_functor;
        m_bt_compare_fcn = bt_compare_function_from_functor;
        
        int rc = m_db->set_bt_compare( m_db, m_bt_compare_fcn );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }
    
    template< class RawT >
    int bt_compare_numeric(DB* rawdb, const DBT* a, const DBT* b )
    {
        RawT ai = 0, bi = 0;

        /* * Returns: * < 0 if a < b * = 0 if a = b * > 0 if a > b */
        memcpy(&ai, a->data, sizeof(RawT) );
        memcpy(&bi, b->data, sizeof(RawT) );
        return int(ai - bi);
    }

    Database::bt_compare_fcn  getInt16Compare()
    {
        return bt_compare_numeric< int16_t >;
    }
    Database::bt_compare_fcn  getUInt16Compare()
    {
        return bt_compare_numeric< u_int16_t >;
    }
    Database::bt_compare_fcn  getInt32Compare()
    {
        return bt_compare_numeric< int32_t >;
    }
    Database::bt_compare_fcn  getUInt32Compare()
    {
        return bt_compare_numeric< u_int32_t >;
    }
    Database::bt_compare_fcn  getInt64Compare()
    {
        return bt_compare_numeric< int64_t >;
    }
    Database::bt_compare_fcn  getUInt64Compare()
    {
        return bt_compare_numeric< u_int64_t >;
    }
    Database::bt_compare_fcn  getFloatCompare()
    {
        return bt_compare_numeric< float >;
    }
    Database::bt_compare_fcn  getDoubleCompare()
    {
        return bt_compare_numeric< double >;
    }

    static int bt_compare_cis(DB* rawdb, const DBT* a, const DBT* b )
    {
        const char *p1, *p2;

        /* * Returns: * < 0 if a < b * = 0 if a = b * > 0 if a > b */
        for (p1 = (const char *)a->data, p2 = (const char *)b->data;
             *p1 && *p2 ;
             ++p1, ++p2)
        {
            if (toupper(*p1) != toupper(*p2))
                return ((long)*p1 - (long)*p2);
        }
        if( !*p1 &&  *p2 )
            return 1;
        if(  *p1 && !*p2 )
            return -1;
        return (0); 
    }
    
    Database::bt_compare_fcn  
    getCISCompare()
    {
        return bt_compare_cis;
    }
    
    static int  
    bt_compare_cs(DB* rawdb, const DBT* a, const DBT* b )
    {
        const char *p1, *p2;

        /* * Returns: * < 0 if a < b * = 0 if a = b * > 0 if a > b */
        for (p1 = (const char *)a->data, p2 = (const char *)b->data;
             *p1 && *p2 ;
             ++p1, ++p2)
        {
            if (*p1 != *p2)
                return ((long)*p1 - (long)*p2);
        }
        if( !*p1 &&  *p2 )
            return 1;
        if(  *p1 && !*p2 )
            return -1;
        return (0); 
    }
    Database::bt_compare_fcn  getCSCompare()
    {
        return bt_compare_cs;
    }
    Database::bt_compare_fcn  getBinaryCompare()
    {
        return bt_compare_cs;
    }

    template < class RawT1 >
    int bt_compose_compare_cs(DB* rawdb,
                              const DBT* a, const DBT* b,
                              Database::bt_compare_fcn F1,
                              Database::bt_compare_fcn F2 )
    {
        int rc = F1( rawdb, a, b );
        if( rc != 0 )
            return rc;

        DBT na;
        DBT nb;
        na.data = (void*)((const char*)a->data + sizeof(RawT1));
        nb.data = (void*)((const char*)b->data + sizeof(RawT1));
        na.size = a->size - sizeof(RawT1);
        nb.size = b->size - sizeof(RawT1);
        return F2( rawdb, &na, &nb );
    }
    static int  bt_compose_uint16_and_binary_compare_cs(DB* rawdb, const DBT* a, const DBT* b )
    {
        return bt_compose_compare_cs< u_int16_t >( rawdb, a, b, getUInt16Compare(), getBinaryCompare() );
    }
    Database::bt_compare_fcn  getUInt16ThenBinaryCompare()
    {
        return bt_compose_uint16_and_binary_compare_cs;
    }

    static int bt_compare_reverse( Database::m_bt_compare_functor_t& f, DB* rawdb, const DBT* a, const DBT* b )
    {
        int ret = f( rawdb, a, b );
        return -1*ret;
    }

    Database::m_bt_compare_functor_t
    makeReverseCompare( Database::m_bt_compare_functor_t& f )
    {
        typedef Loki::Functor< int,
            LOKI_TYPELIST_4( Database::m_bt_compare_functor_t&, DB*, const DBT *, const DBT * ) > F_t;
        F_t ret( &bt_compare_reverse );

        return Loki::BindFirst( ret, f );
    }
    
    
    
    static int  
    FullValueSecIdxFunction( DB* db, const DBT* pkey, const DBT* pdata, DBT* skey)
    {
        memset(skey, 0, sizeof(DBT));
        skey->data = pdata->data;
        skey->size = pdata->size;
        return (0);
    }
    Database::sec_idx_callback  getFullValueSecIdx()
    {
        return FullValueSecIdxFunction;
    }

    void
    Database::associate( fh_database secdb,
                         sec_idx_callback f,
                         u_int32_t flags,
                         fh_trans t )
    {
        t = freshenTransaction( t );
        int rc = raw()->associate( raw(), t->raw(), secdb->raw(), f, flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }

    
    std::string&
    Database::pget( const std::string& skey,
                    std::string& pkey,
                    std::string& data,
                    u_int32_t flags,
                    fh_trans t )
    {
        t = freshenTransaction( t );

        DBT secondary_key;
        setup( secondary_key, skey.data(), skey.length() );

        DBT primary_key;
        DBT primary_data;
        memset(&primary_key,  0, sizeof(primary_key));
        memset(&primary_data, 0, sizeof(primary_data));
        
        int rc = raw()->pget( raw(), t->raw(),
                              &secondary_key,
                              &primary_key,
                              &primary_data,
                              flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        pkey.resize( primary_key.size );
        memcpy( (char*)pkey.data(), primary_key.data, primary_key.size );

        data.resize( primary_data.size );
        memcpy( (char*)data.data(), primary_data.data, primary_data.size );

        return data;
    }
    

//     DBTYPE
//     Database::getType()
//     {
//         DBTYPE dbtype = DB_UNKNOWN;
//         int rc = raw()->get_type( raw(), &dbtype );
//         if( rc != 0 )
//             ThrowFromDB4Error( rc );

//         return dbtype;
//     }
    
    bool
    Database::isRecNum()
    {
        DBTYPE dbtype = DB_UNKNOWN;
        int rc = raw()->get_type( raw(), &dbtype );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        return dbtype == DB_RECNO;
    }
    bool
    Database::isQueue()
    {
        DBTYPE dbtype = DB_UNKNOWN;
        int rc = raw()->get_type( raw(), &dbtype );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        return dbtype == DB_QUEUE;
    }
    

    
    void
    Database::impl_open( const std::string& filename,
                         const std::string& database,
                         DBTYPE type,
                         u_int32_t flags,
                         int mode,
                         fh_trans t )
    {
        t = freshenTransaction( t );
//        cerr << "Database::impl_open() t:" << (void*)t->raw() << endl;
//        cerr << "Database::impl_open() path is " << Ferris::CleanupURL(filename).c_str() << endl;
        
        int rc = m_db->open( m_db,
                             t->raw(),
                             Ferris::CleanupURL(filename).c_str(),
                             database.empty() ? 0 : database.c_str(),
                             type,
                             flags,
                             mode );

        if( rc != 0 )
        {
            stringstream dss;
            dss << "The database at filename:" << filename
                << " database:" << database;
            string base = dss.str();
            
            switch( rc )
            {
            case DB_OLD_VERSION: 
                throw dbOldVersionException( base + " cannot be opened without being first upgraded.", rc );
            case EEXIST:
                throw dbExists( base + "DB_CREATE and DB_EXCL were specified and the file exists", rc );
            case EINVAL:
                throw dbInvalidParams( base + "An invalid flag value or parameter was specified", rc );
            case ENOENT:
                throw dbNonExist( base + "A nonexistent database file was specified", rc );
            }
            ThrowFromDB4Error( rc );
        }

        if( t )
            setImplicitTransaction( t );
    }

    std::streamsize
    Database::impl_getValueSize( const char* key, int keylen, u_int32_t flags, fh_trans t )
    {
        t = freshenTransaction( t );
        
        DBT k;
        DBT dt;
        setup( k,  key, keylen );
        setup( dt, 0,   0 );

        int rc = m_db->get( m_db, t->raw(), &k, &dt, flags );

        if( rc == ENOMEM || rc == DB_BUFFER_SMALL )
        {
            return dt.size;
        }
        if( rc != 0 )
        {
            ThrowFromDB4Error( rc );
        }
        return dt.size;
    }
    
    
    fh_coredatachunk
    Database::impl_getChunk( const char* key, int keylen, u_int32_t flags, fh_trans t )
    {
        t = freshenTransaction( t );

        DBT k;
        setup( k, key, keylen );
        fh_coredatachunk d = new CoreDataChunk( 4096 );
        
        int rc = ENOMEM;

        while( true )
        {
            DBT dt;
            setup( dt, d->data(), d->size() );
            dt.size = 0;
            
            rc = m_db->get( m_db, t->raw(), &k, &dt, flags );

//            cerr << "Database::impl_getChunk() rc:" << rc << endl;
            
            if( rc == 0 )
            {
//                cerr << "Database::impl_getChunk() valid size:" << dt.size << endl;
                break;
            }
            
            if( rc == ENOMEM || rc == DB_BUFFER_SMALL )
            {
//                cerr << "no mem sz:" << d->tostring().size() << endl;
                d->DoubleSize();
                continue;
            }
            
            ThrowFromDB4Error( rc );
        }
        return d;
    }

    void
    Database::impl_getChunk( const char* key, int keylen, char* data, int datalen,
                             u_int32_t flags, fh_trans t )
    {
        t = freshenTransaction( t );

        DBT k;
        setup( k, key, keylen );
        DBT dt;
        setup( dt, data, datalen );
        
        int rc = ENOMEM;

        rc = m_db->get( m_db, t->raw(), &k, &dt, flags );

        if( rc != 0 )
        {
            ThrowFromDB4Error( rc );
        }
    }

    db_recno_t
    Database::impl_append( const char* data,
                      std::streamsize datalen,
                      fh_trans t )
    {
        t = freshenTransaction( t );

        u_int32_t flags  = DB_APPEND;
        db_recno_t recno = 0;
        if( !isRecNum() && !isQueue() )
            recno = size() + 1;
        
        DBT k;
        DBT d;
        setup( k, (const char*)&recno, sizeof(recno) );
        setup( d, data, datalen );
        
        int rc = m_db->put( m_db, t->raw(), &k, &d, flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        return recno;
    }

    void
    Database::set( db_recno_t recno,
                   const std::string& value,
                   u_int32_t flags,
                   fh_trans t )
    {
        impl_setChunk( (const char*)&recno, sizeof(recno),
                       value.data(), value.length(), flags, t );
    }


    std::string&
    Database::get( const std::string& key,
                   std::string& data,
                   u_int32_t flags,
                   fh_trans t )
    {
//        std::cerr << "get() key:" << key << ":" << endl;
        std::streamsize sz = impl_getValueSize( key.data(), key.length(), flags, t );
//        std::cerr << "get() sz:" << sz << std::endl;
        
        data.resize( sz );
        impl_getChunk(  key.data(), key.length(),
                        (char*)data.data(), data.length(), flags, t );
        return data;
    }

    const std::string&
    Database::getWithDefault( const std::string& key,
                              std::string& data,
                              const std::string& def,
                              u_int32_t flags,
                              fh_trans t )
    {
        try
        {
//            cerr << "Database::getWithDefault data:" << data << " def:" << def << endl;
            get( key, data, flags, t );
//            cerr << "Database::getWithDefault(have v) data:" << data << " def:" << def << endl;
            return data;
        }
        catch( exception& e )
        {
        }
//        cerr << "Database::getWithDefault(ret def) data:" << data << " def:" << def << endl;
        data = def;
        return def;
    }
    
    
    

    std::string
    Database::get( db_recno_t recno, fh_trans t )
    {
        t = freshenTransaction( t );

        u_int32_t flags = 0;
        DBT k;
        DBT d;
        setup( k, (const char*)&recno, sizeof(recno) );
        memset(&d, 0, sizeof(d));

        int rc = m_db->get( m_db, t->raw(), &k, &d, flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        string ret;
        ret.resize( d.size );
        memcpy( (char*)ret.data(), d.data, d.size );
        return ret;
    }
    
    
    
    void
    Database::impl_setChunk( const char* key,  std::streamsize keylen,
                             const char* data, std::streamsize datalen,
                             u_int32_t flags,
                             fh_trans t )
    {
        t = freshenTransaction( t );

        DBT k;
        DBT d;
        setup( k, key,  keylen  );
        setup( d, data, datalen );

        int rc = m_db->put( m_db, t->raw(), &k, &d, flags );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
        
        {
            string k( key, keylen );
            string d( data, datalen );
//            cerr << "Database::impl_setChunk() k:" << k << " d:" << d << " tid:" << t->id() << endl;
        }
    }

    Ferris::fh_istream
    Database::impl_getIStream( const char* key,  std::streamsize keylen, u_int32_t flags, fh_trans t )
    {
        t = freshenTransaction( t );
        
        iterator iter = impl_find( key, keylen, t, true );
//         if( iter == end(t) )
//             throw dbNonExist( "key not found" );
        return iter.getIStream( flags );
    }
    
    Ferris::fh_iostream
    Database::impl_getIOStream( const char* key,  std::streamsize keylen, u_int32_t flags, fh_trans t )
    {
        t = freshenTransaction( t );

        iterator iter = impl_find( key, keylen, t, true );
//         if( iter == end(t) )
//             throw dbNonExist( "key not found" );

        return iter.getIOStream( flags );
    }
    
    
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/

    static int IteratorCount = 0;
    static int CursorCount = 0;
    
    static DBC*  makeCursor( DB* db, fh_trans t )
    {
        DBC*  cursorp = 0;
        int rc = db->cursor( db, t->raw(), &cursorp, 0 );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        ++CursorCount;
//        cerr << "makeCursor() CursorCount:" << CursorCount << endl;
        return cursorp;
    }

    static void  freeCursor( DBC*& c )
    {
        --CursorCount;
        int rc = c->c_close( c );
        c = 0;
//        cerr << "freeCursor() CursorCount:" << CursorCount << endl;
            
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }
    
    static DBC*  cloneCursor( DBC* c, u_int32_t flags = DB_POSITION )
    {
        DBC* ret = 0;
        if( c )
        {
            int rc = c->c_dup( c, &ret, flags );
            if( rc != 0 )
            {
                ThrowFromDB4Error( rc );
            }
            ++CursorCount;
//            cerr << "cloneCursor() CursorCount:" << CursorCount << endl;
        }
        return ret;
    }
    

    
    Database::iterator
    Database::begin( fh_trans t )
    {
        t = freshenTransaction( t );

        DBC*  cursorp = makeCursor( m_db, t );
        Database::iterator ret = DatabaseIterator::Create( this, t, cursorp );
        
        int rc = ret.moveCursor( DB_FIRST );
        if( rc == DB_NOTFOUND )
        {
            return end();
        }
        if( rc == DB_NOTFOUND || rc == DB_KEYEMPTY )
        {
            ThrowFromDB4Error( rc );
        }
        
        return ret;
    }
    
    Database::iterator
    Database::end( fh_trans t )
    {
        t = freshenTransaction( t );
        return DatabaseIterator::Create( this, t, 0 );
    }
    
    Database::reverse_iterator
    Database::rbegin( fh_trans t )
    {
        t = freshenTransaction( t );
        return reverse_iterator(end( t ));
    }
    
    Database::reverse_iterator
    Database::rend( fh_trans t )
    {
        t = freshenTransaction( t );
        return reverse_iterator(begin( t ));
    }

    Database::iterator
    Database::impl_find( const char* key,
                         std::streamsize keylen,
                         fh_trans t,
                         bool throw_for_err )
    {
        t = freshenTransaction( t );
        DBC*  cursorp = makeCursor( m_db, t );

        DBT k;
        DBT d;
        setup( k, key,  keylen  );
        memset(&d, 0, sizeof(d));
        int rc = cursorp->c_get( cursorp, &k, &d, DB_SET );
//        free( d.data );
        if( rc == DB_NOTFOUND || rc == DB_KEYEMPTY )
        {
            freeCursor( cursorp );
            if( throw_for_err )
                ThrowFromDB4Error( rc );
            return end(t);
        }
        
        return DatabaseIterator::Create( this, t, cursorp );
    }
    
    
    Database::iterator
    Database::find( const char* key, std::streamsize keylen, fh_trans t )
    {
        return impl_find( key, keylen, t, false );
    }

    Database::iterator
    Database::find( db_recno_t recno, fh_trans t )
    {
        t = freshenTransaction( t );
        DBC*  cursorp = makeCursor( m_db, t );

        DBT k;
        DBT d;
        setup( k, (const char*)&recno, sizeof(recno) );
        memset(&d, 0, sizeof(d));
        int rc = cursorp->c_get( cursorp, &k, &d, DB_SET );
        if( rc == DB_NOTFOUND || rc == DB_KEYEMPTY )
        {
            freeCursor( cursorp );
            return end(t);
        }
        
        return DatabaseIterator::Create( this, t, cursorp );
    }
    
    Database::iterator
    Database::find_partial( const char* partialkey, size_type partialkeylen, fh_trans t )
    {
        t = freshenTransaction( t );
        DBC*  cursorp = makeCursor( m_db, t );

        DBT k;
        DBT d;
        setup( k, partialkey,  partialkeylen  );
        memset(&d, 0, sizeof(d));
        int rc = cursorp->c_get( cursorp, &k, &d, DB_SET_RANGE );

        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
        {
            freeCursor( cursorp );
            return end(t);
        }

        size_type DesiredSize = k.size;
        size_type tmpsz = max( DesiredSize, partialkeylen ) + 1;
//          cerr << "Database::find_partial(1) rc:" << rc << " DesiredSize:" << DesiredSize
//               << " partialkeylen:" << partialkeylen
//               << " tmpsz:" << tmpsz
//               << endl;
        string tmp;
        tmp.resize( tmpsz );
        memcpy( (char*)tmp.data(), partialkey, partialkeylen );
        setup( k, (char*)tmp.data(),  partialkeylen  );
        k.ulen = DesiredSize;
        rc = cursorp->c_get( cursorp, &k, &d, DB_SET_RANGE );

//        cerr << "Database::find_partial(2) rc:" << rc << endl;
        
//        free( d.data );
        if( rc == DB_NOTFOUND || rc == DB_KEYEMPTY )
        {
            freeCursor( cursorp );
            return end(t);
        }
        
        return DatabaseIterator::Create( this, t, cursorp );
    }
    
    Database::iterator
    Database::lower_bound( const char* key, std::streamsize keylen, fh_trans t )
    {
        t = freshenTransaction( t );
        Database::iterator ret = find( key, keylen, t );

        if( ret == end() && m_bt_compare_fcn )
        {
            
            DBT k;
            setup( k, key,  keylen  );

            Database::iterator begin = this->begin();
            if( begin != end() )
            {
                string bkstr;
                begin.getKey( bkstr );
                DBT bk;
                setup( bk, bkstr.data(),  bkstr.length() );

                if( m_bt_compare_fcn( raw(), &k, &bk ) < 0 )
                {
                    return begin;
                }
            }
        }
        
        return ret;
    }
    
    Database::iterator
    Database::upper_bound( const char* key, std::streamsize keylen, fh_trans t )
    {
        t = freshenTransaction( t );
        Database::iterator ret = lower_bound( key, keylen, t );
        
        int rc = 0;
        while( rc != DB_NOTFOUND )
        {
            rc = ret.moveCursor( DB_NEXT_DUP );
        }

        ++ret;
        return ret;
    }

    std::pair<Database::iterator, Database::iterator>
    Database::equal_range( const char* key, size_type keylen, fh_trans t )
    {
        return std::make_pair( lower_bound( key, keylen, t ),
                               upper_bound( key, keylen, t ) );
    }
    std::pair<Database::iterator, Database::iterator>
    Database::equal_range(const key_type& k, fh_trans t )
    {
        return std::make_pair( lower_bound( k, t ), upper_bound( k, t ) );
    }
    std::pair<Database::iterator, Database::iterator>
    Database::equal_range_partial( const char* key, size_type keylen, fh_trans t )
    {
        return std::make_pair( lower_bound_partial( key, keylen, t ),
                               upper_bound_partial( key, keylen, t ) );
    }
    std::pair<Database::iterator, Database::iterator>
    Database::equal_range_partial(const key_type& k, fh_trans t )
    {
//        std::cerr << "Database::equal_range_partial( getting lb ) k:" << k << endl;
        iterator first  = lower_bound_partial( k, t );

//        std::cerr << "Database::equal_range_partial( getting ub ) k:" << k << endl;
        iterator second = upper_bound_partial( k, t );
        
        return std::make_pair( first, second );
    }
    

    static string  
    AdvanceFunctorBytes( fh_database, const std::string& k )
    {
        string ret = k;
        char literal_onechar = (char)1;
        
        if( ret.empty() )
        {
            ret = literal_onechar;
            return ret;
        }
        
        for( int idx = ret.length()-1; idx; --idx )
        {
            if( (unsigned char)ret[ idx ] < 255 )
            {
                ret[ idx ] = ret[ idx ]+1;
                return ret;
            }
        }

        ret = ret + literal_onechar;
        return ret;
    }
    Database::advance_functor_t
    Database::makeAdvanceFunctorBytes()
    {
        return AdvanceFunctorBytes;
    }
    

    static string AdvanceFunctorNumeric( fh_database, const std::string& k )
    {
        typedef long long int64;
        int64 n = Ferris::toType<int64>( k );
        ++n;
//        cerr << "AdvanceFunctorNumeric() k:" << k << " n:" << n << endl;
        return Ferris::tostr(n);
    }
    Database::advance_functor_t
    Database::makeAdvanceFunctorNumeric()
    {
        return AdvanceFunctorNumeric;
    }
    
    
    
    Database::iterator
    Database::upper_bound_partial( const char* key, std::streamsize keylen, fh_trans t )
    {
        t = freshenTransaction( t );

        string tmp( key, keylen );
//        cerr << "Database::upper_bound_partial tmp1:" << tmp << endl;
        fh_database spthis = this;
        tmp = getAdvanceFunctor()( spthis, tmp );
//        cerr << "Database::upper_bound_partial tmp2:" << tmp << endl;

        iterator ret = lower_bound_partial( tmp, t );
        
//         Database::iterator ret = lower_bound_partial( key, keylen, t );
        
//         int rc = 0;
//         while( rc != DB_NOTFOUND )
//         {
//             rc = ret.moveCursor( DB_NEXT_DUP );
//         }

//         ++ret;

//         if( ret != end() )
//             cerr << "Database::upper_bound_partial() ret:" << ret->first << " " << ret->second << endl;
        return ret;
    }
    
    
    
    Database::iterator
    Database::insert( const char* key,  std::streamsize keylen,
                      const char* data, std::streamsize datalen,
                      fh_trans t )
    {
        t = freshenTransaction( t );
        impl_setChunk( key, keylen, data, datalen, 0, t );
        return find( key, keylen, t );
    }

    DatabaseMutableValueRef&
    DatabaseMutableValueRef::operator=( const std::string& v )
    {
//        cerr << "DatabaseMutableValueRef::operator=()" << endl;

        m_iter.setValue( v );
        
        
//         string k;
//         m_iter.getKey( k );
// //        cerr << "setting k:" << k << " to value:" << v << endl;
        
//         m_db->set( k, v, 0, m_trans );
//         {
//             string v;
//             m_db->get( k,v, 0, m_trans );
// //            cerr << "setting k:" << k << " -- read back:" << v << endl;
//         }
        
        return *this;
    }

    bool
    DatabaseMutableValueRef::operator==( const std::string& v )
    {
        string dbv;
        string k;
        m_iter.getKey( k );
        m_db->get( k, dbv, 0, m_trans );
        return v == dbv;
    }
    
    bool
    DatabaseMutableValueRef::operator!=( const std::string& v )
    {
        return !operator==( v );
    }
    

    
    DatabaseMutableValueRef
    Database::operator[]( const std::string& k )
    {
//        cerr << "Database::operator[] making lower bound call k:" << k << endl;
        fh_trans t = m_implicitTransaction;
        
        iterator iter = lower_bound( k, t );
//         cerr << "Database::operator[] iter==end:" << (iter==end(t))
//              << " k:" << k << endl;
        
        if( iter == end(t) || k > iter->first )
        {
            iter = insert( k, t );
        }

//        cerr << "+++ Database::operator[] end" << endl;
        return DatabaseMutableValueRef( this, iter, t );
    }

    DatabaseMutableValueRef
    Database::operator[]( long k )
    {
        const int bufsz = 30;
        char buf[ bufsz ];
        snprintf( buf, bufsz, "%ld", k );
        return operator[]( buf );
    }
    
    

    Database::size_type
    Database::size() const
    {
        int rc = 0;
        DBTYPE type;

        rc = m_db->get_type( m_db, &type);
        if( rc != 0 )
            ThrowFromDB4Error( rc );
        
        void* sp = 0;
        rc = m_db->stat( m_db, 0, &sp, DB_FAST_STAT );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        std::streamsize ret = 0;
        switch( type )
        {
        case DB_RECNO:
        case DB_BTREE: 
        {
            DB_BTREE_STAT* bts = (DB_BTREE_STAT*)sp;
            ret = bts->bt_nkeys;
            break;
        }
        case DB_HASH:
        {
            DB_HASH_STAT* hts = (DB_HASH_STAT*)sp;
            ret = hts->hash_nkeys;
            break;
        }
        case DB_QUEUE:
        {
            DB_QUEUE_STAT* qs = (DB_QUEUE_STAT*)sp;
            ret = qs->qs_nkeys;
            break;
        }
        default:
            cerr << "Unknown database type! please update libstldb4" << endl;
            throw dbException("Unknown database type! please update libstldb4", rc );
        }
        free( sp );
        return ret;
    }

    Database::size_type
    Database::max_size() const
    {
        // FIXME: should be based on page size as in
        // file:///usr/share/doc/db4-devel-4.0.14/ref/am_misc/dbsizes.html
        std::streamsize ret = 2;
        ret = 2^41;
        return ret;
    }
    

    bool
    Database::empty() const
    {
        return size() == 0;
    }
    
    void
    Database::erase( const char* key, size_type keylen, fh_trans t )
    {
        t = freshenTransaction( t );
        
        DBT k;
        setup( k,  key, keylen );
        int rc = m_db->del( m_db, t->raw(), &k, 0 );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }

    void
    Database::erase( iterator pos, fh_trans t )
    {
        t = freshenTransaction( t );

        int rc = pos.m_cursor->c_del( pos.m_cursor, 0 );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }
    
    
    Database::size_type
    Database::count( const char* key, size_type keylen, fh_trans t )
    {
        t = freshenTransaction( t );
        
        pair< iterator, iterator > eqclass = equal_range( key, keylen, t );
        return distance( eqclass.first, eqclass.second );
    }
    
    
    Database::iterator
    Database::compact( iterator b, iterator e, fh_trans t )
    {
        DBT start_backing;
        DBT stop_backing;
        DBT *start = 0;
        DBT *stop  = 0;

        t = freshenTransaction( t );
        DB_COMPACT* args = 0;
        u_int32_t flags = 0;
        
        if( b != e )
        {
            start = toKDBT( start_backing, b );
            stop  = toKDBT( stop_backing,  e );
        }

        DBT last_location;
        int rc = raw()->compact( raw(), t->raw(), start, stop, args, flags, &last_location );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        iterator ret = find( (const char*)last_location.data, last_location.size, t );
        return ret;
    }
    

    DBTYPE
    Database::get_type()
    {
        DBTYPE dbtype = DB_UNKNOWN;
        int rc = raw()->get_type( raw(), &dbtype );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
        return dbtype;
    }
    
    bool
    Database::isSorted()
    {
        return isBTree();
    }
    
    
    bool
    Database::isBTree()
    {
        DBTYPE dbtype = DB_UNKNOWN;
        int rc = raw()->get_type( raw(), &dbtype );
        if( rc != 0 )
            ThrowFromDB4Error( rc );

        return dbtype == DB_BTREE;
    }

    
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/

    
    DatabaseIterator DatabaseIterator::Create( fh_database db, fh_trans t, DBC* cursor )
    {
        DatabaseIterator ret( db, t, cursor );
        return ret;
    }

    DatabaseIterator::DatabaseIterator( fh_database m_db, fh_trans t, DBC* m_cursor )
        :
        m_db( m_db ),
        m_trans( t ),
        m_cursor( m_cursor ),
        m_isCacheValid( false )
    {
        ++IteratorCount;
//        cerr << "DatabaseIterator::DatabaseIterator() IteratorCount:" << IteratorCount << endl;
    }
    
    DatabaseIterator::~DatabaseIterator()
    {
        --IteratorCount;
//         cerr << "DatabaseIterator::~DatabaseIterator() IteratorCount:" << IteratorCount
//              << " m_cur:" << Ferris::toVoid( m_cursor )
//              << endl;
        setCursorToEnd();
    }
    
    DatabaseIterator::DatabaseIterator( const DatabaseIterator& ci )
        :
        m_db(      ci.m_db ),
        m_trans(   ci.m_trans ),
        m_cursor(  cloneCursor(ci.m_cursor) ),
        m_isCacheValid( false )
    {
        ++IteratorCount;
//        cerr << "DatabaseIterator::DatabaseIterator(copy) IteratorCount:" << IteratorCount << endl;
    }
    
    DatabaseIterator& DatabaseIterator::operator=( const DatabaseIterator& ci )
    {
        ++IteratorCount;
//        cerr << "DatabaseIterator::DatabaseIterator(assign) IteratorCount:" << IteratorCount << endl;
        if( this != &ci )
        {
            m_db     = ci.m_db;
            m_trans  = ci.m_trans;
            setCursorToEnd();
            m_cursor = cloneCursor( ci.m_cursor );
        }
        return *this;
    }

    void
    DatabaseIterator::InvalidateCache() const
    {
        m_isCacheValid = false;
    }
    
    
    void
    DatabaseIterator::UpdateCache() const
    {
        if( m_isCacheValid )
            return;

        db_recno_t recno = 0;
        DBT k;
        DBT d;
        memset(&k, 0, sizeof(k));
        setup( d, 0, 0 );

        u_int32_t getflags = DB_CURRENT;
        int rc = m_cursor->c_get( m_cursor, &k, &d, getflags );
        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
        {
            free( k.data );
            ThrowFromDB4Error( rc );
        }
        
        if( m_db->isRecNum() )
        {
            recno = *((db_recno_t*)k.data);
//            m_cache.first = Ferris::tostr(recno);
        }
        else
        {
            m_cache.first.resize( k.size );
            memcpy( (char*)m_cache.first.data(), k.data, k.size );
            setup( k, m_cache.first.data(), m_cache.first.size() );
        }
        
        m_cache.second.resize( d.size );
        setup( d, m_cache.second.data(), m_cache.second.size() );


        rc = m_cursor->c_get( m_cursor, &k, &d, DB_CURRENT );

//        cerr << "deref() value.sz:" << d.size << " rc:" << rc << endl;

        if( rc != 0 )
            ThrowFromDB4Error( rc );

//         if( m_db->isRecNum() )
//         {
//             int rc = m_cursor->c_get( m_cursor, &k, &d, getflags | DB_GET_RECNO );
//             recno = *((db_recno_t*)k.data);

//             stringstream ss;
//             ss << recno;
//             m_cache.first = ss.str();
//         }
        
        if( recno )
        {
            const int bufsz = 30;
            char buf[ bufsz + 1 ];
            snprintf( buf, bufsz, "%ld", (long)recno );
            
            m_cache.first = buf;
//            fprintf(stderr,"recno:%ld\n",recno );
//             stringstream ss;
//             ss << recno;
//             m_cache.first = ss.str();
//             cerr << "RECNO1:" << m_cache.first << endl;
//             cerr << "RECNO2:" << recno << endl;
        }
    }
    
    const std::string&
    DatabaseIterator::getKey( std::string& key ) const
    {
        DBT k;
        DBT d;
        setup( k, 0, 0 );
        setup( d, 0, 0 );

//        cerr << "DatabaseIterator::getKey() m_cursor:" << Ferris::toVoid(m_cursor) << endl;
        
        int rc = m_cursor->c_get( m_cursor, &k, &d, DB_CURRENT );
        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
            ThrowFromDB4Error( rc );

        key.resize( k.size );
        setup( k, key.data(), key.size() );

        rc = m_cursor->c_get( m_cursor, &k, &d, DB_CURRENT );
        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
        {
//            cerr << "getKey() rc:" << rc << endl;
            ThrowFromDB4Error( rc );
        }

//        cerr << "getKey() returning:" << key << endl;
        return key;
    }

    const std::string&
    DatabaseIterator::getPrimaryKey( std::string& primarykey, bool /* isRecno */ ) const
    {
        DBT k;
        DBT pk;
        DBT d;
        memset( &k,  0, sizeof(k));
        memset( &pk, 0, sizeof(pk));
        memset( &d,  0, sizeof(d));

        int rc = m_cursor->c_pget( m_cursor, &k, &pk, &d, DB_CURRENT );
        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
        {
//            cerr << "getKey() rc:" << rc << endl;
            ThrowFromDB4Error( rc );
        }

//         if( isRecno )
//         {
//             db_recno_t rn = 0;
//             rn = *((db_recno_t*)pk.data);
//             primarykey = Ferris::tostr( (long)rn );
//             return primarykey;
//         }
        
        primarykey.resize( pk.size );
        memcpy( (char*)primarykey.data(), pk.data, pk.size );
        return primarykey;
    }
    

    const std::string&
    DatabaseIterator::getPrimaryKey( std::string& primarykey ) const
    {
        return getPrimaryKey( primarykey, false );
        
        
//         DBT k;
//         DBT pk;
//         DBT d;
//         setup( k, 0, 0 );
//         setup( pk, 0, 0 );
//         setup( d, 0, 0 );

// //        cerr << "DatabaseIterator::getKey() m_cursor:" << Ferris::toVoid(m_cursor) << endl;
        
//         int rc = m_cursor->c_get( m_cursor, &k, &d, DB_CURRENT );
//         if( rc != 0 && rc != ENOMEM )
//             ThrowFromDB4Error( rc );

//         string key;
//         key.resize( k.size );
//         setup( k, key.data(), key.size() );

//         rc = m_cursor->c_pget( m_cursor, &k, &pk, &d, DB_CURRENT );
//         if( rc != 0 && rc != ENOMEM )
//         {
// //            cerr << "getKey() rc:" << rc << endl;
//             ThrowFromDB4Error( rc );
//         }

//         primarykey.resize( pk.size );
//         memcpy( (char*)primarykey.data(), pk.data, pk.size );
        
// //        cerr << "getKey() returning:" << key << endl;
//         return primarykey;
    }

    db_recno_t
    DatabaseIterator::getRecNumber() const
    {
        db_recno_t rn = 0;
        std::string s;
        getPrimaryKey( s, false );
        rn = *((db_recno_t*)s.data());
        return rn;
        
//         std::string s;
//         getPrimaryKey( s, true );
//         db_recno_t ret = Ferris::toType<db_recno_t>( s );
//         cerr << "DatabaseIterator::getRecNumber() s:" << s << endl;
//         stringstream ss;
//         ss << s;
//         ss >> ret;
// //        cerr << "DatabaseIterator::getRecNumber() ret:" << ret << endl;
//         return ret;
    }
    
    const std::string&
    DatabaseIterator::getValue( std::string& v ) const
    {
        UpdateCache();
        v = m_cache.second;
        return v;
    }

    std::streamsize
    DatabaseIterator::getValueSize() const
    {
        DBT k;
        DBT d;
        memset(&k, 0, sizeof(k));
        setup( d, 0, 0 );
        
        int rc = m_cursor->c_get( m_cursor, &k, &d, DB_CURRENT );
//        free( k.data );
        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
        {
            ThrowFromDB4Error( rc );
        }
        return d.size;
    }

    void
    DatabaseIterator::setValue( const char* data, std::streamsize datalen )
    {
        DBT k;
        DBT d;
        memset(&k, 0, sizeof(k));
        setup( d, 0, 0 );

        int rc = m_cursor->c_get( m_cursor, &k, &d, DB_CURRENT );
        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
        {
            ThrowFromDB4Error( rc );
        }

        string keystring;
        keystring.resize( k.size );
        
        memcpy( (char*)keystring.data(), k.data, k.size );
        setup( k, keystring.data(), keystring.size() );

        setup( d, data, datalen );
        rc = m_cursor->c_put( m_cursor, &k, &d, DB_CURRENT );
        if( rc != 0 )
            ThrowFromDB4Error( rc );
    }
    

    template <class CharT>
    class FERRISEXP_DLLLOCAL db4_basic_streambuf
        :
        public Ferris::ferris_basic_streambuf< CharT >
    {
        typedef db4_basic_streambuf<CharT> _Self;
        typedef Ferris::ferris_basic_streambuf< CharT > _Base;
        
        fh_database         m_db;
        Database::iterator  m_iter;
        typedef typename STLdb4::db4_basic_streambuf<CharT>::pos_type pos_type;
        typedef typename STLdb4::db4_basic_streambuf<CharT>::char_type char_type;
        typedef typename STLdb4::db4_basic_streambuf<CharT>::off_type off_type;
        typedef typename STLdb4::db4_basic_streambuf<CharT>::seekd_t seekd_t;
        pos_type            m_goffset;
        pos_type            m_poffset;
        
    public:

        db4_basic_streambuf( fh_database db, Database::iterator iter )
            :
            m_db( db ),
            m_iter( iter ),
            m_goffset( 0 ),
            m_poffset( 0 )
            {
//                cerr << "db4_streambuf() this:" << (void*)this << endl;
                
                m_iter.debug();
                typedef typename _Base::write_out_given_data_functor_t f_t;
                f_t f = f_t( this, &_Self::my_write_out_given_data );
                _Base::setWriteOutGivenDataFunctor( f );
            }
        virtual ~db4_basic_streambuf()
            {
//                cerr << "~db4_streambuf()" << endl;
                this->ensureMode( _Base::mode_mute );
            }
        
    protected:

        /**
         * This is the only methods that really needs to be here. It gets
         * up to maxsz data into buffer and returns how much data was really
         * read. Return 0 for a failure, you must read atleast one byte.
         */
        virtual int make_new_data_avail( char_type* buffer, std::streamsize maxsz )
            {
                std::streamsize bread = m_iter.partialRead( buffer, maxsz, m_goffset );
                m_goffset += bread;
                return bread;
            }

        /**
         * Write out the data starting at buffer of length sz to the "external"
         * device.
         * 
         * return -1 for error or 0 for success
         */
        int my_write_out_given_data( const char_type* buffer, std::streamsize sz )
            {
//                cerr << "write_out_given_data() sz:" << sz << " this:" << (void*)this << endl;
                m_iter.debug();
                std::streamsize bwrite = m_iter.partialWrite( buffer, sz, m_poffset );
                m_poffset += bwrite;
                return bwrite;
            }

        pos_type clamp( pos_type p )
            {
                pos_type min = 0;
                pos_type max = m_iter.getValueSize();
                
                if( p < min ) p = min;
                if( p > max ) p = max;

                return p;
            }
        
        
        virtual pos_type
        seekoff(off_type offset, seekd_t d, std::ios_base::openmode m)
            {
                if( m & ios_base::in )
                {
                    if( d == ios_base::beg )
                        m_goffset = offset;
                    else if( d == ios_base::cur )
                        m_goffset += offset;
                    else if( d == ios_base::end )
                        m_goffset = m_iter.getValueSize() + offset;

                    m_goffset = clamp( m_goffset );
                }
                if( m & ios_base::out )
                {
                    if( d == ios_base::beg )
                        m_poffset = offset;
                    else if( d == ios_base::cur )
                        m_poffset += offset;
                    else if( d == ios_base::end )
                        m_poffset = m_iter.getValueSize() + offset;

                    m_poffset = clamp( m_poffset );
                    return this->have_been_seeked( m_poffset );
                }
                return this->have_been_seeked( m_goffset );
            }
        
        virtual pos_type
        seekpos(pos_type pos, std::ios_base::openmode m)
            {
                if( m & ios_base::in )
                {
                    m_goffset = clamp( pos );
                }
                if( m & ios_base::out )
                {
                    m_poffset = clamp( pos );
                    return this->have_been_seeked( m_poffset );
                }

                return this->have_been_seeked( m_goffset );
            }
        
    };

    Ferris::fh_istream
    DatabaseIterator::getIStream( u_int32_t flags )
    {
        Ferris::ferris_basic_streambuf<char>* streambuf
            = new db4_basic_streambuf<char>( m_db, *this );
        Ferris::fh_istream ss( streambuf );
        return ss;
    }
    
    Ferris::fh_iostream
    DatabaseIterator::getIOStream( u_int32_t flags )
    {
        Ferris::ferris_basic_streambuf<char>* streambuf
            = new db4_basic_streambuf<char>( m_db, *this );
        Ferris::fh_iostream ss( streambuf );
        return ss;
    }

    Ferris::fh_iostream
    DatabaseIterator::getIOStream( Ferris::ferris_ios::openmode m, u_int32_t flags )
    {
        if( m & ios::trunc )
        {
            setValue("");
        }
        Ferris::fh_iostream ret = getIOStream( flags );
        if( m & ios::ate )
        {
            ret.seekg(0, ios::end);
            ret.seekp(0, ios::end);
        }
        return ret;
    }

    
    DatabaseIterator::reference
    DatabaseIterator::operator*() const
    {
        UpdateCache();
        return m_cache;
    }
    
    DatabaseIterator::pointer
    DatabaseIterator::operator->() const
    {
        UpdateCache();
        return &m_cache;
    }

    int
    DatabaseIterator::moveCursorNextDup()
    {
        int rc = 0;
        try
        {
            rc = moveCursor( DB_NEXT_DUP );
        }
        catch( exception& e )
        {
            setCursorToEnd();
        }
        return rc;
    }
    
    int
    DatabaseIterator::moveCursor( u_int32_t flags )
    {
        InvalidateCache();

        /**
         * end() is one past the end of the database. we simulate this will
         * a NULL m_cursor and thus need to move the the DB_LAST item when
         * end()--; is performed
         */
        if( !m_cursor && (flags & DB_PREV) )
        {
            m_cursor = makeCursor( m_db->raw(), m_trans );
            flags = DB_LAST;
        }
        
        DBT k;
        DBT d;
        memset(&k, 0, sizeof(k));
        memset(&d, 0, sizeof(d));
        
        int rc = m_cursor->c_get( m_cursor, &k, &d, flags );
//         free( k.data );
//         free( d.data );

        if( rc != 0 && rc != ENOMEM && rc != DB_NOTFOUND )
            ThrowFromDB4Error( rc );

        if( flags & DB_NEXT_DUP && rc == DB_NOTFOUND )
        {
            setCursorToEnd();
        }
        
        return rc;
    }
    
    void
    DatabaseIterator::setCursorToEnd()
    {
        InvalidateCache();

        if( m_cursor )
        {
            freeCursor( m_cursor );
        }
        m_cursor = 0;
    }
    
    DatabaseIterator& DatabaseIterator::operator++()
    {
        if( DB_NOTFOUND == moveCursor( DB_NEXT ) )
        {
            setCursorToEnd();
        }
        return *this;
    }
    
    DatabaseIterator
    DatabaseIterator::operator++(int)
    {
        DatabaseIterator tmp( *this );
        operator++();
        return tmp;
    }
    
    DatabaseIterator&
    DatabaseIterator::operator--()
    {
//        cerr << "DatabaseIterator::operator--()" << endl;
        int rc = moveCursor( DB_PREV );
        if( rc == DB_NOTFOUND )
        {
            throw dbNonExist( "there is no previous record", rc );
        }
        return *this;
    }
    
    DatabaseIterator
    DatabaseIterator::operator--(int)
    {
//        cerr << "DatabaseIterator::operator--(int)" << endl;
        DatabaseIterator tmp( *this );
        operator--();
        return tmp;
    }

    bool  operator==(const DatabaseIterator& x, const DatabaseIterator& y)
    {
        // both at end()
        if( x.m_db == y.m_db && !x.m_cursor && !y.m_cursor )
            return true;
        
        if( x.m_db == y.m_db && x.m_cursor && y.m_cursor )
        {
//             string xk;
//             string yk;
//             x.getKey( xk );
//             y.getKey( yk );
//             return xk == yk;

            return x->first == y->first && x->second == y->second;
        }
        return false;
    }

    bool  operator!=(const DatabaseIterator& x, const DatabaseIterator& y)
    {
        return !operator==( x, y );
    }
    

    
    bool  operator<(const DatabaseIterator& x, const DatabaseIterator& y)
    {
        if( x.m_db == y.m_db )
        {
            string xk;
            string yk;
            x.getKey( xk );
            y.getKey( yk );
            return xk < yk;
        }
        return false;
    }
        
    
    std::streamsize
    DatabaseIterator::partialRead( char* buffer, std::streamsize maxsz, std::streamsize startoffset )
    {
        std::streamsize ret = 0;

        DBT k;
        DBT d;
        memset(&k, 0, sizeof(k));
        setup( d, 0, 0 );
        
        int rc = m_cursor->get( m_cursor, &k, &d, DB_CURRENT );
        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
        {
//            free( k.data );
            ThrowFromDB4Error( rc );
        }

        string keystring;
        keystring.resize( k.size );
        
        memcpy( (char*)keystring.data(), k.data, k.size );
//        free( k.data );
        setup( k, keystring.data(), keystring.size() );

        
        setup( d, buffer, maxsz );
        d.dlen = maxsz;
        d.doff = startoffset;
        d.flags |= DB_DBT_PARTIAL;
        rc = m_cursor->c_get( m_cursor, &k, &d, DB_CURRENT );

        if( rc != 0 )
            ThrowFromDB4Error( rc );

        ret = d.size;
        return ret;
    }
    
    std::streamsize
    DatabaseIterator::partialWrite( const char* buffer, std::streamsize maxsz, std::streamsize startoffset )
    {
//        cerr << "DatabaseIterator::partialWrite() maxsz:" << maxsz << " offset:" << startoffset << endl;
        
        std::streamsize ret = -1;

        DBT k;
        DBT d;
        memset(&k, 0, sizeof(k));
        setup( d, 0, 0 );

//        cerr << "m_cur:" << (void*)( m_cursor ) << endl;
        
        int rc = m_cursor->c_get( m_cursor, &k, &d, DB_CURRENT );
        if( rc != 0 && rc != ENOMEM && rc != DB_BUFFER_SMALL )
        {
//            free( k.data );
            ThrowFromDB4Error( rc );
        }

        string keystring;
        keystring.resize( k.size );
        
        memcpy( (char*)keystring.data(), k.data, k.size );
//        free( k.data );
        setup( k, keystring.data(), keystring.size() );

        
        setup( d, buffer, maxsz );
        d.dlen = maxsz;
        d.doff = startoffset;
        d.flags |= DB_DBT_PARTIAL;
        rc = m_cursor->c_put( m_cursor, &k, &d, DB_CURRENT );

        if( rc != 0 )
            ThrowFromDB4Error( rc );

        ret = d.size;
        return ret;
    }
    
    void
    DatabaseIterator::debug()
    {
//        cerr << "m_cursor:" << (void*)m_cursor << endl;
    }
    

    
    
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    
};
