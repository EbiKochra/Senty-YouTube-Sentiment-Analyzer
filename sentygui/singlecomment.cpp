/*  comment.cpp
    GROUP: group26
    DESC: ___
*/

#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <regex>
#include <stdexcept>

#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"

// #include "include/vader/vaderSentiment.hpp"
// #include "include/vader/SentiText.hpp"
// #include "include/vader/SentimentIntensityAnalyzer.hpp"
#include <nlohmann/json.hpp>
#include <map>

#include "singlecomment.h"

#include "request.h"
#include "translator.h"

using json = nlohmann::json;
using namespace std;

//  Constructor
SingleComment::SingleComment() {}

//  Deconstructor
SingleComment::~SingleComment() {}

//  DESC: creates a single comment object that will hold all the information needed in the comment
//  PARAM: location of json value
//  RETURN: the object of a single comment
void SingleComment::setSingleComment(const nlohmann::json& singleComment) {
    analysis = Analysis();
    // singleComment = SingleComment();
    // Analysis analysis;
    // SingleComment::SingleCommentInfo c;
    try {
        // cout << "singlecomment here" << endl;
        // cout << singleComment.dump(2) << endl;
        auto idCat = singleComment.find("id");
        if (idCat != singleComment.end()) { // There is an ID to the comment, meaning there is a comment
            id = singleComment["id"];
            author = singleComment["snippet"]["topLevelComment"]["snippet"]["authorDisplayName"];
            commenterProfileImageUrl = singleComment["snippet"]["topLevelComment"]["snippet"]["authorProfileImageUrl"];
            text = singleComment["snippet"]["topLevelComment"]["snippet"]["textOriginal"];
            // cout << text << endl;
            likeCount = singleComment["snippet"]["topLevelComment"]["snippet"]["likeCount"];
            publishedAt = singleComment["snippet"]["topLevelComment"]["snippet"]["publishedAt"];
            updatedAt = singleComment["snippet"]["topLevelComment"]["snippet"]["updatedAt"];
            analysis.setAnalysis(text);
            commentScore = analysis.getScoreNum();
            // cout << text << endl;
            // cout << commentScore["compound"] << endl;
            if (commentScore["compound"] == 0.0) { // Potentially, compound = 0 because it's in a different language
                Translator t;
                //  Check if the comment is in a different language
                isEn = t.detect(text);
                
                if (isEn && t.translate(text)) {    // it can be translated to english from another language
                    //  Set the translation as the gingle comment's text
                    text = t.getTranslated();
                    analysis.setAnalysis(text);
                    //  Get the new score of the translated single comment
                    commentScore = analysis.getScoreNum();
                }
            }
            //  Set direction and polarity
            setConclusion();
            // return c;
        }
    } catch (exception e){
        cerr << "Could not create single comment object" << endl;
    }
    // return c;
    
}

//  DESC: determines the direction and polarity of the comment
//  PARAM: Single comment object
//  RETURN: NAN, sets conclusion direction and intensity of single comment
void SingleComment::setConclusion() {
    //  map of the range of a single comment
    map<double, pair<string, string>> sentimentMap = {
        {-1.00, {"negative", "very"}},
        {-0.50, {"negative", "quite"}},
        {-0.05, {"neutral", "slightly negative"}},
        {0.00, {"neutral", "very"}},
        {0.20, {"neutral", "slightly positive"}},
        {0.35, {"positive", "quite"}},
        {0.50, {"positive", "very"}}
    };
    //  Compound score dtermines direction and polarity
    double compound = commentScore["compound"];

    //  Compare compound against the values in the map
    auto it = sentimentMap.upper_bound(compound);
    --it;
    //  Set up pair with direction and polarity
    pair<string, string> sentimentInfo = it->second;
    //  Set that pair as the conclusion of the object
    conclusion = make_pair(sentimentInfo.first, sentimentInfo.second);
}


string SingleComment::getId() const {
    return id;
}

string SingleComment::getAuthor() const {
    return author;
}

string SingleComment::getCommenterProfileImageUrl() const {
    return commenterProfileImageUrl;
}

string SingleComment::getText() const {
    return text;
}

int SingleComment::getLikeCount() const {
    return likeCount;
}

string SingleComment::getPublishedAt() const {
    return publishedAt;
}

string SingleComment::getUpdatedAt() const {
    return updatedAt;
}

map<string, double> SingleComment::getCommentScore() const {
    return commentScore;
}

// string SingleComment::getIntensity() const {
//     return conclusion.second;
// }

// string SingleComment::getDirection() const {
//     return conclusion.first;
// }

bool SingleComment::getIsEn() const {
    return isEn;
}

pair<string, string> SingleComment::getConclusion() const {
    return conclusion;
}

