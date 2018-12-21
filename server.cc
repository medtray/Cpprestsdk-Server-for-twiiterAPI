/**
 
 * at any time, the client program can be run.  It will take a
 * command line parameter, which is a search term, and it will issue a query
 * against the Twitter API.  It will process the result set as follows:
 *     Produce a histogram from the top 100 results, matching number of tweets
 *     to dates.  Save this in a MongoDB collection called "hist".  The query
 *     term should be the document name.
 * - Note: query terms that are user handles (@name) or hashtags (#tag) will
 *   naturally show user activity or topic trends
 * - Note: authentication for the Twitter API uses OAuth.
 *
 * The server program should do three things.
 *   - It should serve an index.html file in response to GET /
 *   - It should serve other files in response to GET /files/<filename>
 *   - It should serve JSON, pulled from MongoDB, in response to GET /data/term
 * With regard to the third of these tasks, "term" should be a search term, and
 * the response should be a JSON representation of the histogram for that term,
 * as determined by the client.  If no histogram exists, then the JSON response
 * should be empty.
 *
 * The HTML5 component is a single-page web app that allows the user to
 * enter a search term.  In response to that term, the page will issue an AJAX
 * request to the server to get some JSON data, and then it should use that JSON
 * data to initialize a chart. 
 *
 * the program use Microsoft's cpprestsdk. 
 */


#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "twitcurl.h"
#include <jsoncpp/json/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
#include <stdio.h>

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <chrono>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/detail/ptree_utils.hpp>
#include <boost/property_tree/detail/file_parser_error.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <boost/program_options/option.hpp>
#include <boost/asio.hpp>
using namespace std;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;


using boost::property_tree::ptree;
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


using namespace std; 
namespace fs = boost::filesystem;

#include <iostream>
#include <map>
#include <set>
#include <string>
using namespace std;
#define TRACE(msg)  cout << msg


map<string, string> months
    {
        { "Jan", "01" },
        { "Feb", "02" },
        { "Mar", "03" },
        { "Apr", "04" },
        { "May", "05" },
        { "Jun", "06" },
        { "Jul", "07" },
        { "Aug", "08" },
        { "Sep", "09" },
        { "Oct", "10" },
        { "Nov", "11" },
        { "Dec", "12" }
    };

int already_authentified=0;
 mongocxx::instance inst{};
mongocxx::client conn{mongocxx::uri{""}};

boost::filesystem::path full_path(boost::filesystem::current_path());
string current_path_of_server=full_path.string();
string path_of_project = current_path_of_server.substr(0, current_path_of_server.find_last_of("/\\"));

twitCurl twitterObj;

 

string parse_tweets(string input)

{
Json::Value root;
Json::Reader reader;
reader.parse(input, root);

string tweet_stream="{";

int tweet_index=0;

for (Json::Value::ArrayIndex i = 0; i != root.size(); i++)
{
    tweet_index++;
    if (root[i].isMember("created_at"))
    {
        
        string tweet_number="tweet"+to_string(tweet_index);
        tweet_stream+="\""+tweet_number+"\""+":\""+root[i]["created_at"].asString()+"\",";
    }
            
}
tweet_stream = tweet_stream.substr(0, tweet_stream.size()-1);
    tweet_stream += '}';

    return tweet_stream;
}



