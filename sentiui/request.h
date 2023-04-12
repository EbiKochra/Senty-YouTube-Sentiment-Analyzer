/**
 * @brief Request class header
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
*/
// Desc: header for the request class of the Senty program
//Group: 26

// GUARDS
#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

#include <regex>

using json = nlohmann::json;


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

        bool execute();
        bool executeTln(std::string url, std::string requestBody);
        std::string result() const;
        std::string extractVideoID();

        //setters
        std::string getErrorMsg() const;
        json getJson();
        std::string getLink() const;

};

#endif 
