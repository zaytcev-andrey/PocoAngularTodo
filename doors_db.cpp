#include "stdafx.h"

#include "doors_db.h"

#include <Poco/Path.h>
#include <Poco/File.h>

#include <boost/spirit/home/support/utf8.hpp>

#include <iterator>

namespace
{

int create_door_manufacturer( 
     soci::session& sql_session 
     , const std::wstring& name )
{       
     try
     {
          const std::string utf8_name = boost::spirit::to_utf8( name );
          
          sql_session << "insert into doors_manufacturers (name) values (:name)",
               soci::use( utf8_name );

          int last_id = 0;
          sql_session << "select last_insert_rowid()",
               soci::into( last_id );

          return last_id;
     }
     catch ( const soci::soci_error& error )
     {
          std::cerr << "db error: " << error.what() << std::endl;
          throw;
     }
}

int create_door_protection_class( 
                                 soci::session& sql_session 
                                 , const std::wstring& name )
{       
     try
     {
          const std::string utf8_name = boost::spirit::to_utf8( name );

          sql_session << "insert into door_protection_classes (name) values (:name)",
               soci::use( utf8_name );

          int last_id = 0;
          sql_session << "select last_insert_rowid()",
               soci::into( last_id );

          return last_id;
     }
     catch ( const soci::soci_error& error )
     {
          std::cerr << "db error: " << error.what() << std::endl;
          throw;
     }
}

int create_locks_manufacturers( 
                               soci::session& sql_session 
                               , const std::wstring& name )
{       
     try
     {
          const std::string utf8_name = boost::spirit::to_utf8( name );

          sql_session << "insert into locks_manufacturers (name) values (:name)",
               soci::use( utf8_name );

          int last_id = 0;
          sql_session << "select last_insert_rowid()",
               soci::into( last_id );

          return last_id;
     }
     catch ( const soci::soci_error& error )
     {
          std::cerr << "db error: " << error.what() << std::endl;
          throw;
     }
}

int create_door_locks( 
                soci::session& sql_session 
                , const std::wstring& name
                , int protection_score
                , int lock_manufacturer_id )
{       
     try
     {
          const std::string utf8_name = boost::spirit::to_utf8( name );

          sql_session << "insert into door_locks (name, protection_score, lock_manufacturer_id) \
                         values (:name, :protection_score, :lock_manufacturer_id)",
                         soci::use( utf8_name ),
                         soci::use( protection_score ), 
                         soci::use( lock_manufacturer_id );

          int last_id = 0;
          sql_session << "select last_insert_rowid()",
               soci::into( last_id );

          return last_id;
     }
     catch ( const soci::soci_error& error )
     {
          std::cerr << "db error: " << error.what() << std::endl;
          throw;
     }
}

int create_door( 
                soci::session& sql_session 
                , const std::wstring& name
                , int cost_basis
                , int manufacturer_id
                , int locks_count
                , int first_lock_id
                , int second_lock_id
                , int construction_protection
                , int overall_quality
                , int protection_class_id)
{
     try
     {
          const std::string utf8_name = boost::spirit::to_utf8( name );

          sql_session << "insert into doors \
                         (name, cost_basis, manufacturer_id, \
                         locks_count, first_lock_id, second_lock_id, \
                         construction_protection, overall_quality, \
                         protection_class_id) \
                         values (:name, :cost_basis, :manufacturer_id, \
                         :locks_count, :first_lock_id, :second_lock_id, \
                         :construction_protection, :overall_quality, \
                         :protection_class_id)",
                         soci::use( utf8_name ),
                         soci::use( cost_basis ),
                         soci::use( manufacturer_id ),
                         soci::use( locks_count ),
                         soci::use( first_lock_id ),
                         soci::use( second_lock_id ),
                         soci::use( construction_protection ),
                         soci::use( overall_quality ),
                         soci::use( protection_class_id );

          int last_id = 0;
          sql_session << "select last_insert_rowid()",
               soci::into( last_id );

          return last_id;
     }
     catch ( const soci::soci_error& error )
     {
          std::cerr << "db error: " << error.what() << std::endl;
          throw;
     }
}

}



