#ifndef doors_db_h__
#define doors_db_h__

#include "door.h"
#include "doors_manufacturer.h"
#include "door_protection_class.h"

#include <string>
#include <vector>

#include <soci/soci.h>
#include <soci/soci-sqlite3.h>

// TODO: implements class params for get doors

class doors_db
{
public:
     typedef Poco::SharedPtr< std::vector< door::shared_ptr > > shared_doors;
     typedef Poco::SharedPtr< std::vector< doors_manufacturer::shared_ptr > > shared_doors_manufacturers;
     typedef Poco::SharedPtr< std::vector< door_protection_class::shared_ptr > > shared_door_protection_classes;
     
     doors_db( const std::string& storage_path );
     ~doors_db();

      shared_doors get_doors() const;
      shared_doors get_doors( int cost_basis_min, int cost_basis_max ) const;

      shared_doors_manufacturers get_doors_manufacturers() const;
      shared_door_protection_classes get_protection_classes() const;

private:

     void create_db();
     void init_by_default();

private:
     std::string db_name_;
     std::string storage_path_;

     mutable soci::session sql_session_;
};

#endif // doors_db_h__