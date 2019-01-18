#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

void handle_get(http_request message) {
   
    cout << "Method: " << message.method() << std::endl;
 	cout << "URI: " << http::uri::decode(message.relative_uri().path()) << std::endl;

    message.extract_json()
            .then([=](json::value value){
            message.reply(status_codes::OK, "ACCEPTED");
        })
            .wait();
}


int main(){
	
	string_t address = U("http://172.17.0.4:5000/test");
    http_listener listener(address);
    listener.support(methods::GET, handle_get);
    listener.open().wait();

    std::cout << "Press ENTER to exit." << std::endl;

    std::string line;
    std::getline(std::cin, line);

    listener.close().wait();

    return 0;
}
