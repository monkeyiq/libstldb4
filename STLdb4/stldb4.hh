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

    $Id: stldb4.hh,v 1.9 2010/09/24 05:15:25 ben Exp $

*******************************************************************************
*******************************************************************************
******************************************************************************/

#ifndef _ALREADY_INCLUDED_STLDB4_H_
#define _ALREADY_INCLUDED_STLDB4_H_

#include <STLdb4/HiddenSymbolSupport.hh>


#include <string>
#include <iosfwd>
//#include <iostream>
#include <iterator>

#include <STLdb4/stldbraw.h>
#include <errno.h>
#include <string.h>

#include <FerrisLoki/loki/SmartPtr.h>
#include <FerrisLoki/Extensions.hh>
#include <FerrisStreams/All.hh>

namespace STLdb4
{
    class DatabaseIterator;
    class Database;
    class Transaction;
    class Environment;
    struct DatabaseMutableValueRef;
    typedef Loki::SmartPtr<
        Environment,
        FerrisLoki::FerrisExRefCounted,
        Loki::DisallowConversion,
        FerrisLoki::FerrisExSmartPointerChecker,
        FerrisLoki::FerrisExSmartPtrStorage >  fh_env;
    typedef Loki::SmartPtr<
        Transaction,
        FerrisLoki::FerrisExRefCounted,
        Loki::DisallowConversion,
        FerrisLoki::FerrisExSmartPointerChecker,
        FerrisLoki::FerrisExSmartPtrStorage >  fh_trans;

    
    /**
     * A special handle for Database smartpointers adding the ability to
     * lookup items by using [] on the smartpointer. This allows a much
     * more natural code like;
     *
     * fh_database db = new Database( DB_BTREE, "myfile.db" );
     * cerr << "value is:" << db["key"] << endl;
     */
    template
    <
        typename T,
        template <class> class OwnershipPolicy,
        class ConversionPolicy,
        template <class> class CheckingPolicy,
        template <class> class StoragePolicy
    >    
    class DatabaseHandle
        :
        public Loki::SmartPtr< T,
                               OwnershipPolicy,
                               ConversionPolicy,
                               CheckingPolicy,
                               StoragePolicy >
        {
            typedef Loki::SmartPtr< T,
                                    OwnershipPolicy,
                                    ConversionPolicy,
                                    CheckingPolicy,
                                    StoragePolicy > _Base;
            typedef DatabaseHandle< T,
                                    OwnershipPolicy,
                                    ConversionPolicy,
                                    CheckingPolicy,
                                    StoragePolicy > _Self;
        public:

            DatabaseHandle()
                {}
        

            DatabaseHandle( const typename STLdb4::DatabaseHandle<T, 
                            OwnershipPolicy, ConversionPolicy, CheckingPolicy, 
                            StoragePolicy>::StoredType & p )
                :
                _Base( p )
                {}

            DatabaseHandle( typename STLdb4::DatabaseHandle<T, 
                            OwnershipPolicy, ConversionPolicy, CheckingPolicy, StoragePolicy>::CopyArg& rhs)
                :
                _Base( rhs )
                {}
        
            template
            <
                typename T1,
                template <class> class OP1,
                class CP1,
                template <class> class KP1,
                template <class> class SP1
            >
            DatabaseHandle(const DatabaseHandle<T1, OP1, CP1, KP1, SP1>& rhs)
                :
                _Base( rhs )
                {
                }
        
            template
            <
                typename T1,
                template <class> class OP1,
                class CP1,
                template <class> class KP1,
                template <class> class SP1
            >
            DatabaseHandle( DatabaseHandle<T1, OP1, CP1, KP1, SP1>& rhs)
                :
                _Base( rhs )
                {
                }
        

//             DatabaseHandle( Loki::ByRef<DatabaseHandle> rhs )
//                 :
//                 _Base( rhs )
//                 {}
        
            DatabaseMutableValueRef operator[]( const std::string& k );
            DatabaseMutableValueRef operator[]( long k );
            
//                 {
//                     return this->operator->()->operator[]( k );
//                 }
        };
    typedef DatabaseHandle<
        Database,
        FerrisLoki::FerrisExRefCounted,
        Loki::DisallowConversion,
        FerrisLoki::FerrisExSmartPointerChecker,
        FerrisLoki::FerrisExSmartPtrStorage >  fh_database;

    
    
    class CoreDataChunk;
    typedef Loki::SmartPtr<
        CoreDataChunk,
        FerrisLoki::FerrisExRefCounted,
        Loki::DisallowConversion,
        FerrisLoki::FerrisExSmartPointerChecker,
        FerrisLoki::FerrisExSmartPtrStorage >  fh_coredatachunk;

    /**
     * Base class for all exceptions thrown in STLdb4
     */
    class FERRISEXP_EXCEPTION dbException : public std::exception
    {
        std::string m_desc;
        int m_ecode;
        
    public:
        dbException( const std::string& s = "", int ecode = 0 )
            :
            m_desc( s ),
            m_ecode( ecode )
            {
            }
        virtual ~dbException() throw() 
            {
            }

        /**
         * Get a decent error message for what happended
         */
        virtual const char* what() const throw ()
            {
                return m_desc.c_str();
            }
        
        /**
         * Returns the db4 C API return value of the function that caused the
         * exception to be thrown.
         */
        virtual int get_errno() const throw()
            {
                return m_ecode;
            }
    };
    class FERRISEXP_EXCEPTION dbOldVersionException : public dbException
    {
    public:
        dbOldVersionException( const std::string& s = "", int ecode = 0 )
            :
            dbException( s, ecode )
            {
            }
    };
    class FERRISEXP_EXCEPTION dbExists : public dbException
    {
    public:
        dbExists( const std::string& s = "", int ecode = 0 )
            :
            dbException( s, ecode )
            {
            }
    };
    class FERRISEXP_EXCEPTION dbInvalidParams : public dbException
    {
    public:
        dbInvalidParams( const std::string& s = "", int ecode = 0 )
            :
            dbException( s, ecode )
            {
            }
    };
    class FERRISEXP_EXCEPTION dbNonExist : public dbException
    {
    public:
        dbNonExist( const std::string& s = "", int ecode = 0 )
            :
            dbException( s, ecode )
            {
            }
    };

    /**
     * Used internally to throw the correct exception from a db4 error code
     *
     * @param ecode is the db4 C API return value that caused the error
     */
    FERRISEXP_API void ThrowFromDB4Error( int ecode );

    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/


    
    /**
     * This is roughly the equal of the Dbt type in the sleepycat
     * C++ API. We handle a reference counted resizeable chunk
     * of memory.
     */
    class FERRISEXP_API CoreDataChunk
        :
        public FerrisLoki::Handlable
    {
        std::string m_data;

    public:

        CoreDataChunk()
            :
            m_data("")
            {
            }
        
        CoreDataChunk( const std::string& data )
            :
            m_data( data )
            {
            }

        CoreDataChunk( std::streamsize defaultsize )
            {
                m_data.reserve( defaultsize );
            }
        
        virtual ~CoreDataChunk()
            {
            }

        const std::string& tostring()
            {
                return m_data;
            }
        
        char* data()
            {
                return (char*)m_data.data();
            }

        std::streamsize size()
            {
                return m_data.size();
            }

        std::streamsize length()
            {
                return m_data.length();
            }
        
        void DoubleSize( bool preserve = false );
    };

    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/

