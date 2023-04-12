/**
 * @file comment.h
 * @ingroup group26
 * @brief Header for the SingleComment class.
 */

#ifndef SINGLECOMMENT_H
#define SINGLECOMMENT_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <map>
#include "request.h"
#include "analysis.h"

using json = nlohmann::json;

/**
 * @class SingleComment
 * @brief A class to manage and analyze a single comment.
 */
class SingleComment {

public:
    /**
     * @struct SingleCommentInfo
     * @brief Struct to store the information of a single comment.
     */
    struct SingleCommentInfo {
        std::string id;
        std::string author;
        std::string commenterProfileImageUrl;
        std::string text;
        int likeCount;
        std::string publishedAt;
        std::string updatedAt;
        Score commentScore;
        std::string intensity;
        std::string direction;
        bool isEn;
        std::pair<std::string, std::string> conclusion;
    };

    /**
     * @brief Default constructor for SingleComment.
     */
    SingleComment();

    /**
     * @brief Destructor for SingleComment.
     */
    ~SingleComment();

    /**
     * @brief Create a SingleCommentInfo object that holds all the information needed in the comment.
     *
     * @param singleComment Location of the JSON value containing the comment data.
     * @return The object of a single comment (SingleCommentInfo).
     */
    SingleCommentInfo setSingleComment(const nlohmann::json& singleComment);

    /**
     * @brief Determine the direction and polarity of the comment.
     *
     * @param c Single comment object (SingleCommentInfo).
     * @return void, sets the conclusion direction and intensity of the single comment.
     */
    void setConclusion(SingleCommentInfo c);

private:
    Analysis analysis;
    Score overallScore;
    std::string y_url;
};

#endif