void handle_get(web::http::http_request request) {
using namespace std;
using namespace web::http;
auto relativePath = uri::decode(request.relative_uri().path());
std::vector<utility::string_t> path = uri::split_path(relativePath);
  if (!path.empty()) {

   
         if (path[0] == "data") {

             if (path.size()==4)
             {

                



auto session = conn.start_session();

    bsoncxx::builder::stream::document document{};

    auto collection = conn["Project5"]["Twitter"];
    string id;
    
    if (path[2]=="1")
      id="#"+path[1];
    else
      id=path[1];
   

    using bsoncxx::stdx::string_view;

    auto cursor = collection.find(session, make_document(kvp("_id", id)));
    string exist;
    for (auto&& doc : cursor) {
        exist= bsoncxx::to_json(doc);
    }


std::string json = exist;
  // Read json.
  ptree pt2;
  std::istringstream is (json);
  read_json (is, pt2);
  std::string foo = pt2.get<std::string> ("twitter");
  ptree pt3;
  std::istringstream iss (foo);
  read_json (iss, pt3);
  multimap<string,int> mymm;
  multimap<string,int>::iterator it;
  
  vector<string> result;
  string field;

  for (int i=1;i<=pt3.size();i++)
  {
      string tweet="tweet"+to_string(i);
      std::string value = pt3.get<std::string> (tweet);
     // cout<<value;
          istringstream iss(value);

     vector<string> tokens{istream_iterator<string>{iss},
                      istream_iterator<string>{}};

                      std::multimap<string,string>::iterator itt = months.find(tokens[1]);

   // string field= tokens[1]+" "+tokens[2]+" "+tokens[5];
        if (path[3]=="year")
            field= tokens[5];

        if (path[3]=="monthly")
            field= tokens[5]+"/"+itt->second;

        if (path[3]=="daily")
            field= tokens[5]+"/"+itt->second+"/"+tokens[2];
        
        else if(path[3]=="hour")
        {
            
            string hour=tokens[3].substr(0,2);
            field= tokens[5]+"/"+itt->second+"/"+tokens[2]+"-"+hour;
        }

        else if(path[3]=="minute")
        {
            
            string hour_minute=tokens[3].substr(0,5);

            field= tokens[5]+"/"+itt->second+"/"+tokens[2]+"-"+hour_minute;
        }

        else if (path[3]=="second")
            field= tokens[5]+"/"+itt->second+"/"+tokens[2]+"-"+tokens[3];



      std::multimap<string,int>::iterator it = mymm.find(field);

      if (it == mymm.end())
    {
         mymm.insert(pair<string,int>(field,1));
    }
    else{
        it->second+=1;
    }
      

  }
ptree root;
string json_result="{";
// show content:
  for (std::multimap<string,int>::iterator it=mymm.begin(); it!=mymm.end(); ++it)
  {
      root.put((*it).first, (*it).second);
      json_result+="\""+(*it).first+"\""+":"+"\""+to_string((*it).second)+"\",";

  }
    json_result = json_result.substr(0, json_result.size()-1);
    json_result+="}";

    request.reply(status_codes::OK, json_result, "text");


}

else
  {
   
request.reply(status_codes::OK, "Not found", "text/html");

  }
         }

else if (path[0] == "files")
{

    if (path.size()==2)
    {

 ifstream file(path_of_project+"/FilesInServer/"+path[1]);
stringstream buffer;
buffer << file.rdbuf();
if((buffer.str()).size()>0)
  request.reply(status_codes::OK, buffer.str(), "text/plain"); 
else
  request.reply(status_codes::OK, "File Not Found", "text/plain");

}
else
  {
   
request.reply(status_codes::OK, "Not found", "text/html");

  }
}

else if (path[0] == "mongodb")
{

    if (path.size()==3)
    {

  
    
    char tmpBuf[1024];
    std::string replyMsg;
if(already_authentified!=1)
{

    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( "" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "" ) );

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

    
    twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
    twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    

        
    /* OAuth flow ends */

    /* Account credentials verification */
    if( twitterObj.accountVerifyCredGet() )
    {
        twitterObj.getLastWebResponse( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet web response:\n%s\n", replyMsg.c_str() );
        already_authentified=1;
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet error:\n%s\n", replyMsg.c_str() );
    }

}

    
    string query=path[1];

    string id;
    
    string tweet_stream;
    

    if (path[2]=="1")
    {
        id="#"+query;

        if( twitterObj.search( id, "100" ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        replyMsg = replyMsg.substr(12, replyMsg.size());

        tweet_stream=parse_tweets(replyMsg);
       // printf( "\ntwitterClient:: twitCurl::search web response:\n%s\n", replyMsg.c_str() );
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::search error:\n%s\n", replyMsg.c_str() );
    }  




    }
    else
    {
      id=query;
      if (query[0]=='@')

      {
                  string query_for_api= query.substr(1, query.size());


           printf( "\nGetting user timeline\n" );
    if( twitterObj.timelineUserGet( true, false, 100,query_for_api,false ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        tweet_stream=parse_tweets(replyMsg);
       // printf( "\ntwitterClient:: twitCurl::timelineUserGet web response:\n%s\n", replyMsg.c_str() );
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::timelineUserGet error:\n%s\n", replyMsg.c_str() );
    } 



      }


      else
      {


         if( twitterObj.search( id, "100" ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        replyMsg = replyMsg.substr(12, replyMsg.size());

        tweet_stream=parse_tweets(replyMsg);
       // printf( "\ntwitterClient:: twitCurl::search web response:\n%s\n", replyMsg.c_str() );
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::search error:\n%s\n", replyMsg.c_str() );
    }  

      }








    }

   // cout<<tweet_stream;
    //std::system(command.c_str());

    if (tweet_stream=="}")
    {
                request.reply(status_codes::OK, "Couldn't find tweets for the given query", "text/plain");

    }

    else
    {
   
    auto session = conn.start_session();

    bsoncxx::builder::stream::document document{};

    auto collection = conn["Project5"]["Twitter"];

    std::string str=tweet_stream;
    document << "_id"<<id <<"twitter" << str;

   

    using bsoncxx::stdx::string_view;

     auto cursor = collection.find(session, make_document(kvp("_id", id)));
    int element_exist=0;

    for (auto&& doc : cursor) {
        string exist= bsoncxx::to_json(doc);
        if (exist.length()>0)
        {
            element_exist=1;
            collection.delete_one(session, make_document(kvp("_id", id)));
            collection.insert_one(document.view());
            request.reply(status_codes::OK, "done updating", "text/plain");
        }
    } 

    if (element_exist==0) 
    {
        collection.insert_one(document.view());
        request.reply(status_codes::OK, "done inserting", "text/plain");
    }
      
    }
      
 
     

}
else
  {
   
request.reply(status_codes::OK, "Not found", "text/html");

  }
  }
    

else
  {
   
request.reply(status_codes::OK, "Not found", "text/html");

  }



  }

  else
  {
    ifstream file(path_of_project+"/index2.html");
stringstream buffer;
buffer << file.rdbuf();
request.reply(status_codes::OK, buffer.str(), "text/html");

  }
}

int main(int argc, char *argv[]) {
  



  http_listener listener("http://localhost:8080");
 
   listener.support(methods::GET, [](http_request req) { handle_get(req); });

   try
   {
      listener
         .open()
         .then([&listener](){TRACE("\nstarting to listen\n");})
         .wait();
 
      while (true);
   }
   catch (exception const & e)
   {
      cout << e.what() << endl;
   }
 
   return 0;
}