    /*
     * Because these constants change between db releases (4.0 to 4.1 changed MPOOL)
     * then I have a bunch of static env methods to get the value and I setup the
     * default open flags etc as constants
     */
//         enum 
//         {
// //            default_open_flags = ( DB_CREATE | DB_INIT_TXN | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL ),
// //            default_open_mode  = 0
//         };
    FERRISEXP_API extern const u_int32_t environment_default_open_flags;
    FERRISEXP_API extern const u_int32_t environment_default_open_mode;

    
    /**
     * based on DB_ENV
     */
    class FERRISEXP_API Environment
        :
        public FerrisLoki::Handlable
    {
        friend class Transaction;
        friend class Database;
        
        DB_ENV* m_env;
        DB_ENV* getRaw();

        static u_int32_t s_defaultOpenFlags; //< allow the getDefault() flags to be overridden
    protected:
        
        /**
         * Common constructor code
         */
        virtual void commonSetup( std::string db_home,
                                  u_int32_t create_flags,
                                  u_int32_t open_flags,
                                  int mode );

        /**
         * For creating a null environment. Doesn't call commonSetup()
         */
        Environment( double dummy );
        
    public:

        static u_int32_t get_openflag_create();
        static u_int32_t get_openflag_init_txn();
        static u_int32_t get_openflag_init_lock();
        static u_int32_t get_openflag_init_log();
        static u_int32_t get_openflag_init_mpool();

        static void   setDefaultOpenFlags( u_int32_t flags = environment_default_open_flags );
        static fh_env getDefault();
        static fh_env getNull();
        static void   setDefault( fh_env e );
        static fh_env getDefaultIfNull( fh_env e );
 
        Environment( const std::string& db_home,
                     u_int32_t flags = environment_default_open_flags, int mode = environment_default_open_mode );
        Environment( u_int32_t flags = environment_default_open_flags, int mode = environment_default_open_mode );
        virtual ~Environment();

        virtual DB_ENV* raw();
    };
    
    
    /**
     * Based on DB_TXN.
     */
    class FERRISEXP_API Transaction
        :
        public FerrisLoki::Handlable
    {
        DB_TXN* m_txn;
        
    protected:

        /**
         * Used by null transaction subclass to init a transaction without
         * getting a real one from db4
         */
        Transaction( bool weAreFalse );
        
    public:

        /**
         * This signal is fired whenever the transaction has attempted to
         * commit. The status of the commit is passed in as a bool and is
         * true if the commit was successful.
         *
         * pointer the the transaction, bool on if the commit or abort was successful
         */
        typedef sigc::signal2< void, fh_trans, bool > Commit_Sig_t;
        Commit_Sig_t& getCommitSig() 
            { return Commit_Sig; }
        
        /**
         * This signal is fired whenever the transaction has attempted to
         * abort. The status of the abort is passed in as a bool and is
         * true if the abort was successful.
         *
         * pointer the the transaction, bool on if the abort was successful
         */
        typedef sigc::signal2< void, fh_trans, bool > Abort_Sig_t;
        Abort_Sig_t& getAbortSig()
            { return Abort_Sig; }
        
        Transaction( fh_trans parent = 0, fh_env env = 0, u_int32_t flags = 0 );
        virtual ~Transaction();

        virtual u_int32_t id();
        virtual u_int32_t set_timeout( db_timeout_t timeout = 120 * 1000,
                                       u_int32_t flags = DB_SET_TXN_TIMEOUT );

        virtual void commit( u_int32_t flags = DB_TXN_SYNC );
        virtual void abort();

        virtual DB_TXN* raw();

        void setDefaultDestructionIsAbort( bool v );

    private:
        Transaction( const Transaction& );
        Transaction& operator=( const Transaction& );
        
        Commit_Sig_t Commit_Sig;
        Abort_Sig_t  Abort_Sig;
        bool m_DefaultDestructionIsAbort;
    };

    /**
     * Get a Transaction object which does nothing
     */
    FERRISEXP_API fh_trans getNullTransaction();    

    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    
    FERRISEXP_API bool operator==(const DatabaseIterator& x, const DatabaseIterator& y);
    FERRISEXP_API bool operator!=(const DatabaseIterator& x, const DatabaseIterator& y);
    FERRISEXP_API bool operator<(const DatabaseIterator& x, const DatabaseIterator& y);

