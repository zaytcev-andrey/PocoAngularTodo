#ifndef i_modifiable_h__
#define i_modifiable_h__

// forward declaration
namespace Poco
{
     class DateTime;
}

class imodifiable 
{
public:
     virtual ~imodifiable() {}

     virtual const Poco::DateTime& getLastModifiedTime() const = 0;
};

#endif // i_modifiable_h__