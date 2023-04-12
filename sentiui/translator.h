/**
 * @file translator.h
 * @brief Header file for the Translator class.
 */

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <iostream>
#include <string>
#include "request.h"

class Translator {
private:
    Request request;
    std::string endpoint;
    std::string key;
    std::string body;
    std::string originLang;
    std::string targetLang = "en";
    std::string doneText;
    std::string requestBody;
    bool canTranslate = true;

public:
    /**
     * @brief Default constructor for Translator.
     */
    Translator();

    /**
     * @brief Destructor for Translator.
     */
    ~Translator();

    /**
     * @brief Determines whether a comment needs translation.
     * @param text Text input of the comment to be analyzed.
     * @return True if it can be translated from another language, False if not (e.g., already in English or not a language in the API).
     */
    bool detect(std::string text);

    /**
     * @brief Translate the given text if it is translatable.
     * @param text Text input of the comment to be translated.
     * @return True if the text was successfully translated, False otherwise.
     */
    bool translate(std::string text);

    /**
     * @brief Get the results of the translation.
     * @return Translated text as a string.
     */
    std::string getTranslated() const;

    /**
     * @brief Get the language of the original text.
     * @return Detected text's original language as a string.
     */
    std::string getOriginLang() const;
};

#endif // TRANSLATOR_H