    /**
     * An STL iterator for db4 files. The iterator adds methods to get IOStream
     * access to the data of the entry pointed to and an explicit partial IO
     * interface.
     *
     */
    class FERRISEXP_API DatabaseIterator
//         : 
//         public std::iterator< std::bidirectional_iterator_tag, std::string >
    {
        typedef DatabaseIterator _Self;
        
        fh_database m_db;
        fh_trans    m_trans;
        DBC*        m_cursor;
        mutable std::pair< std::string, std::string > m_cache;
        mutable bool m_isCacheValid;

        void UpdateCache() const;
        void InvalidateCache() const;

    public: // FIXME
        /**
         * Used by both the public getPrimaryKey() and getRecNumber() methods
         */
        const std::string& getPrimaryKey( std::string& key, bool isRecno ) const;
        
    public:

        typedef std::bidirectional_iterator_tag              iterator_category;
        typedef std::pair< std::string, std::string >        value_type;
        typedef long                                         difference_type;
        typedef std::pair< std::string, std::string >*       pointer;
        typedef const std::pair< std::string, std::string >& reference;

        // for STL
        DatabaseIterator( fh_database m_db = 0, fh_trans t = 0, DBC* m_cursor = 0 );
        virtual ~DatabaseIterator();
        DatabaseIterator( const DatabaseIterator& ci );
        DatabaseIterator& operator=( const DatabaseIterator& ci );
        
        reference operator*() const;
        pointer   operator->() const;

        /**
         * equal to dbc_get with DB_NEXT_DUP
         *
         * if there is no next duplicate item then the iterator will
         * be set to end()
         */
        int moveCursorNextDup();

        /**
         * Move the cursor as specified by the db4.1 flags given
         *
         * if( rc != 0 && rc != ENOMEM && rc != DB_NOTFOUND )
         * then an exception is thrown
         */
        int moveCursor( u_int32_t flags );
        void setCursorToEnd();
        
        _Self& operator++();
        _Self  operator++(int);
        _Self& operator--();
        _Self  operator--(int);

        /**
         * Get the key associated with this entry.
         * this may be more efficient than doing
         * iter->first because using this method can have less memory
         * management due to doing a key.resize() and reading directly
         * into the passed string reference.
         *
         * When used on a secondary index iterator this is the key
         * in the secondary index. See getParimaryKey() for the ability
         * to get the primary key from a secondary index iterator.
         *
         * @param key where to put the data for the key of this iterator
         * @return reference to key
         * @see getData()
         */
        const std::string& getKey( std::string& key ) const;

        /**
         * Get the key associated with the primary index for this entry.
         * equal to the value of pkey in the c_pget() db4 C API call
         *
         * this may be more efficient than doing
         * iter->first because using this method can have less memory
         * management due to doing a key.resize() and reading directly
         * into the passed string reference.
         *
         * @param key where to put the data for the key of this iterator
         * @return reference to key
         * @see getData()
         */
        const std::string& getPrimaryKey( std::string& key ) const;

        /**
         * If this cursor is over a recno database then this method
         * will get the recno that is currently pointed to. Also
         * for secondary indexes on recno databases this method will
         * get the recno from the primary db that a iterator on the
         * secondary index points to
         */
        db_recno_t getRecNumber() const;
        
        
        /**
         * Get the data for the item pointed to by this iterator into
         * 'v'. Note that this method should give the same data as
         * iter->second but might be faster due to less memory copying.
         *
         * Don't use this method for getting access to items when there
         * are large data items. Use the getIStream() or PartialRead()
         * methods to allow partial IO.
         *
         * @param v where to put the data for this iterator
         * @return a reference to v
         *
         * @see getKey()
         * @see getIStream()
         * @see getIOStream()
         */
        const std::string& getValue( std::string& v ) const;

        /**
         * Logically equal to string tmp; getValue( tmp ).length();
         * but may be much faster.
         *
         * @return the size of the data item for this iterator
         */
        std::streamsize getValueSize() const;

        /**
         * Set the value for this iterator to the data given
         *
         * @see setValue(string)
         */
        void setValue( const char* data, std::streamsize datalen );

        /**
         * Set the value for this iterator to the data given
         */
        void setValue( const std::string& data )
            {
                setValue( data.data(), data.size() );
            }
        
        

        
        /**
         * Get a Standard IO Stream for partial read access to the data.
         *
         * @return a stream for partial io
         * @see getIOStream()
         */
        Ferris::fh_istream  getIStream( u_int32_t flags = 0 );

        /**
         * Get a Standard IO Stream for partial read/write access to the data.
         *
         * @return a stream for partial io
         * @see getIStream()
         */
        Ferris::fh_iostream getIOStream( u_int32_t flags = 0 );

        /**
         * Get a Standard IO Stream for partial read/write access to the data.
         * The ios::trunc flag may be passed as openmode to truncate the existing
         * data. ios::ate is simulated by seeking the stream to its end before
         * returning.
         *
         * @return a stream for partial io
         * @see getIStream()
         */
        Ferris::fh_iostream getIOStream( Ferris::ferris_ios::openmode m, u_int32_t flags );
        
        
        /**
         * Read up to maxsz octets into buffer and return the number of bytes read
         */
        std::streamsize partialRead( char* buffer, std::streamsize maxsz, std::streamsize startoffset );

        /**
         * Write out data starting at buffer of length maxsz to the starting offset startoffset
         * overwriting existing data.
         *
         * return -1 for error or 0 for success
         */
        std::streamsize partialWrite( const char* buffer, std::streamsize maxsz, std::streamsize startoffset );

        /**
         * An easy way to print the internal state for internal library debuging only.
         */
        void debug();
        
    protected:
        friend class Database;

        /**
         * Note that the cursor is taken by this call, you shouldn't use or free it
         * after.
         */
        static DatabaseIterator Create( fh_database db, fh_trans t, DBC* cursor );

        friend bool operator==(const DatabaseIterator& x, const DatabaseIterator& y);
        friend bool operator!=(const DatabaseIterator& x, const DatabaseIterator& y);
        friend bool operator<(const DatabaseIterator& x, const DatabaseIterator& y);
    };

    
    /********************************************************************************/
    /********************************************************************************/

    /**
     * Based on the std::reverse_iterator class in the STLport 4.5 codebase
     *
     * Created for two reasons;
     * 1) allow methods on the iterator itself.
     * 2) allow reference to be a pair<>& in the base and a pair<> in the reverse
     *    iterator
     *
     * This is the reference type explaination.
     *
     * From looking at traces when someone does the following
     * Database::reverse_iterator iter = db->rbegin();
     * const pair<string,string>& p = *iter;
     *
     * code from reverse_iterator does the following;
     *   reference operator*() const {
     *        _Iterator __tmp = current;
     *        return *--__tmp;
     *   }
     * which means that the reference typedef below can not be
     * const pair< string, string >&
     * because the iterator 'tmp' is reclaimed before the reference is returned
     * to the caller, thus the reference must be able to outlive the iterator
     * and we resort to pair<string,string>
     *
     * I'm not really happy with the extra copying needed there with the use of plain reverse_iterator.
     * this way a operator->() on a reverse iterator will only create a small object on the stack.
     */
    template <class _Iterator>
    class db_reverse_iterator
        : 
        public std::iterator<typename std::iterator_traits<_Iterator>::iterator_category,
                             typename std::iterator_traits<_Iterator>::value_type,
                             typename std::iterator_traits<_Iterator>::difference_type,
                             typename std::iterator_traits<_Iterator>::pointer,
                             typename std::iterator_traits<_Iterator>::reference>
    {
    protected:
        _Iterator current;
        typedef db_reverse_iterator<_Iterator> _Self;
    public:
        typedef typename std::iterator_traits<_Iterator>::iterator_category  iterator_category;
        typedef typename std::iterator_traits<_Iterator>::value_type         value_type;
        typedef typename std::iterator_traits<_Iterator>::difference_type    difference_type;
        typedef typename std::iterator_traits<_Iterator>::pointer            pointer;
        typedef const std::pair< std::string, std::string >                  reference;
        typedef _Iterator iterator_type;
    public:
        db_reverse_iterator() {}
        explicit db_reverse_iterator(iterator_type __x) : current(__x) {}
        db_reverse_iterator(const _Self& __x) : current(__x.current) {}
        _Self& operator = (const _Self& __x) { current = __x.base(); return *this; } 
        template <class _Iter>
        db_reverse_iterator(const db_reverse_iterator<_Iter>& __x) : current(__x.base()) {}
        template <class _Iter>
        _Self& operator = (const db_reverse_iterator<_Iter>& __x) { current = __x.base(); return *this; } 
    
        iterator_type base() const { return current; }
        iterator_type getCurrent() const
            {
                _Iterator __tmp = current;
                return --__tmp;
            }
        
        reference operator*() const {
            _Iterator __tmp = current;
            return *--__tmp;
        }
        struct Arrow
        {
            typedef std::pair< std::string, std::string >*      pointer;
            typedef const std::pair< std::string, std::string > reference;
            _Iterator m_iter;
            Arrow( _Iterator iter )
                :
                m_iter( iter )
                {
                }

            pointer operator->() const
                {
                    return m_iter.operator->();
                }
        };
        Arrow operator->() const
            {
                _Iterator __tmp = current;
                --__tmp;
                return Arrow(__tmp);
            }
        _Self& operator++() {
            --current;
            return *this;
        }
        _Self operator++(int) {
            _Self __tmp = *this;
            --current;
            return __tmp;
        }
        _Self& operator--() {
            ++current;
            return *this;
        }
        _Self operator--(int) {
            _Self __tmp = *this;
            ++current;
            return __tmp;
        }

        _Self operator+(difference_type __n) const {
            return _Self(current - __n);
        }
        _Self& operator+=(difference_type __n) {
            current -= __n;
            return *this;
        }
        _Self operator-(difference_type __n) const {
            return _Self(current + __n);
        }
        _Self& operator-=(difference_type __n) {
            current += __n;
            return *this;
        }

    public:

        const std::string& getKey( std::string& key ) const
            {
                return getCurrent().getKey( key );
            }
        
        const std::string& getValue( std::string& v ) const
            {
                return getCurrent().getValue( v );
            }
        
        std::streamsize getValueSize() const
            {
                return getCurrent().getValueSize();
            }
        
        Ferris::fh_istream  getIStream( u_int32_t flags = 0 )
            {
                return getCurrent().getIStream( flags );
            }

        Ferris::fh_iostream getIOStream( u_int32_t flags = 0 )
            {
                return getCurrent().getIOStream( flags );
            }
        
        std::streamsize partialRead( char* buffer, std::streamsize maxsz, std::streamsize startoffset )
            {
                return getCurrent().partialRead( buffer, maxsz, startoffset );
            }

        std::streamsize partialWrite( const char* buffer, std::streamsize maxsz, std::streamsize startoffset )
            {
                return getCurrent().partialWrite( buffer, maxsz, startoffset );
            }
        
    }; 
 
