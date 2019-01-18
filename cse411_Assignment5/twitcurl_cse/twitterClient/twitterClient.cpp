#include <iostream>
#include "twitterClient.h"
#include <jsoncpp/json/json.h>
using namespace std;

#include <cstdint>
#include <iostream>
#include <vector>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

int main( int argc, char* argv[] )
{    
    twitCurl twitterObj;
    std::string tmpStr, tmpStr2;
    std::string replyMsg;
    char tmpBuf[1024];

    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( "D8jxZe6w9VfT9E1dltFOzR0th" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "ifnp6tOJRfwIHcl9yo0sEhdA39tZ7GNRyfAxTEdVa2lBaEkJVp" ) );

    /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn;

    oAuthTokenKeyIn.open( "twitterClient_token_key.txt" );
    oAuthTokenSecretIn.open( "twitterClient_token_secret.txt" );

    memset( tmpBuf, 0, 1024 );
    oAuthTokenKeyIn >> tmpBuf;
    myOAuthAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    oAuthTokenSecretIn >> tmpBuf;
    myOAuthAccessTokenSecret = tmpBuf;

    oAuthTokenKeyIn.close();
    oAuthTokenSecretIn.close();

    if( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
        /* If we already have these keys, then no need to go through auth again */
        printf( "\nUsing:\nKey: %s\nSecret: %s\n\n", myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str() );
        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    }

    /* Search a string */
    printf( "\nEnter string to search: " );
    memset( tmpBuf, 0, 1024 );
    fgets( tmpBuf, sizeof( tmpBuf ), stdin );
    tmpStr = tmpBuf;
    printf( "\nLimit search results to: " );
    memset( tmpBuf, 0, 1024 );
    fgets( tmpBuf, sizeof( tmpBuf ), stdin );
    tmpStr2 = tmpBuf;
    replyMsg = "";
    if( twitterObj.search( tmpStr, tmpStr2 ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        // printf( "\ntwitterClient:: twitCurl::search web response:\n%s\n", replyMsg.c_str() );
        
        Json::Reader reader;
        Json::Value val;
        //step 1 using json

        mongocxx::instance inst{};
        string username = "naiqiao";
        string pass = "nq123456";
    
        mongocxx::client conn {
            mongocxx::uri{"mongodb://"+username+":"+pass+"@ds227674.mlab.com:27674/naiqiao2"}
        };

        auto collection = conn["naiqiao2"]["cse411"];

        if(reader.parse(replyMsg,val)){

            const Json::Value arrayObj = val["statuses"];

            cout<<"**************************************************"<<endl;
            for (unsigned int i = 0; i < arrayObj.size(); i++){
                // cout<< arrayObj[i] <<endl;
                const Json::Value element = arrayObj[i]["text"];
                // cout<<element<<endl;
                string text = element.asString();
                cout<< text <<endl;
                bsoncxx::builder::stream::document document{};
                document << "text" << text;
                collection.insert_one(document.view());
            }
            cout<<"**************************************************"<<endl;

            cout<<"**************************************************"<<endl;
            auto cursor = collection.find({});
            for (auto&& doc : cursor) {
                std::cout << bsoncxx::to_json(doc) << std::endl;
            }
            // cout<< val <<endl;
        }
        //step 2 insert into Mongodb
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::search error:\n%s\n", replyMsg.c_str() );
    }

    return 0;
}






