doors_db::doors_db( const std::string& storage_path )
     : storage_path_( storage_path )
     , db_name_( "items.db" )
{
     Poco::Path full_path( Poco::Path( storage_path ), db_name_ );

     const std::string utf8_full_path = 
          Poco::Path::transcode( full_path.toString() );

     const std::string db_path_string = 
          std::string( "db=" ) + "\"" + utf8_full_path + "\"";

     const std::string timeout = "timeout=10";

     const std::string connection_str =  db_path_string + " " + timeout;

     sql_session_.open( soci::sqlite3, connection_str.c_str() );

     const Poco::File doors_db_file( full_path.toString() );
     
     if ( doors_db_file.getSize() == 0 )
     {
          create_db();

          init_by_default();
     }
}

doors_db::~doors_db()
{
     sql_session_.close();
}

void doors_db::create_db()
{
     soci::transaction tr( sql_session_ );
     {
          sql_session_ << "CREATE TABLE doors ( id INTEGER PRIMARY KEY  NOT NULL, \
                         name TEXT NOT NULL , manufacturer_id INTEGER NOT NULL, \
                         cost_basis INTEGER NOT NULL, \
                         locks_count INTEGER NOT NULL, \
                         first_lock_id INTEGER NOT NULL, \
                         second_lock_id INTEGER, \
                         construction_protection INTEGER NOT NULL, \
                         overall_quality INTEGER NOT NULL, \
                         protection_class_id INTEGER NULL, \
                         FOREIGN KEY(manufacturer_id) REFERENCES doors_manufacturers(id), \
                         FOREIGN KEY(first_lock_id) REFERENCES door_locks(id), \
                         FOREIGN KEY(second_lock_id) REFERENCES door_locks(id), \
                         FOREIGN KEY(protection_class_id) REFERENCES door_protection_classes(id) )";
          

          sql_session_ << "CREATE TABLE doors_manufacturers ( id INTEGER PRIMARY KEY  NOT NULL, \
                          name TEXT NOT NULL )";

          sql_session_ << "CREATE TABLE door_locks ( id INTEGER PRIMARY KEY  NOT NULL, \
                          name TEXT NOT NULL, \
                          protection_score INTEGER NOT NULL, \
                          lock_manufacturer_id INTEGER NOT NULL, \
                          FOREIGN KEY(lock_manufacturer_id) REFERENCES locks_manufacturers(id) )";

          sql_session_ << "CREATE TABLE locks_manufacturers ( id INTEGER PRIMARY KEY  NOT NULL, \
                          name TEXT NOT NULL )";

          sql_session_ << "CREATE TABLE door_protection_classes ( id INTEGER PRIMARY KEY  NOT NULL, \
                          name TEXT NOT NULL )";
     }

     tr.commit();
}