    template <class _Iterator>
    inline bool   operator==(const db_reverse_iterator<_Iterator>& __x, 
                             const db_reverse_iterator<_Iterator>& __y) {
        return __x.base() == __y.base();
    }

    template <class _Iterator>
    inline bool  operator<(const db_reverse_iterator<_Iterator>& __x, 
                           const db_reverse_iterator<_Iterator>& __y) {
        return __y.base() < __x.base();
    }

    template <class _Iterator>
    inline bool  operator!=(const db_reverse_iterator<_Iterator>& __x, 
                            const db_reverse_iterator<_Iterator>& __y) {
        return !(__x == __y);
    }

    template <class _Iterator>
    inline bool  operator>(const db_reverse_iterator<_Iterator>& __x, 
                           const db_reverse_iterator<_Iterator>& __y) {
        return __y < __x;
    }

    template <class _Iterator>
    inline bool  operator<=(const db_reverse_iterator<_Iterator>& __x, 
                            const db_reverse_iterator<_Iterator>& __y) {
        return !(__y < __x);
    }

    template <class _Iterator>
    inline bool  operator>=(const db_reverse_iterator<_Iterator>& __x, 
                            const db_reverse_iterator<_Iterator>& __y) {
        return !(__x < __y);
    }

    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    
    /**
     * Core database abstraction logically equal to the C API 'DB' class.
     * many methods mimic the STL versions, like begin(), end(), find(), lower_bound()
     * etc. These are not documented at current, but should be easy enough to guess at.
     *
     * There are also versions of find(), upper_bound() etc with _partial appended to
     * their names. These work using partial lookup as described by the db4 docs.
     *
     * See the testsuite/cpp files for examples.
     */
    class FERRISEXP_API Database 
        :
        public FerrisLoki::Handlable
    {
        /**
         * All database objects have to die before the environment they depend on.
         * So we keep a reference to the environment to lock it for out lifetime.
         */
        fh_env m_env;
        
        /**
         * Common constructor code
         */
        void commonSetup( fh_env env = 0 );

        fh_trans    m_implicitTransaction;
        /**
         * If the param 't' is valid then return that, otherwise
         * check for an implicit transaction and return that.
         */
        fh_trans freshenTransaction( fh_trans t );

        /**
         * Wipe out implicit transaction if the user commits or aborts it
         * behind us.
         */
        void OnImplicitTransactionCommitOrAbort( fh_trans t, bool succ );
        sigc::connection m_OnCommitConnection;
        sigc::connection m_OnAbortConnection;

        /**
         * disconnect from m_implicitTransaction and drop reference
         */
        void clearImplicitTransaction();

        /**
         * Advance a key k to be the next key k just using string positioning.
         * eg.
         *   k=fred out is free
         */
        const std::string& advanceKey( std::string& k );
        
    public:

        typedef const std::string key_type;
        typedef       std::string data_type;
        typedef std::pair<const key_type, data_type> value_type;
        typedef std::less< std::string > key_compare;
        typedef std::less< std::string > value_compare;
        typedef std::streamsize size_type;

        typedef int (*sec_idx_callback)(DB *, const DBT *, const DBT *, DBT *);

        
        /**
         * Set the transaction that all actions will be implicitly a part
         * of.
         * example:
         * fh_database db = new Database( ... );
         * fh_trans    tr = new Transaction();
         * db->setImplicitTransaction( tr );
         * db["key"] = "value";
         * db["key2"] = "value2";
         * db->commit(); or db->abort();
         *
         * When the commit() line is executed the Implicit transaction is commit()ed
         * and its reference is dropped and the database is back to non transacted mode.
         *
         * Either use start() if you don't need to sync transactions across many
         * databases or create a transaction and use setImplicitTransaction()
         * on each database you are going to use, then do the updates, then commit
         * the transaction. If the implicit transaction is commited via a smartptr
         * you hold then the database won't try to commit() it again. Once the
         * transaction is commit()ed, no matter if its via Database::commit() or
         * the commit() method on the transaction itself, then there is no
         * implicit transaction for the database anymore.
         *
         * If a new implicit transaction is set when the previous one has
         * not been commited then the previous transaction is commit()ed
         * and the handle dropped.
         *
         * Note that one can still use the same database handle with
         * other transactions when an implicit transaction has been
         * set by passing the fh_trans to the API call for that action.
         *
         * PASSED transaction pointers will ALWAYS override implicit ones. 
         */
        void setImplicitTransaction( fh_trans t );

        /**
         * Start a new implicit transaction.
         * Logically the same as
         * setImplicitTransaction( new Transaction() );
         */
        void start()
            {
                setImplicitTransaction( new Transaction() );
//                std::cerr << "start() rc:" << m_implicitTransaction->getReferenceCount() << std::endl;
            }
        
        /**
         * commit transaction set via setImplicitTransaction()
         */
        void commit();
        
        /**
         * abort transaction set via setImplicitTransaction()
         */
        void abort();
        

        /**
         * Create a database pointer that is not associated with any storage.
         * You must call open() or create() before attempting to access the
         * database.
         *
         * @param env db environment
         *
         * @see open()
         * @see create()
         */
        Database( fh_env env = 0 );

        /**
         * create or open the database at filename/dbname
         *
         * much like a x = new Database(); x->create(); combination
         * @see create()
         */
        Database( DBTYPE type,
                  const std::string& filename,
                  const std::string& database = "",
                  u_int32_t flags = DB_CREATE,
                  int mode = 0 )
            {
                commonSetup();
//                impl_open( filename, database, type, flags, mode, 0 );
                create( type, filename, database, flags, mode );
            }

        /**
         * create or open the database at filename/dbname
         *
         * much like a x = new Database(); x->create(); combination
         * @see create()
         */
        Database( fh_trans t,
                  DBTYPE type,
                  const std::string& filename,
                  const std::string& database = "",
                  u_int32_t flags = DB_CREATE,
                  int mode = 0 )
            {
                commonSetup();
//                impl_open( filename, database, type, flags, mode, t );
                create( t, type, filename, database, flags, mode );
            }

        /**
         * Open a database.
         *
         * @param filename The name of the file to store the database in
         * @param database optional name of the database within the file
         *        set this to a zero length string to not specify a dbname
         *
         * @see open()
         */
        Database( const std::string& filename,
                  const std::string& database = "" )
            {
                commonSetup();
                impl_open( filename, database, DB_UNKNOWN, 0, 0, 0 );
            }
        /**
         * Open a database.
         *
         * @param filename The name of the file to store the database in
         * @param database optional name of the database within the file
         *        set this to a zero length string to not specify a dbname
         * @param read_only if set the db is open read only, otherwise DB_CREATE
         *        is set.
         *
         * @see open()
         */
        Database( const std::string& filename,
                  const std::string& database,
                  bool read_only );
        
        /**
         * Open a database.
         *
         * @param filename The name of the file to store the database in
         * @param database optional name of the database within the file
         *        set this to a zero length string to not specify a dbname
         *
         * @see open()
         */
        Database( fh_trans t,
                  const std::string& filename,
                  const std::string& database = "" )
            {
                commonSetup();
                impl_open( filename, database, DB_UNKNOWN, 0, 0, t );
            }
        
        
        /**
         * Create or open the secondary index database at filename/dbname
         * The passed in database is the primary db and the secondary index
         * is associated for you.
         *
         * @param pdb primary database to associate this secondary index with
         * @param f the function to get the data from the primary index to
         *          build the secondary index with.
         * @see create()
         */
        static fh_database makeSecondaryIndex( fh_database pdb,
                                               sec_idx_callback f,
                                               DBTYPE type,
                                               const std::string& filename,
                                               const std::string& database = "",
                                               u_int32_t flags = DB_CREATE,
                                               int mode = 0 );
        typedef int (*bt_compare_fcn)(DB *, const DBT *, const DBT *);
        static fh_database makeSecondaryIndex( fh_trans trans,
                                               fh_database pdb,
                                               sec_idx_callback f,
                                               DBTYPE type,
                                               const std::string& filename,
                                               const std::string& database = "",
                                               u_int32_t flags = DB_CREATE,
                                               int mode = 0,
                                               bt_compare_fcn btcompf = 0 );
        static fh_database makeSecondaryIndex( fh_env env,
                                               fh_trans trans,
                                               fh_database pdb,
                                               sec_idx_callback f,
                                               DBTYPE type,
                                               const std::string& filename,
                                               const std::string& database = "",
                                               u_int32_t flags = DB_CREATE,
                                               int mode = 0,
                                               bt_compare_fcn btcompf = 0 );
        
        virtual ~Database();

        /**
         * close a database. This is done for you if the database is still open
         * and the last database handle goes out of scope, but that does open
         * the issue that implicit close() through the dtor can throw an exception.
         *
         * Read more exceptional C++ for why throwing in dtors is a bad thing.
         */
        void close();
        
        /**
         * Open a database.
         *
         * @param filename The name of the file to store the database in
         * @param database optional name of the database within the file
         *        set this to a zero length string to not specify a dbname
         *
         * @see db_open in the C API
         */
        void open( const std::string& filename,
                   const std::string& database = "" )
            {
                impl_open( filename, database, DB_UNKNOWN, 0, 0, 0 );
            }
        
        /**
         * Create a database.
         *
         * Note that the flags argument will be automatically combined
         * with DB_CREATE internally.
         *
         * @param type one of DB_BTREE, DB_HASH, DB_QUEUE, DB_RECNO
         * @param filename The name of the file to store the database in
         * @param database optional name of the database within the file
         *        set this to a zero length string to not specify a dbname
         * @param flags a combination of DB_CREATE, DB_DIRTY_READ, DB_EXCL
         *            DB_NOMMAP, DB_RDONLY, DB_THREAD and DB_TRUNCATE
         * @param mode UNIX mode to use for newly created files.
         *
         * @see db_open in the C API
         */
        void create( DBTYPE type,
                     const std::string& filename,
                     const std::string& database = "",
                     u_int32_t flags = DB_CREATE,
                     int mode = 0 )
            {
                if( !(flags & DB_RDONLY) )
                    flags |= DB_CREATE;
                
                impl_open( filename, database, type, flags, mode, 0 );
            }

        /**
         * Create a database.
         *
         * Note that the flags argument will be automatically combined
         * with DB_CREATE internally.
         *
         * @param type one of DB_BTREE, DB_HASH, DB_QUEUE, DB_RECNO
         * @param filename The name of the file to store the database in
         * @param database optional name of the database within the file
         *        set this to a zero length string to not specify a dbname
         * @param flags a combination of DB_CREATE, DB_DIRTY_READ, DB_EXCL
         *            DB_NOMMAP, DB_RDONLY, DB_THREAD and DB_TRUNCATE
         * @param mode UNIX mode to use for newly created files.
         *
         * @see db_open in the C API
         */
        void create( fh_trans t,
                     DBTYPE type,
                     const std::string& filename,
                     const std::string& database = "",
                     u_int32_t flags = DB_CREATE,
                     int mode = 0 )
            {
                if( !(flags & DB_RDONLY) )
                    flags |= DB_CREATE;

                impl_open( filename, database, type, flags, mode, t );
            }
        
        /**
         * Open or create a database.
         *
         * @param filename The name of the file to store the database in
         * @param database optional name of the database within the file
         *        set this to a zero length string to not specify a dbname
         * @param type one of DB_BTREE, DB_HASH, DB_QUEUE, DB_RECNO, or DB_UNKNOWN
         *             only required if flags contains DB_CREATE
         * @param flags a combination of DB_CREATE, DB_DIRTY_READ, DB_EXCL
         *            DB_NOMMAP, DB_RDONLY, DB_THREAD and DB_TRUNCATE
         * @param mode UNIX mode to use for newly created files.
         *
         * @see db_open in the C API
         */
        void open( const std::string& filename,
                   const std::string& database,
                   DBTYPE type,
                   u_int32_t flags,
                   int mode )
            {
                impl_open( filename, database, type, flags, mode, 0 );
            }

        /**
         * Open or create a database.
         *
         * @param filename The name of the file to store the database in
         * @param database optional name of the database within the file
         *        set this to a zero length string to not specify a dbname
         * @param type one of DB_BTREE, DB_HASH, DB_QUEUE, DB_RECNO, or DB_UNKNOWN
         *             only required if flags contains DB_CREATE
         * @param flags a combination of DB_CREATE, DB_DIRTY_READ, DB_EXCL
         *            DB_NOMMAP, DB_RDONLY, DB_THREAD and DB_TRUNCATE
         * @param mode UNIX mode to use for newly created files.
         *
         * @see db_open in the C API
         */
        void open( fh_trans t,
                   const std::string& filename,
                   const std::string& database,
                   DBTYPE type,
                   u_int32_t flags,
                   int mode )
            {
                impl_open( filename, database, type, flags, mode, t );
            }
        
        /**
         * Sync data to disk
         *
         * @see db_sync in the C API
         */
        virtual void sync()
            {
                int rc = m_db->sync( m_db, 0 );
                if( rc != 0 )
                    ThrowFromDB4Error( rc );
            }

        /**
         * Associate a secondary index with this database
         * 
         */
        void associate( fh_database secdb,
                        sec_idx_callback f,
                        u_int32_t flags = 0,
                        fh_trans trans = 0 );
        
        /**
         * true if the database is a DB_RECNO
         */
        bool isRecNum();

        /**
         * true if the database is a DB_QUEUE
         */
        bool isQueue();
        

//         typedef Loki::Functor< int, TYPELIST_5( DB*,
//                                                 const char* k1, size_type k1len,
//                                                 const char* k2, size_type k2len ) > bt_compare_functor_t;

        bt_compare_fcn m_bt_compare_fcn;
        
        /**
         * C API DB->set_bt_compare
         *
         * @see getInt32Compare();
         * @see getUInt32Compare();
         * @see getInt64Compare();
         * @see getUInt64Compare();
         * @see getFloatCompare();
         * @see getDoubleCompare();
         * @see getCISCompare();
         * @see getCSCompare();
         */
        void set_bt_compare( bt_compare_fcn f );

        typedef Loki::Functor< int, LOKI_TYPELIST_3( DB*, const DBT *, const DBT * ) > m_bt_compare_functor_t;
        void set_bt_compare( m_bt_compare_functor_t f );
        

        /**
         * access to the raw DB C API pointer.
         */
        DB* raw()
            {
                return m_db;
            }
        
        /**
         * Get a streambuf for the data at the given key
         */
        template < class DataSizeMaker >
        inline Ferris::fh_istream
        getIStream( const DataSizeMaker& key, u_int32_t flags = 0, fh_trans t = 0 )
            {
                return impl_getIStream( key.data(), (std::streamsize)key.length(), flags, t );
            }

        /**
         * Get a streambuf for the data at the given key
         */
        inline Ferris::fh_istream
        getIStream( const std::string& key, u_int32_t flags = 0, fh_trans t = 0 )
            {
                return impl_getIStream( key.data(), (std::streamsize)key.length(), flags, t );
            }

        inline Ferris::fh_istream
        getIStream( const char* key, fh_trans t = 0 )
            {
                return impl_getIStream( key, strlen(key), 0, t );
            }

        template < class DataSizeMaker >
        inline Ferris::fh_iostream
        getIOStream( const DataSizeMaker& key, u_int32_t flags = 0, fh_trans t = 0 )
            {
                return impl_getIOStream( key.data(), key.length(), flags, t );
            }

        /**
         * Get a streambuf for the data at the given key
         */
        inline Ferris::fh_iostream
        getIOStream( const std::string& key,
                     u_int32_t flags = 0,
                     fh_trans t = 0 )
            {
                return impl_getIOStream( key.data(), key.length(), flags, t );
            }

        inline Ferris::fh_iostream
        getIOStream( const char* key,
                     fh_trans t = 0 )
            {
                return impl_getIOStream( key, strlen(key), 0, t );
            }

        
        /**
         * Get a in memory chunk version of the data at the given key
         */
        template < class DataSizeMaker >
        inline fh_coredatachunk getChunk( const DataSizeMaker& key, u_int32_t flags = 0, fh_trans t = 0 )
            {
                std::streamsize  sz = impl_getValueSize( key.data(), key.length(), flags, t );
                fh_coredatachunk ret = new CoreDataChunk( sz );
                return impl_getChunk(  key.data(), key.length(),
                                       ret->data(), ret->length(),
                                       flags, t );
            }
        inline fh_coredatachunk getChunk( const std::string& key,
                                          u_int32_t flags = 0,
                                          fh_trans t = 0 )
            {
                std::streamsize  sz = impl_getValueSize( key.data(), key.length(), flags, t );
                fh_coredatachunk ret = new CoreDataChunk( sz );
                impl_getChunk(  key.data(), key.length(), 
                                ret->data(), ret->length(),
                                flags, t );
                return ret;
            }

        /**
         * Logically equal to data = operator[]( key ) but might be
         * faster because data can be read directly into data
         *
         * @see set()
         */
        std::string& get( const std::string& key,
                         std::string& data,
                         u_int32_t flags = 0,
                         fh_trans t = 0 );

        /**
         * Logically equal to performing a get() operation and returning
         * the default value if an error occurs.
         *
         * @see set()
         */
        const std::string& getWithDefault( const std::string& key,
                                           std::string& data,
                                           const std::string& def,
                                           u_int32_t flags = 0,
                                           fh_trans t = 0 );

        /**
         * Get the primary key and data from a secondary index using the
         * secondary key 'skey' and return a reference the the primary data
         */
        std::string& pget( const std::string& skey,
                           std::string& pkey,
                           std::string& data,
                           u_int32_t flags = 0,
                           fh_trans t = 0 );
        

            
        /**
         * Get data via record number for recno/queue db types
         */
        std::string get( db_recno_t recno, fh_trans t = 0 );
        
        
        /**
         * Set the data at the given key to the value.
         */
        template < class DataSizeMaker >
        inline void setChunk( const DataSizeMaker& key,
                              const DataSizeMaker& value,
                              u_int32_t flags = 0,
                              fh_trans t = 0 )
            {
                impl_setChunk( key.data(), key.length(), value.data(), value.length(), flags, t );
            }
        template < class DataSizeMaker >
        inline void setChunk( const std::string& key,
                              const DataSizeMaker& value,
                              u_int32_t flags = 0,
                              fh_trans t = 0 )
            {
                impl_setChunk( key.data(), key.length(), value.data(), value.length(), flags, t );
            }
        /**
         * equal to operator[ key ] = value;
         */
        inline void set( const std::string& key,
                         const std::string& value,
                         u_int32_t flags = 0,
                         fh_trans t = 0 )
            {
                impl_setChunk( key.data(), key.length(), value.data(), value.length(), flags, t );
            }
        void set( db_recno_t recno,
                  const std::string& value,
                  u_int32_t flags = 0,
                  fh_trans t = 0 );

        /**
         * Used to add a new item to a queue or recno db. The number of the
         * new record is returned.
         */
        db_recno_t impl_append( const char* data = 0,
                                std::streamsize datalen = 0,
                                fh_trans t = 0 );
        inline db_recno_t append( const std::string& data, fh_trans t = 0 )
            {
                return impl_append( data.data(), data.length(), t );
            }
        
        

        /**
         * equal to DB->set_flags(). Mainly useful for tweaking how the db4 library
         * will setup files, for example allowing duplicate data items for the same
         * key.
         *
         * 
         */
        void set_flags( u_int32_t flags );


        /**
         * equal to DB->set_pagesize(). 
         */
        void set_pagesize( u_int32_t pagesize );

        /**
         * qual to DB->set_cachesize().
         */
        int set_cachesize( u_int32_t gbytes, u_int32_t bytes, int ncache );
        int set_cachesize( unsigned long MegaBytes );
        
        /********************************************************************************/
        /********************************************************************************/
        /********************************************************************************/

        typedef DatabaseIterator iterator;
        typedef db_reverse_iterator<DatabaseIterator> reverse_iterator;
        iterator begin( fh_trans t = 0 );
        iterator end( fh_trans t = 0 );
        reverse_iterator rbegin( fh_trans t = 0 );
        reverse_iterator rend( fh_trans t = 0 );
        iterator find( const char* key, std::streamsize keylen, fh_trans t = 0 );
        iterator find( const char* key, fh_trans t = 0 )
            {
                return find( (std::string)key, t );
            }
        inline iterator find( const std::string& key, fh_trans t = 0 )
            {
                return find( key.data(), key.length(), t );
            }
        template < class DataSizeMaker >
        iterator find( const DataSizeMaker& key, fh_trans t = 0 )
            {
                return find( key.data(), key.length(), t );
            }
        iterator find( db_recno_t recno, fh_trans t = 0 );
        
        iterator find_partial( const char* partialkey, size_type partialkeylen, fh_trans t = 0 );
        iterator find_partial( const std::string& partialkey, fh_trans t = 0 )
            {
                return find_partial( partialkey.data(), partialkey.length(), t );
            }
        template < class DataSizeMaker >
        iterator find_partial( const DataSizeMaker& key, fh_trans t = 0 )
            {
                return find_partial( key.data(), key.length(), t );
            }
        
        iterator lower_bound( const char* key, std::streamsize keylen, fh_trans t = 0 );
        iterator lower_bound( const std::string& key, fh_trans t = 0 )
            {
                return lower_bound( key.data(), key.length(), t );
            }
        iterator lower_bound_partial( const char* key, std::streamsize keylen, fh_trans t = 0 )
            {
                return find_partial( key, keylen, t );
            }
        iterator lower_bound_partial( const std::string& key, fh_trans t = 0 )
            {
                return lower_bound_partial( key.data(), key.length(), t );
            }
        iterator upper_bound( const char* key, std::streamsize keylen, fh_trans t = 0 );
        iterator upper_bound( const std::string& key, fh_trans t = 0 )
            {
                return upper_bound( key.data(), key.length(), t );
            }

        typedef Loki::Functor< std::string, LOKI_TYPELIST_2( fh_database,
                                                        const std::string& ) > advance_functor_t;
    private:
        advance_functor_t m_advance_functor;
    public:
        static advance_functor_t makeAdvanceFunctorBytes();
        static advance_functor_t makeAdvanceFunctorNumeric();
        
        advance_functor_t& getAdvanceFunctor()
            {
                return m_advance_functor;
            }
        void setAdvanceFunctor( const advance_functor_t& f )
            {
                m_advance_functor = f;
            }
        
        iterator upper_bound_partial( const char* key, std::streamsize keylen, fh_trans t = 0 );
        iterator upper_bound_partial( const std::string& key, fh_trans t = 0 )
            {
                return upper_bound_partial( key.data(), key.length(), t );
            }
        std::pair<iterator, iterator>
        equal_range( const char* key, size_type keylen, fh_trans t = 0 );
        std::pair<iterator, iterator> equal_range(const key_type& k, fh_trans t = 0);

        std::pair<iterator, iterator>
        equal_range_partial( const char* key, size_type keylen, fh_trans t = 0 );
        std::pair<iterator, iterator> equal_range_partial(const key_type& k, fh_trans t = 0 );

        
        iterator insert( const char* key, std::streamsize keylen,
                         const char* data = 0, std::streamsize datalen = 0,
                         fh_trans t = 0 );
        iterator insert( const std::string& k, fh_trans t = 0 )
            {
                return insert( k.data(), k.length(), 0, 0, t );
            }
        iterator insert( const std::pair< std::string, std::string >& p, fh_trans t = 0 )
            {
                return insert( p.first.data(),  p.first.length(),
                               p.second.data(), p.second.length(), t );
            }
        template <class InputIterator>
        void insert(InputIterator it, InputIterator end, fh_trans t = 0)
            {
                while( it != end )
                    insert( *it, t );
            }
        
        DatabaseMutableValueRef operator[]( const std::string& k );
        DatabaseMutableValueRef operator[]( long k );


        size_type size() const;
        size_type max_size() const;
        bool empty() const;

        void erase( const char* key, size_type keylen, fh_trans t = 0 );
        void erase( iterator pos, fh_trans t = 0 );
        void erase(const key_type& k, fh_trans t = 0)
            {
                erase( k.data(), k.length(), t );
            }
        void erase(iterator iter, iterator last, fh_trans t = 0)
            {
                while( iter != last )
                {
                    iterator next = iter;
                    ++next;
                    erase( iter->first, t );
                    iter = next;
                }
            }
        void clear( fh_trans t = 0 )
            {
                erase( begin( t ), end( t ) );
            }
        size_type count( const char* key, size_type keylen, fh_trans t = 0 );
        size_type count( const key_type& k, fh_trans t = 0 )
            {
                return count( k.data(), k.length(), t );
            }
        
        
                
            
        /********************************************************************************/
        /********************************************************************************/
        /********************************************************************************/

        
    protected:
        DB* m_db;

        virtual void impl_open( const std::string& filename,
                                const std::string& database,
                                DBTYPE type,
                                u_int32_t flags,
                                int mode,
                                fh_trans t );

        iterator impl_find( const char* key,
                            std::streamsize keylen,
                            fh_trans t = 0,
                            bool throw_for_err = false );
        
        Ferris::fh_istream  impl_getIStream(  const char* key,  std::streamsize keylen,
                                              u_int32_t flags = 0, fh_trans t = 0 );
        Ferris::fh_iostream impl_getIOStream( const char* key,  std::streamsize keylen,
                                              u_int32_t flags = 0, fh_trans t = 0 );
        
        virtual fh_coredatachunk
        impl_getChunk( const char* key, int keylen, u_int32_t flags, fh_trans t = 0 );
        /**
         * It is assumed that data is of correct size obtained via impl_getValueSize()
         */
        virtual void
        impl_getChunk( const char* key, int keylen, char* data, int datalen, u_int32_t flags, fh_trans t = 0 );

        std::streamsize
        impl_getValueSize( const char* key, int keylen, u_int32_t flags, fh_trans t = 0 );

        virtual void
        impl_setChunk( const char* key,  std::streamsize keylen,
                       const char* data, std::streamsize datalen,
                       u_int32_t flags = 0,
                       fh_trans t = 0 );

    private:
        m_bt_compare_functor_t m_bt_compare_functor;

    public:
        typedef size_t (*bt_prefix_fcn)(DB *, const DBT *, const DBT *);

        void set_bt_prefix( bt_prefix_fcn f );
//         typedef Loki::Functor< size_t, LOKI_TYPELIST_3( DB*, const DBT *, const DBT * ) > m_bt_prefix_functor_t;
//         void set_bt_prefix( m_bt_prefix_functor_t f );
        
//     private:        
//         m_bt_prefix_functor_t m_bt_prefix_functor;
//         bt_prefix_fcn m_bt_prefix_fcn;

        /**
         * The DB->compact method compacts Btree and Recno access method databases
         */
        iterator compact( iterator b, iterator e, fh_trans t = 0 );
        iterator compact( fh_trans t = 0 )
            {
                return compact( end(), end(), t );
            }
        
        /**
         * See if this is a hash, btree, or whatever
         */
        DBTYPE  get_type();

        /**
         * Are range queries like equal_range_partial() valid
         */
        bool isSorted();

        /**
         * Is this stored into a BTree on disk
         */
        bool isBTree();
        
    };

