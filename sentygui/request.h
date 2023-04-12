/* request.h
** DESC: This code defines game.cpp. It defines a class Request, which holds 1 game 
   against 2 teams (i.e., 1 away team and 1 home team).

   PRIV: teamName, score
   PUBL: 3 constructors, 1 deconstructor, setTeamName(), 
   setScore(), getTeamName(), getScore()
*/

// GUARDS
#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

#include <regex>

using json = nlohmann::json;


enum RequestError {VideoDNE, NotYoutubeLink, NoLink, NoPermission, Unknown};

class RequestException {
        RequestError error;
    public:
        RequestException(RequestError error);
        std::string what() const;
};

// class RequestException {
//         std::string message;
//     public:
//         RequestException(std::string message);
//         std::string what() const;
// };




class Request {
    private: 
        // stores request information
        struct MemoryStruct {
            char *chunk;
            std::size_t size;
        };
        std::string url;
        std::string response;
        std::string errorMsg;
        std::string y_link;
        std::string y_id;
        json jResponse;

        static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

    public: 
        // Constructor
        Request(const std::string& url);
        Request();

        // Destructor
        ~Request();

        int execute();
        bool executeTln(std::string url, std::string requestBody);
        std::string result() const;

        std::string getErrorMsg() const;
        json getJson();
        std::string getLink() const;
        std::string extractVideoID();
};

#endif 
