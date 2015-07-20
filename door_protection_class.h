#ifndef door_protection_class_h__
#define door_protection_class_h__

class door_protection_class : public modifiable_common 
{
public:
     typedef Poco::SharedPtr< door_protection_class > shared_ptr;

     door_protection_class( int id
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

#endif // door_protection_class_h__