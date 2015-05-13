#pragma once

#include <Poco/Mutex.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Path.h>
#include <Poco/ScopedLock.h>
#include <Poco/StringTokenizer.h>
#include <Poco/URI.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/SharedPtr.h>
#include <Poco/DateTime.h>

#include "modifiable_common.h"
#include "doors_db.h"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using namespace std;

/**
    Todo
*/
class CTodo : public modifiable_common
{
    size_t id;
    string text;
public:
     explicit CTodo( const std::string& text)
         : text(text)
    {
    }
    /* getters & setters */
    size_t getId() const { return id; }
    void setId(size_t id)
    { 
         this->id = id;
         updateLastModifiedTime();
    }
    std::string getText() const { return text; }
};

typedef Poco::SharedPtr<CTodo> TodoPtr;

/**
    Список Todo
*/
class CTodoList : public modifiable_common
{
    size_t id;
    map<size_t, TodoPtr> todos;

public:
    CTodoList():id(0){}
    /* CRUD */
    void create( TodoPtr todo)
    { 
         todo->setId(++id); 
         todos.insert( std::pair< size_t, TodoPtr >( id, todo ) );
         setLastModifiedTime( todo->getLastModifiedTime() );

    }
    map<size_t, TodoPtr>& readList(){ return todos; }
    TodoPtr del( size_t id )
    { 
         TodoPtr removed = todos[ id ];
         todos.erase( id );
         updateLastModifiedTime();
         return removed;
    }
};

class CRemovedTodoList : public modifiable_common
{
     size_t id;
     map<size_t, TodoPtr> todos;

public:
     /* CRUD */
     void insert( TodoPtr todo)
     { 
          todos[ todo->getId() ] = todo;
          updateLastModifiedTime();
     }
     map<size_t, TodoPtr>& readList(){ return todos; }
};

/**
    Сервер
*/
class TodoServerApp : public ServerApplication
{
public:
    TodoServerApp( const std::string& storage_path );
     
    /* CRUD */
    static void createTodo( TodoPtr todo);
    static CTodoList& readTodoList();
    //static void updateTodo(size_t id, CTodo& todo);
    static void deleteTodo(size_t id);

    static doors_db::shared_doors readDoorsList();
    static doors_db::shared_doors readDoorsList( int cost_basis_min, int cost_basis_max );
    static doors_db::shared_doors_manufacturers readDoorsManufacturers();

protected:
    int main(const vector<string> &);
    static Mutex todoLock;
    static CTodoList todoList;
    static CRemovedTodoList deletedTodos;
    static Poco::SharedPtr< doors_db > doors_storage_;
};
