#include "stdafx.h"

#include "TodoServerApp.h"

#include <Poco/Path.h>

#include <string>

int _tmain(int argc, _TCHAR* argv[])
{
     const std::string self_str( argv[ 0 ] );
     const Poco::Path self_path( self_str );
     
     TodoServerApp app( self_path.parent().toString() );

     return app.run(argc, argv);
}
