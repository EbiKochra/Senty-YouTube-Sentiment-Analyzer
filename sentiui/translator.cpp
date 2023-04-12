/**
 * @file translator.cpp
 * @brief Implementation of the Translator class.
 */
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "request.h"

#include "translator.h"

using json = nlohmann::json;
/**
 * @brief Default constructor for Translator.
 */
Translator::Translator() {}

/**
 * @brief Destructor for Translator.
 */
Translator::~Translator() {}

/**
 * @brief Determines whether a comment needs translation.
 * @param text Text input of the comment to be analyzed.
 * @return True if it can be translated from another language, False if not (e.g., already in English or not a language in the API).
 */
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

/**
 * @brief Translate the given text if it is translatable.
 * @param text Text input of the comment to be translated.
 * @return True if the text was successfully translated, False otherwise.
 */
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

/**
 * @brief Get the results of the translation.
 * @return Translated text as a string.
 */
std::string Translator::getTranslated() const {
    return doneText;
}

/**
 * @brief Get the language of the original text.
 * @return Detected text's original language as a string.
 */
std::string Translator::getOriginLang() const {
    return originLang;
}


