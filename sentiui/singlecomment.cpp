/**
 * @file comment.cpp
 * @ingroup group26
 * @brief Definition of the SingleComment class and its methods.
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
#include <nlohmann/json.hpp>
#include <map>

#include "singlecomment.h"

#include "request.h"
#include "translator.h"

using json = nlohmann::json;

/**
 * @brief Default constructor for SingleComment.
 */
SingleComment::SingleComment() {}

/**
 * @brief Destructor for SingleComment.
 */
SingleComment::~SingleComment() {}

/**
 * @brief Create a SingleCommentInfo object that holds all the information needed in the comment.
 *
 * @param singleComment Location of the JSON value containing the comment data.
 * @return The object of a single comment (SingleCommentInfo).
 */
SingleComment::SingleCommentInfo SingleComment::setSingleComment(const nlohmann::json& singleComment) {
    analysis = Analysis();
    SingleComment::SingleCommentInfo c;
    try {
        auto idCat = singleComment.find("id");
        if (idCat != singleComment.end()) { // There is an ID to the comment, meaning there is a comment
            c.id = singleComment["id"];
            c.author = singleComment["snippet"]["topLevelComment"]["snippet"]["authorDisplayName"];
            c.commenterProfileImageUrl = singleComment["snippet"]["topLevelComment"]["snippet"]["authorProfileImageUrl"];
            c.text = singleComment["snippet"]["topLevelComment"]["snippet"]["textOriginal"];
            c.likeCount = singleComment["snippet"]["topLevelComment"]["snippet"]["likeCount"];
            c.publishedAt = singleComment["snippet"]["topLevelComment"]["snippet"]["publishedAt"];
            c.updatedAt = singleComment["snippet"]["topLevelComment"]["snippet"]["updatedAt"];
            c.commentScore = analysis.getScore();
            if (c.commentScore.compound == 0) { // Potentially, compound = 0 because it's in a different language
                Translator t;
                //  Check if the comment is in a different language
                c.isEn = t.detect(c.text);
                
                if (c.isEn && t.translate(c.text)) {    // it can be translated to english from another language
                    //  Set the translation as the gingle comment's text
                    c.text = t.getTranslated();
                    analysis.setAnalysis(c.text);
                    //  Get the new score of the translated single comment
                    c.commentScore = analysis.getScore();
                }
            }
            //  Set direction and polarity
            setConclusion(c);
            return c;
        }
    } catch (std::exception e){
        std::cerr << "Could not create and push object" << std::endl;
    }
    return c;
    
}

/**
 * @brief Determine the direction and polarity of the comment.
 *
 * @param c Single comment object (SingleCommentInfo).
 * @return void, sets the conclusion direction and intensity of the single comment.
 */
void SingleComment::setConclusion(SingleComment::SingleCommentInfo c) {
    //  map of the range of a single comment
    std::map<double, std::pair<std::string, std::string>> sentimentMap = {
        {-1.00, {"negative", "very"}},
        {-0.50, {"negative", "quite"}},
        {-0.05, {"neutral", "slightly negative"}},
        {0.00, {"neutral", "very"}},
        {0.20, {"neutral", "slightly positive"}},
        {0.35, {"positive", "quite"}},
        {0.50, {"positive", "very"}}
    };
    //  Compound score dtermines direction and polarity
    double compound = c.commentScore.compound;

    //  Compare compound against the values in the map
    auto it = sentimentMap.upper_bound(compound);
    --it;
    //  Set up pair with direction and polarity
    std::pair<std::string, std::string> sentimentInfo = it->second;
    //  Set that pair as the conclusion of the object
    c.conclusion = std::make_pair(sentimentInfo.first, sentimentInfo.second);
}