    /**
     * Used as a temporary object by operator[] to allow the return value
     * to be mutable and effect the datastore when changed. Users of
     * STLdb4 shouldn't worry about this class.
     */
    struct FERRISEXP_API DatabaseMutableValueRef 
    {
        typedef DatabaseMutableValueRef _Self;

        fh_database        m_db;
        Database::iterator m_iter;
        fh_trans           m_trans;
        DatabaseMutableValueRef( fh_database db, Database::iterator iter, fh_trans t )
            :
            m_db( db ),
            m_iter( iter ),
            m_trans( t )
            {
            }
        
        DatabaseMutableValueRef& operator=( const std::string& v );
        bool operator!=( const std::string& v );
        bool operator==( const std::string& v );
        
        operator std::string() const
            {
                std::string v;
                m_iter.getValue( v );
                return v;
            }
    };
    
    template < class _CharT, class _Traits >
    std::basic_ostream< _CharT, _Traits >&
    operator<<( std::basic_ostream< _CharT, _Traits >& ss, const DatabaseMutableValueRef& p )
    {
        std::string v;
        p.m_iter.getValue( v );
        ss << v;
        return ss;
    }

    template
    <
        typename T,
        template <class> class OwnershipPolicy,
        class ConversionPolicy,
        template <class> class CheckingPolicy,
        template <class> class StoragePolicy
    >    
    DatabaseMutableValueRef
    DatabaseHandle<T, OwnershipPolicy, ConversionPolicy, CheckingPolicy, StoragePolicy>
    ::operator[]( const std::string& k )
                {
                    return this->operator->()->operator[]( k );
                }
    template
    <
        typename T,
        template <class> class OwnershipPolicy,
        class ConversionPolicy,
        template <class> class CheckingPolicy,
        template <class> class StoragePolicy
    >    
    DatabaseMutableValueRef
    DatabaseHandle<T, OwnershipPolicy, ConversionPolicy, CheckingPolicy, StoragePolicy>
    ::operator[]( long k )
                {
                    return this->operator->()->operator[]( k );
                }
    

    
    FERRISEXP_API Database::bt_compare_fcn getInt16Compare();
    FERRISEXP_API Database::bt_compare_fcn getUInt16Compare();
    FERRISEXP_API Database::bt_compare_fcn getInt32Compare();
    FERRISEXP_API Database::bt_compare_fcn getUInt32Compare();
    FERRISEXP_API Database::bt_compare_fcn getInt64Compare();
    FERRISEXP_API Database::bt_compare_fcn getUInt64Compare();
    FERRISEXP_API Database::bt_compare_fcn getFloatCompare();
    FERRISEXP_API Database::bt_compare_fcn getDoubleCompare();
    FERRISEXP_API Database::bt_compare_fcn getCISCompare();
    FERRISEXP_API Database::bt_compare_fcn getCSCompare();
    FERRISEXP_API Database::bt_compare_fcn getBinaryCompare();

