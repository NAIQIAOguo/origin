#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

// *******************************************************
#include <stdio.h>
#include <cpprest/uri.h>
#include <cpprest/http_listener.h>
#include <cpprest/asyncrt_utils.h>
#pragma comment(lib, "cpprest_2_7.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "httpapi.lib")

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class CommandHandler
{
public:
    CommandHandler() {}
    CommandHandler(utility::string_t url);
    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }
private:
    void handle_get_or_post(http_request message);
    http_listener m_listener;
};

CommandHandler::CommandHandler(utility::string_t url) : m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&CommandHandler::handle_get_or_post, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&CommandHandler::handle_get_or_post, this, std::placeholders::_1));
}

void CommandHandler::handle_get_or_post(http_request message)
{
    ucout << "Method: " << message.method() << std::endl;
    ucout << "URI: " << http::uri::decode(message.relative_uri().path()) << std::endl;
    ucout << "Query: " << http::uri::decode(message.relative_uri().query()) << std::endl << std::endl;
    message.reply(status_codes::OK, "ACCEPTED");
};

// ***************************************************************************************

std::vector<std::string> split(std::string str,std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str+=pattern;//扩展字符串以方便操作
    int size=str.size();

    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}


int main(){    

    utility::string_t address = U("http://172.17.0.4:");
    address.append(U("5000"));
    web::uri_builder uri(address);
    uri.append_path(U("naiqiao"));
    std::string addr = uri.to_uri().to_string();
    web::http::experimental::listener::http_listener listener(addr);

    listener.support(web::http::methods::GET, [](web::http::http_request request) {
 
// ***********************************************************************************
        //get string 
        ucout << "Method: " << request.method() << std::endl;
        ucout << http::uri::decode(request.request_uri().path()) <<std::endl;
        ucout << "URI: " << http::uri::decode(request.relative_uri().path()) << std::endl;

        string filename = "index.html";  
        ifstream file(filename);  
        stringstream buffer;  
        buffer << file.rdbuf(); 

        web::uri res = request.relative_uri(); 
        string res_str = res.to_string();

        if (res_str == "/"){ 
            string filename = "index.html";  
            ifstream file(filename);  
            stringstream buffer;  
            buffer << file.rdbuf();
            request.reply(web::http::status_codes::OK, buffer.str(), "text/html"); 

        }

//ajax part
        std::string query_name = "eashan";
        std::cout<<"********************************" <<res_str.find("/data")<<std::endl;

        if (res_str.find("/data") == 0){
        
            string filename = "index.html";  
            ifstream file(filename);  
            stringstream buffer;  
            buffer << file.rdbuf();
            
                    //search from MongoDB
            mongocxx::instance inst{};
            string username = "naiqiao";
            string pass = "nq123456";
    
            mongocxx::client conn {
                mongocxx::uri{"mongodb://"+username+":"+pass+"@ds227674.mlab.com:27674/naiqiao2"}
            };

            auto collection = conn["naiqiao2"]["cse411"];
            
            auto cursor = collection.find(bsoncxx::builder::stream::document{}
                                              << "query" << query_name << bsoncxx::builder::stream::finalize);

            std::string message;
            for (auto doc : cursor){
                std::cout << bsoncxx::to_json(doc) << std::endl;
                message = message + bsoncxx::to_json(doc);
            }


            request.reply(web::http::status_codes::OK,U(message));

        }

// ***********************************************************************************
        //search from MongoDB
        // mongocxx::instance inst{};
        // string username = "naiqiao";
        // string pass = "nq123456";
    
        // mongocxx::client conn {
        //     mongocxx::uri{"mongodb://"+username+":"+pass+"@ds227674.mlab.com:27674/naiqiao2"}
        // };

        // auto collection = conn["naiqiao2"]["cse411"];

        // std::string query_name = "man";
        // auto cursor = collection.find(bsoncxx::builder::stream::document{}
        //                                       << "query" << query_name << bsoncxx::builder::stream::finalize);

        // for (auto doc : cursor){
        //     std::cout << bsoncxx::to_json(doc) << std::endl;
        // }

// ************************************************************************************
        // send the seraching outcome from db to html
// ************************************************************************************

        // request.reply(web::http::status_codes::OK, buffer.str(), "text/html");

    });

    listener
    .open()
    .then([]() {
        std::cout << "Listening on http://localhost:5000/naiqaio...\n";
    })
    .wait();
    std::string line;
    std::getline(std::cin, line);
    listener.close().wait();
    return 0;
}


