// /* translater.h
// ** DESC: 
// */

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
        Translator();
        ~Translator();
        bool detect(std::string text);
        bool translate(std::string text);
        std::string getTranslated() const;
        std::string getOriginLang() const;


};

#endif // TRANSLATOR_H

