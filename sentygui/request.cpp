/* request.cpp
** DESC: used to make HTTP requests. This code will store a string response from a specified url into memory.
   Calls to get in memory are supported by getinmemory.c, by Daniel Stenberg <daniel@haxx.se>, et al.
   Comments for writeMemoryCallback written by Daniel Stenberg
   Code used from nlohmann/json, available on Github.
*/

#include <iostream>
#include <string>
#include <cstring>
#include <curl/curl.h>
#include <regex>
#include <stdexcept>
#include <nlohmann/json.hpp>


#include "request.h"


using json = nlohmann::json;


using namespace std;


// -------------- RequestException -----------------
// RequestException::RequestException(string message) : message{message} { }

// std::string RequestException::what() const {
//     return message;
// }
RequestException::RequestException(RequestError error) : error{error} { }

std::string RequestException::what() const {
    switch(error) {
        case RequestError::VideoDNE:
            return "Video with this link does not exist.";
        case RequestError::NotYoutubeLink:
            return "Error 403: This is not a youtube link, or you don't have the permission to access it.";
        case RequestError::NoLink:
            return "Cannot have empty input.";
        case RequestError::NoPermission:
            return "Cannot have empty input.";
        case RequestError::Unknown:
            return "Unrecognized error from Youtube API";
    
    }
    return "";
}


/* CONSTRUCTORS + DESTRUCTOR
** 1. Constructor takes url as a string parameter.
   2. Destructor
*/

Request::Request() {
    // y_link = "https://api.cognitive.microsofttranslator.com";

}

Request::Request(const std::string& url) : url{url} {
    y_link = url;
    if (y_link == "") {
        throw RequestException(RequestError::NotYoutubeLink);
    }
    // linkType = checkLink();
}

Request::~Request() {}

/* FUNC NAME: writeMemoryCallback
** DESC: This is used by curl to write data returned from server.
** PARAM DESC: contents = pointer to data block recieved from HTTP response, 
   size = size of each element in contents, nmemb = # elements in contents, 
   and userp = user-defined data (MemoryStruct) are descriptors of the memory.
** RETURN DESC: returns the size of the data written into memory as a size_t type = size*nmemb
*/
size_t Request::writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = (char*) realloc(mem->chunk, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
  mem->chunk = ptr;
  memcpy(&(mem->chunk[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->chunk[mem->size] = 0;

  return realsize;
}

// size_t Request::writeMemoryCallback(char *contents, size_t size, size_t nmemb, void *userp) {
//     size_t realsize = size * nmemb;
//     MemoryStruct *mem = (MemoryStruct *)userp;
//     mem->chunk.append(contents, realsize);
//     return realsize;
// }

/* FUNC NAME: execute
** DESC: makes a get request to the specified url using curl. This stores it in memory. 
** PARAM DESC: no parameters are in the function itself, but it is of the request object, 
   which will have url.
** RETURN DESC: returns true if the request was able to execute. else returns false, and
   an error message.
*/
int Request::execute() {
    CURL *curl_handle;
    CURLcode res;
    MemoryStruct memory;

    memory.chunk = (char *)malloc(1);  /* will be grown as needed by the realloc above */
    memory.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, Request::writeMemoryCallback);
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&memory);
    /* some servers do not like requests that are made without a user-agent
        field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    res = curl_easy_perform(curl_handle);

    /* check for errors */
    if(res != CURLE_OK) {
        errorMsg = "curl_easy_perform() failed: " + std::string(curl_easy_strerror(res));
        response = "";
        curl_easy_cleanup(curl_handle);
        free(memory.chunk);
        return -1;
    } else {
        errorMsg = "";
        response.assign (memory.chunk, strlen(memory.chunk));
    }

    jResponse = json::parse(response);
    // std::cout << jResponse.dump(2) << std::endl;

    if (jResponse.contains("error") ) {
        try {
            if (jResponse["error"].contains("code") && jResponse["error"]["code"] == 403) {
                // std::cout << jResponse["error"]["message"] << std::endl;
                throw RequestException(RequestError::NoPermission);
                // return 0;
            } else if (jResponse["error"].contains("code") && jResponse["error"]["code"] == 404) {
                // std::cout << jResponse["error"]["message"] << std::endl;
                throw RequestException(RequestError::VideoDNE);
                // return 0;
            }

        } catch (exception &e) {
            throw RequestException(RequestError::Unknown);
        }
        
    }
 
    curl_easy_cleanup(curl_handle);

    free(memory.chunk);

    /* we are done with libcurl, so clean it up */
    curl_global_cleanup();
    // std::cout << "here6" << std::endl;

    return 1;
}

/* FUNC NAME: executeTLN
** DESC: Same as execute, but specifically for the Translate API
** RETURN DESC: returns true if the request was able to execute. else returns false, and
   an error message.
*/
bool Request::executeTln(std::string url, std::string requestBody) {
    MemoryStruct tlnmemory;
    tlnmemory.chunk = (char *)malloc(1);  /* will be grown as needed by the realloc above */
    tlnmemory.size = 0;    /* no data at this point */
    std::string key = "5716e569e7d4441c82b5d3f39c75b329";
    const std::string location = "canadacentral";

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return "fail1";
    }
    //  Headers for translator
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Ocp-Apim-Subscription-Key: " + key).c_str());
    headers = curl_slist_append(headers, ("Ocp-Apim-Subscription-Region: " + location).c_str());
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, (url).c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Request::writeMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&tlnmemory);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Failed to perform request: " << curl_easy_strerror(res) << std::endl;
        response = "";
        free(tlnmemory.chunk);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return false;
        
    }  else {
        errorMsg = "";
        response.assign (tlnmemory.chunk, strlen(tlnmemory.chunk));
    }

    jResponse = json::parse(response);
    
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    return true;
}


json Request::getJson() {
    return jResponse;
}

/* FUNC NAME: getErrorMsg
** DESC: The Request object has a string errorMsg. This will return it
** PARAM DESC: This is from the Request object
** RETURN DESC: The errorMsg will be "" if there is nothing.
*/
std::string Request::getErrorMsg() const {
    return errorMsg;
}

/* FUNC NAME: result
** DESC: This will give us the string contents of the URL that we have input
** PARAM DESC: Result object, which has url
** RETURN DESC: A string response of the contents of our Request
*/
std::string Request::result() const {
    return response;
}
std::string Request::getLink() const {
    return y_link;
}


//  Get video's ID for API
std::string Request::extractVideoID() {
    // std::string video_id;
    std::regex regex_model("^(https?://)?(www\\.)?(m\\.)?(youtube\\.com/.*[?&]v=([^&]+)|(youtu\\.be/)?(([^&]+)))");
    std::smatch match;
    // std:cout << match[5].str() << std::endl;
    try {
        if (std::regex_search(y_link, match, regex_model)) {
            if (y_link.find("youtu.be") != std::string::npos) {
                // std:cout << match[0].str() << match[1].str() << match[2].str() << match[3].str() << std::endl;
                y_id = match[7].str();
            } else {
                y_id = match[5].str();
            }
            // std:cout << y_id << std::endl;
        }
    } catch (exception &e) {
        throw RequestException(RequestError::NotYoutubeLink);
    }
    // } else {
    //     std::cerr << "Error: " << y_link << " is an invalid YouTube Link." << std::endl;
    //     throw std::runtime_error("invalid link 2");
    // }
    return y_id;
}
