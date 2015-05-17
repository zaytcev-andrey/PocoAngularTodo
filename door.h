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
          , int cost_basis
          , int locks_count
          , const std::string& first_lock_name
          , const std::string& second_lock_name
          , int construction_protection
          , const std::string& protection_class_name )
          : id_( id )
          , name_( name )
          , manufacturer_name_( manufacturer_name )
          , cost_basis_( cost_basis )
          , locks_count_( locks_count )
          , first_lock_name_( first_lock_name )
          , second_lock_name_( second_lock_name )
          , construction_protection_( construction_protection )
          , protection_class_name_( protection_class_name )
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

     int get_locks_count() const
     {
          return locks_count_;
     }

     std::string get_first_lock_name() const
     {
          return first_lock_name_;
     }

     std::string get_second_lock_name() const
     {
          return second_lock_name_;
     }

     int get_construction_protection() const
     {
          return construction_protection_;
     }

     std::string get_protection_class_name() const
     {
          return protection_class_name_;
     }
     
private:
     int id_;
     std::string name_;
     std::string manufacturer_name_;
     int cost_basis_;
     int locks_count_;
     std::string first_lock_name_;
     std::string second_lock_name_;
     int construction_protection_;
     std::string protection_class_name_;
};

#endif // door_h__