void doors_db::init_by_default()
{
     soci::transaction tr( sql_session_ );
     {
          const int first_protection_class = 
               create_door_protection_class( sql_session_, L"1 Класс защиты" );
          const int second_protection_class = 
               create_door_protection_class( sql_session_, L"2 Класс защиты" );
          const int third_protection_class = 
               create_door_protection_class( sql_session_, L"3 Класс защиты" );
          
          int locks_man = 
               create_locks_manufacturers( sql_session_, L"Производитель замков №1" );
          const int first_lock = 
               create_door_locks( sql_session_, L"Замок №1", 10, locks_man );

          locks_man = 
               create_locks_manufacturers( sql_session_, L"Производитель замков №2" );
          const int second_lock = 
               create_door_locks( sql_session_, L"Замок №2", 15, locks_man );

          locks_man = 
               create_locks_manufacturers( sql_session_, L"Производитель замков №3" );
          const int third_lock = 
               create_door_locks( sql_session_, L"Замок №3", 20, locks_man );

          const int not_exist = -1;
          
          int last_man_id = 
               create_door_manufacturer( sql_session_, L"Производитель №1" );
          {
               create_door( sql_session_, L"Слабая дверь", 10, last_man_id
                    , 1, first_lock, not_exist, 10, not_exist, first_protection_class );
               create_door( sql_session_, L"Среднияя дверь", 12, last_man_id
                    , 2, first_lock, first_lock, 15, not_exist, second_protection_class );
               create_door( sql_session_, L"Прочная дверь", 20, last_man_id
                    , 2, first_lock, third_lock, 30, not_exist, third_protection_class );
          }

          last_man_id =
          create_door_manufacturer( sql_session_, L"Производитель №2" );
          {
               create_door( sql_session_, L"Слабая дверь", 15, last_man_id
                    , 1, first_lock, not_exist, 12, not_exist, first_protection_class );
               create_door( sql_session_, L"Среднияя дверь", 20, last_man_id
                    , 2, first_lock, second_lock, 20, not_exist, second_protection_class );
               create_door( sql_session_, L"Прочная дверь", 40, last_man_id
                    , 2, second_lock, third_lock, 40, not_exist, third_protection_class );
          }

          last_man_id = 
          create_door_manufacturer( sql_session_, L"Производитель №3" );
          {
               create_door( sql_session_, L"Слабая дверь", 20, last_man_id
                    , 2, first_lock, second_lock, 10, not_exist, second_protection_class );
               create_door( sql_session_, L"Среднияя дверь", 20, last_man_id
                    , 2, second_lock, second_lock, 30, not_exist, second_protection_class );
               create_door( sql_session_, L"Прочная дверь", 40, last_man_id
                    , 2, second_lock, third_lock, 45, not_exist, third_protection_class );
          }
     }

     tr.commit();
}

doors_db::shared_doors doors_db::get_doors() const
{
     try
     {    
          soci::rowset< soci::row > rs = (
          sql_session_.prepare << "select doors.id as id, \
                          doors.name as name, \
                          doors.cost_basis as cost_basis, \
                          doors_manufacturers.name as man_name, \
                          doors.locks_count, \
                          ( select door_locks.name from door_locks where door_locks.id = doors.first_lock_id ) as first_lock_name, \
                          ( select door_locks.name from door_locks where door_locks.id = doors.second_lock_id ) as second_lock_name, \
                          doors.construction_protection, \
                          door_protection_classes.name as protection_class_name \
                          from \
                          doors, \
                          doors_manufacturers, \
                          door_protection_classes \
                          where \
                          ( doors.manufacturer_id = doors_manufacturers.id ) and \
                          ( doors.protection_class_id = door_protection_classes.id )" );

          //const size_t rows_count = std::distance( rs.begin(), rs.end() );
          shared_doors doors( new std::vector< door::shared_ptr >() );
          //doors->reserve( rows_count );

          for ( soci::rowset< soci::row >::const_iterator it = rs.begin(); it != rs.end(); ++it )
          {
               const soci::row& row = *it;
               
               const int id = it->get< int >( 0 );
               const std::string name = it->get< std::string >( 1 );
               const int cost_basis = it->get< int >( 2 );
               const std::string man_name = it->get< std::string >( 3 );
               const int locks_count = it->get< int >( 4 );
               const std::string first_lock_name = it->get< std::string >( 5 );
               const soci::indicator ind = it->get_indicator( "second_lock_name" );
               const std::string second_lock_name = ( ind == soci::i_ok ? it->get< std::string >( 6 ) : "" );
               const int construction_protection = it->get< int >( 7 );
               const std::string protection_class_name = it->get< std::string >( 8 );

               door::shared_ptr item( new door( id, name, man_name, cost_basis
                    , locks_count, first_lock_name, second_lock_name
                    , construction_protection, protection_class_name ) );

               doors->push_back( item );
          }

          return doors;
     }
     catch( const soci::soci_error& err )
     {
          std::cerr << "db error: " << err.what() << std::endl;
          throw;
     }
}

