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

int create_door( 
     soci::session& sql_session 
     , const std::wstring& name
     , int cost_basis
     , int manufacturer_id )
{       
     try
     {
          const std::string utf8_name = boost::spirit::to_utf8( name );

          sql_session << "insert into doors (name, cost_basis, manufacturer_id) \
                         values (:name, :cost_basis, :manufacturer_id)",
                         soci::use( utf8_name ),
                         soci::use( cost_basis ),
                         soci::use( manufacturer_id );

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
                         FOREIGN KEY(manufacturer_id) REFERENCES doors_manufacturers(id) )";

          sql_session_ << "CREATE TABLE doors_manufacturers ( id INTEGER PRIMARY KEY  NOT NULL, \
                          name TEXT NOT NULL )";
     }

     tr.commit();
}

void doors_db::init_by_default()
{
     soci::transaction tr( sql_session_ );
     {
          int last_man_id = 
               create_door_manufacturer( sql_session_, L"Производитель №1" );
          {
               create_door( sql_session_, L"Слабая дверь", 10, last_man_id );
               create_door( sql_session_, L"Среднияя дверь", 12, last_man_id );
               create_door( sql_session_, L"Прочная дверь", 20, last_man_id );
          }

          last_man_id =
          create_door_manufacturer( sql_session_, L"Производитель №2" );
          {
               create_door( sql_session_, L"Слабая дверь", 15, last_man_id );
               create_door( sql_session_, L"Среднияя дверь", 20, last_man_id );
               create_door( sql_session_, L"Прочная дверь", 40, last_man_id );
          }

          last_man_id = 
          create_door_manufacturer( sql_session_, L"Производитель №3" );
          {
               create_door( sql_session_, L"Слабая дверь", 20, last_man_id );
               create_door( sql_session_, L"Среднияя дверь", 30, last_man_id );
               create_door( sql_session_, L"Прочная дверь", 40, last_man_id );
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
                          doors_manufacturers.name as man_name \
                          from \
                          doors, doors_manufacturers \
                          where \
                          doors.manufacturer_id = doors_manufacturers.id" );

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

               door::shared_ptr item( new door( id, name, man_name, cost_basis ) );

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