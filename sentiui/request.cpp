/**
 * @brief utilizes curl to make HTTP requests and store response in memory
   @authors Adrian, Emily, Karanvir, Muhammad, Salar
 */

/*
 * DESC: used to make HTTP requests. This code will store a string response from a specified url into memory.
 * Calls to get in memory are supported by getinmemory.c, by Daniel Stenberg <daniel@haxx.se>, et al.
 * Comments for writeMemoryCallback written by Daniel Stenberg
 * Code used from nlohmann/json, available on Github.
 * Group: 26
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


// Constructor
Request::Request() {
    // y_link = "https://api.cognitive.microsofttranslator.com";

}

Request::Request(const std::string& url) : url{url} {
    y_link = url;
    // linkType = checkLink();
}
// Destructor
Request::~Request() {}


/**
 * @brief This is used by curl to write data returned from API
 * @param *contents, size, nmemb, *userp
 * @returns returns the size_t real size of the data written into memory
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
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


/**
 * @brief executes the request at the specified url, stores to memory
 * @param none
 * @returns true if request was executed, else, returns false and an error message
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 */
bool Request::execute() {
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
        return false;
    } else {
        errorMsg = "";
        response.assign (memory.chunk, strlen(memory.chunk));
    }

    jResponse = json::parse(response);
    // std::cout << jResponse.dump(2) << std::endl;

    if (jResponse.find("error") != jResponse.end()) {
        throw std::runtime_error("video DNE. Potentially private, deleted, removed, or incorrect link.");
    }
 
    curl_easy_cleanup(curl_handle);

    free(memory.chunk);

    /* we are done with libcurl, so clean it up */
    curl_global_cleanup();
    // std::cout << "here6" << std::endl;

    return true;
}


/**
 * @brief similar to execute; executes requests to the Translation API
 * @param url, requestBody
 * @returns true if request was executed, false and an error message if not
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
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

/**
 * @brief getter for jResponse attribute
 * @param none
 * @returns jResponse
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 */
json Request::getJson() {
    return jResponse;
}


/**
 * @brief getter for errorMsg attribute
 * @param none
 * @returns errorMsg
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 */
std::string Request::getErrorMsg() const {
    return errorMsg;
}


/**
 * @brief returns the response from the api as a string
 * @param none
 * @returns response
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 */
std::string Request::result() const {
    return response;
}

/**
 * @brief getter for y_link attribute
 * @param none
 * @returns y_link
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 */
std::string Request::getLink() const {
    return y_link;
}


/**
 * @brief gets the videoId from the given url
 * @param none
 * @returns videoID as a string
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 */
std::string Request::extractVideoID() {
    // std::string video_id;
    std::regex regex_model("^(https?://)?(www\\.)?(m\\.)?(youtube\\.com/.*[?&]v=([^&]+)|(youtu\\.be/)?(([^&]+)))");
    std::smatch match;
    // std:cout << match[5].str() << std::endl;
    if (std::regex_search(y_link, match, regex_model)) {
        if (y_link.find("youtu.be") != std::string::npos) {
            // std:cout << match[0].str() << match[1].str() << match[2].str() << match[3].str() << std::endl;
            y_id = match[7].str();
        } else {
            y_id = match[5].str();
        }
        // std:cout << y_id << std::endl;
    } else {
        std::cerr << "Error: " << y_link << " is an invalid YouTube Link." << std::endl;
        throw std::runtime_error("invalid link 2");
    }

    return y_id;
}