    FERRISEXP_API Database::bt_compare_fcn getUInt16ThenBinaryCompare();

    FERRISEXP_API Database::m_bt_compare_functor_t makeReverseCompare( Database::m_bt_compare_functor_t& f );
    
    template < int offset, int len >
    int offsetLengthSecIdxFunction( DB* db, const DBT* pkey, const DBT* pdata, DBT* skey)
    {
        memset(skey, 0, sizeof(DBT));
        skey->data = (char*)pdata->data + offset;
        skey->size = std::min( len, (int)(pdata->size - offset) );
        return (0);
    }

    /**
     * Create a secondary index function that creates the secondary key from
     * the primary data starting at offset and of the given length or less
     * if the payload doesn't have enough bytes.
     *
     * see ut_secondary for an example usage
     */
    template < int offset, int len >
    Database::sec_idx_callback getOffsetLengthSecIdx()
    {
        return offsetLengthSecIdxFunction< offset, len >;
    }

    template < int offset >
    int offsetSecIdxFunction( DB* db, const DBT* pkey, const DBT* pdata, DBT* skey)
    {
        if( !skey || !pdata )
            return(0);
        if( pdata->size < offset )
            return( DB_DONOTINDEX );
        
        memset(skey, 0, sizeof(DBT));
        skey->data = (char*)pdata->data + offset;
        skey->size = pdata->size - offset;
        return (0);
    }

