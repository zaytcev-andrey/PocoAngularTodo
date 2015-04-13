#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <iterator>
#include <algorithm>

#include <Poco/DateTimeParser.h>

#include "TodoServerApp.h"

Poco::Mutex TodoServerApp::todoLock;
CTodoList TodoServerApp::todoList;
CRemovedTodoList TodoServerApp::deletedTodos;

ostream& operator<<(ostream& os, const Poco::DateTime& date_time )
{
     os << date_time.second();
     return os;
}

ostream& operator<<(ostream& os, TodoPtr todo)
{
    os << "{ \"_id\": "<< todo->getId() << 
         ", \"text\": \"" << todo->getText() << "\"" <<
         ", \"last_modified_time\": " << todo->getLastModifiedTime() << " }";
    return os;
}

ostream& operator<<(ostream& os, CTodoList& todoList)
{
    map<size_t, TodoPtr> todos = todoList.readList();

    os << "[";
    if(!todos.empty())
    {
        if(todos.size() == 1)
            os << todos.begin()->second;
        else
            for ( map<size_t, TodoPtr>::iterator it = todos.begin();;)
            {
                os << it->second;
                if(++it != todos.end())
                    os << ',';
                else
                    break;
            }

    }
    os << "]\n";

    return os;
}

// implements REST api
class CTodoHandler : public HTTPRequestHandler
{
public:
    void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
    {
        URI uri(req.getURI());
        string method = req.getMethod();

        cerr << "URI: " << uri.toString() << endl;
        cerr << "Method: " << req.getMethod() << endl;

        if ( req.has( "If-Modified-Since" ) )
        {                          
             const std::string if_modified_since = req.get( "If-Modified-Since" );
             cerr << "If-Modified-Since :" << if_modified_since << endl;
             
             // must be under the mutex
             const CTodoList& todos = TodoServerApp::readTodoList();
             const Poco::DateTime& modified = todos.getLastModifiedTime();

             Poco::DateTime modifiedSince;
             int tzd;
             Poco::DateTimeParser::parse( if_modified_since, modifiedSince, tzd );

             if (modified <= modifiedSince)
             {
                  resp.setContentLength( 0 );
                  resp.setStatusAndReason( Poco::Net::HTTPResponse::HTTP_NOT_MODIFIED );
                  resp.send();

                  return;
             }
        }

        StringTokenizer tokenizer(uri.getPath(), "/", StringTokenizer::TOK_TRIM);
        HTMLForm form(req,req.stream());

        if(!method.compare("POST"))
        {
            cerr << "Create:" << form.get("text") << endl;
            TodoPtr todo( new CTodo( form.get("text") ) );
            TodoServerApp::createTodo( todo );
        }
        else if(!method.compare("PUT"))
        {
            cerr << "Update id:" << *(--tokenizer.end()) << endl;
            cerr << "Update text:" << form.get("text") << endl;
            //size_t id=stoull(*(--tokenizer.end()));
            //TodoServerApp::updateTodo(id, form.get("text"));
        }
        else if(!method.compare("DELETE"))
        {
            cerr << "Delete id:" << *(--tokenizer.end()) << endl;
            const std::string id_str = *(--tokenizer.end());
            std::istringstream str_stream( id_str );
            size_t erase_id = 0;
            str_stream >> erase_id;
            TodoServerApp::deleteTodo( erase_id );
        }

        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("application/json");
        //resp.setContentLength(  );

        ostream& out = resp.send();

        cerr << TodoServerApp::readTodoList() << endl;
        out << TodoServerApp::readTodoList() << endl;

        out.flush();
    }
};

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <map>          // std::ifstream

// implements getting files - htmpl.index, app.js
class CFileHandler : public HTTPRequestHandler
{
    typedef std::map<const std::string, std::string> TStrStrMap;
    TStrStrMap CONTENT_TYPE;

    string getPath(string& path){

        if(path == "/"){
            path="/index.html";
        }

        path.insert(0, "./www");

        return path;
    }

    string getContentType(string& path){

        string contentType("text/plain");
        Poco::Path p(path);

        TStrStrMap::const_iterator i=CONTENT_TYPE.find(p.getExtension());

        if (i != CONTENT_TYPE.end())
        { /* Found, i->first is f, i->second is ++-- */
           contentType = i->second;
        }

        if(contentType.find("text/") != std::string::npos)
        {
            contentType+="; charset=utf-8";
        }

        cerr << path << " : " << contentType << endl;

        return contentType;
    }

public:

     CFileHandler()
     {
          struct conv_pair
          {
               std::string first_;
               std::string second_;
          };

          conv_pair ContentTypeArray[] = {
                    #include "MimeTypes.h" 
          };

          const size_t types_len = _countof(ContentTypeArray);

          for ( size_t idx = 0; idx < types_len; idx++ )
          {
               const std::string& key = ContentTypeArray[ idx ].first_;
               const std::string& val = ContentTypeArray[ idx ].second_;
               CONTENT_TYPE[ key ] = val;
          }
     }

    void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
    {
        cerr << "Get static page: ";
        //system("echo -n '1. Current Directory is '; pwd");

        URI uri(req.getURI());
        string path(uri.getPath());

        ifstream ifs (getPath(path).c_str(), ifstream::in);

        if(ifs)
        {
            resp.setStatus(HTTPResponse::HTTP_OK);
            resp.setContentType(getContentType(path));
            ostream& out = resp.send();

            std::noskipws( ifs );
            std::istream_iterator< char > ifs_it( ifs );
            std::istream_iterator< char > ifs_eof;
            std::ostream_iterator< char > out_it( out );

            std::copy( ifs_it, ifs_eof, out_it );                     

            out.flush();
        }
        else
        {
            resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
            ostream& out = resp.send();

            out << "File not found" << endl;

            out.flush();
        }

        ifs.close();
    }
};

class TodoRequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
    virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest & request)
    {
        if (!request.getURI().find("/api/"))
            return new CTodoHandler;
        else
            return new CFileHandler;
    }
};

void TodoServerApp::createTodo( TodoPtr todo )
{
    ScopedLock<Mutex> lock(todoLock);
    todoList.create(todo);
}

CTodoList& TodoServerApp::readTodoList()
{
    ScopedLock<Mutex> lock(todoLock);
    return todoList;
}

void TodoServerApp::deleteTodo(size_t id)
{
    ScopedLock<Mutex> lock(todoLock);
    TodoPtr removed = todoList.del(id);
    deletedTodos.insert( removed );
}

int TodoServerApp::main(const vector<string> &)
{
    HTTPServerParams* pParams = new HTTPServerParams;

    pParams->setMaxQueued(100);
    pParams->setMaxThreads(16);

    HTTPServer s(new TodoRequestHandlerFactory, ServerSocket(8000), pParams);

    s.start();
    cerr << "Server started" << endl;

    waitForTerminationRequest();  // wait for CTRL-C or kill

    cerr << "Shutting down..." << endl;
    s.stop();

    return Application::EXIT_OK;
}


