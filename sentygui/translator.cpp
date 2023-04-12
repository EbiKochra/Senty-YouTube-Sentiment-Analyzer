// /* translater.cpp
// ** DESC: 
// */
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "request.h"

#include "translator.h"

using json = nlohmann::json;

Translator::Translator() {}

//  Deconstructor
Translator::~Translator() {}

//  DESC: Determines whether or not a comment needs translation
//  PARAM: Text input of the comment to be added
//  RETURN: True if it can be translated from another language, False if not (e.g., already in English or not a language in the API)
bool Translator::detect(std::string text) {
    //  Convert the Text string into a json value
    json body = json::array({ {{"Text", text}} });
    requestBody = body.dump();

    // Microsoft Azure Translation API url
    std::string endpoint = "https://api.cognitive.microsofttranslator.com";
    std::string route = "/detect?api-version=3.0";
    std::string url = (endpoint + route).c_str();

    request = Request();
    if (request.executeTln(url, requestBody)) { // could fetche API
        std::string result = request.result();
        json response = request.getJson();
        if (response.contains("error")) {   // could not detect the lagnauge
            std::string errorMessage = response["error"]["message"];
            return false;
        }
        originLang = response[0]["language"];
        canTranslate = response[0]["isTranslationSupported"];
        if (canTranslate == true) { // The language is supported
            return true;
        }
    } 
    return false;
}


bool Translator::translate(std::string text) {
    if (canTranslate == true) { // Translatable
        // API links
        std::string endpoint = "https://api.cognitive.microsofttranslator.com";
        std::string route = "/translate?api-version=3.0&from="+ originLang +"&to=en";
        std::string url = (endpoint + route).c_str();

        request = Request();

        if (request.executeTln(url, requestBody)) { // API was reached
            std::string result = request.result();
            json response = request.getJson();
            if (response.contains("error")) {   // Could not get translation
                std::string errorMessage = response["error"]["message"];
                return false;
            }
            // get resulting text
            doneText = response[0]["translations"][0]["text"];
            return true;
        }
    }
    return false;

}

//  DESC: get the results of the translation
//  PARAM: NAN, using translation result
//  RETURN: translated text
std::string Translator::getTranslated() const {
    return doneText;
}

//  DESC: get the language of the original 
//  PARAM: NAN, using detect result
//  RETURN: detected text original language
std::string Translator::getOriginLang() const {
    return originLang;
}