    template < int offset, int numberOfStringsToSkip >
    int offsetNullTerminatedSecIdxFunction( DB* db, const DBT* pkey, const DBT* pdata, DBT* skey)
    {
        if( !skey || !pdata )
            return(0);
        if( pdata->size < offset )
            return( DB_DONOTINDEX );

        char* end = (char*)pdata->data + pdata->size;
        memset(skey, 0, sizeof(DBT));
        skey->data = (char*)pdata->data + offset;
        skey->size = pdata->size - offset;

        for( int c = numberOfStringsToSkip; c > 0; --c )
        {
            char* p = std::find( (char*)skey->data, end, '\0' );
            skey->data = p+1;
            skey->size = std::distance( (char*)skey->data, end );
        }
        
        char* p = std::find( (char*)skey->data, end, '\0' );
        skey->size = std::distance( (char*)skey->data, p );

//         std::cerr << "offsetNullTerminatedSecIdxFunction() sz:" << skey->size
//                   << " data:" << (char*)skey->data
//                   << std::endl;
        return (0);
    }

    
    /**
     * Create a secondary index function that creates the secondary key from
     * the primary data starting at an offset and containing all data from there
     * onwards.
     *
     * Note for primary data items that are shorter in length than the offset
     * given such items are implicitly not secondary indexed by using DB_DONOTINDEX
     */
    template < int offset >
    Database::sec_idx_callback getOffsetSecIdx()
    {
        return offsetSecIdxFunction< offset >;
    }


    template < int offset, int numberOfStringsToSkip >
    Database::sec_idx_callback getOffsetNullTerminatedSecIdx()
    {
        return offsetNullTerminatedSecIdxFunction< offset, numberOfStringsToSkip >;
    }

    
    FERRISEXP_API Database::sec_idx_callback getFullValueSecIdx();
    
    
};
#endif
