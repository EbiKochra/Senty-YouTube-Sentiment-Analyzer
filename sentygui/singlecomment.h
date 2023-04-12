/*  comment.h
    GROUP: group26
    DESC: ___
*/

// Guards
#ifndef SINGLECOMMENT_H 
#define SINGLECOMMENT_H 

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <map>
#include <iostream>

#include "request.h"
#include "analysis.h"


using json = nlohmann::json;

using namespace std;

class SingleComment {
        
    public:

        SingleComment();
        ~SingleComment();

        void setSingleComment(const nlohmann::json& singleComment);

        void setConclusion();
        string getId() const;
        string getAuthor() const;
        string getCommenterProfileImageUrl() const;
        string getText() const;
        int getLikeCount() const;
        string getPublishedAt() const;
        string getUpdatedAt() const;
        map<string, double> getCommentScore() const;
        bool getIsEn() const;
        pair<string, string> getConclusion() const;

    private:
        Analysis analysis;
        // Analysis overallScore;
        string y_url;

        // SingleCommentInfo c;

        string id;
        string author;
        string commenterProfileImageUrl;
        string text;
        int likeCount;
        string publishedAt;
        string updatedAt;
        map<string, double> commentScore;
        string intensity;
        string direction;
        bool isEn;
        pair<string, string> conclusion;
        // string id;
        // string author;
        // string commenterProfileImageUrl;
        // string text;
        // int likeCount;
        // string publishedAt;
        // string updatedAt;
        // Score commentScore;
        // string intensity;
        // string direction;
        // bool isEn;
        // pair<string, string> conclusion;

};

#endif 
