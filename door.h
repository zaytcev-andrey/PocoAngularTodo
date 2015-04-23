#ifndef door_h__
#define door_h__

#include "modifiable_common.h"

#include <Poco/SharedPtr.h>

class door : public modifiable_common
{
public:
     typedef Poco::SharedPtr< door > shared_ptr;

     door( int id
          , const std::string& name
          , const std::string& manufacturer_name
          , int cost_basis )
          : id_( id )
          , name_( name )
          , manufacturer_name_( manufacturer_name )
          , cost_basis_( cost_basis )
     {
     }

     int get_id() const
     {
          return id_;
     }

     std::string get_name() const
     {
          return name_;
     }

     std::string get_manufacturer_name() const
     {
          return manufacturer_name_;
     }

     int get_cost_basis() const
     {
          return cost_basis_;
     }

private:
     const int id_;
     const std::string name_;
     const std::string manufacturer_name_;
     const int cost_basis_;
};

#endif // door_h__