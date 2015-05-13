#ifndef doors_manufacturer_h__
#define doors_manufacturer_h__

#include "modifiable_common.h"

#include <Poco/SharedPtr.h>

class doors_manufacturer : public modifiable_common 
{
public:
     typedef Poco::SharedPtr< doors_manufacturer > shared_ptr;

     doors_manufacturer( int id
          , const std::string& name )
          : id_( id )
          , name_( name )
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

private:
     const int id_;
     const std::string name_;
};

#endif // doors_manufacturer_h__