doors_db::shared_doors doors_db::get_doors( int cost_basis_min, int cost_basis_max ) const
{
     try
     {
          soci::rowset< soci::row > rs = (
               sql_session_.prepare << "select doors.id as id, \
                                       doors.name as name, \
                                       doors.cost_basis as cost_basis, \
                                       doors_manufacturers.name as man_name \
                                       doors.locks_count, \
                                       ( select door_locks.name from door_locks where door_locks.id = doors.first_lock_id ) as first_lock_name, \
                                       ( select door_locks.name from door_locks where door_locks.id = doors.second_lock_id ) as second_lock_name, \
                                       doors.construction_protection, \
                                       door_protection_classes.name as protection_class_name \
                                       from \
                                       doors, \
                                       doors_manufacturers, \
                                       door_protection_classes \
                                       where \
                                       ( doors.manufacturer_id = doors_manufacturers.id ) and \
                                       ( doors.protection_class_id = door_protection_classes.id ) and \
                                       ( doors.cost_basis between :cost_basis_min and :cost_basis_max )"
                                       , soci::use( cost_basis_min ), soci::use( cost_basis_max ) );

          //const size_t rows_count = std::distance( rs.begin(), rs.end() );
          shared_doors doors( new std::vector< door::shared_ptr >() );
          //doors->reserve( rows_count );

          for ( soci::rowset< soci::row >::const_iterator it = rs.begin(); it != rs.end(); ++it )
          {
               const soci::row& row = *it;

               const int id = it->get< int >( 0 );
               const std::string name = it->get< std::string >( 1 );
               const int cost_basis = it->get< int >( 2 );
               const std::string man_name = it->get< std::string >( 3 );
               const int locks_count = it->get< int >( 4 );
               const std::string first_lock_name = it->get< std::string >( 5 );
               const soci::indicator ind = it->get_indicator( "second_lock_name" );
               const std::string second_lock_name = ( ind == soci::i_ok ? it->get< std::string >( 6 ) : "" );
               const int construction_protection = it->get< int >( 7 );
               const std::string protection_class_name = it->get< std::string >( 8 );

               door::shared_ptr item( new door( id, name, man_name, cost_basis
                    , locks_count, first_lock_name, second_lock_name
                    , construction_protection, protection_class_name ) );

               doors->push_back( item );
          }

          return doors;
     }
     catch( const soci::soci_error& err )
     {
          std::cerr << "db error: " << err.what() << std::endl;
          throw;
     }
}

doors_db::shared_doors_manufacturers doors_db::get_doors_manufacturers() const
{
     try
     {
          soci::rowset< soci::row > rs = (
               sql_session_.prepare << "select doors_manufacturers.id as id, \
                                       doors_manufacturers.name as man_name \
                                       from \
                                       doors_manufacturers" );

          //const size_t rows_count = std::distance( rs.begin(), rs.end() );
          shared_doors_manufacturers doors_manufacturers( new std::vector< doors_manufacturer::shared_ptr >() );
          //doors->reserve( rows_count );

          for ( soci::rowset< soci::row >::const_iterator it = rs.begin(); it != rs.end(); ++it )
          {
               const soci::row& row = *it;

               const int id = it->get< int >( 0 );
               const std::string name = it->get< std::string >( 1 );

               doors_manufacturer::shared_ptr item( new doors_manufacturer( id, name ) );

               doors_manufacturers->push_back( item );
          }

          return doors_manufacturers;
     }
     catch( const soci::soci_error& err )
     {
          std::cerr << "db error: " << err.what() << std::endl;
          throw;
     }
}