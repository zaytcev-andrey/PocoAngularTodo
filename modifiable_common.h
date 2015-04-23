#ifndef modifiable_common_h__
#define modifiable_common_h__

#include "i_modifiable.h"

#include <Poco/DateTime.h>

class modifiable_common : public imodifiable
{
public:

     modifiable_common()
          : last_modified_time_( Poco::DateTime() )
     {}

     virtual const Poco::DateTime& getLastModifiedTime() const 
     { 
          return last_modified_time_; 
     }

protected:

     virtual void updateLastModifiedTime() 
     { 
          last_modified_time_ = Poco::DateTime(); 
     }

     virtual void setLastModifiedTime( const Poco::DateTime& date_time )
     {
          last_modified_time_ = date_time;
     }

private:
     Poco::DateTime last_modified_time_;
};

#endif // modifiable_common_h__