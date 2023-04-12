/**
 * @file comment.h
 * @brief Header for Comment class
 * @author Adrian, Emily, Karanvir, Muhammad, Salar
 * @ingroup 26
 */

// Guards
#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "singlecomment.h"
#include "request.h"
#include "analysis.h"

using json = nlohmann::json;

/**
 * @brief Comment class with attributes representing the comment section of a video, and methods to retrieve individual
 * comment information
 */
class Comment {
public:
    /**
     * @brief Structure for CommentInfo
     */
    struct CommentInfo {
        std::vector<SingleComment::SingleCommentInfo> comments; ///< Vector of SingleCommentInfo objects
        std::vector<std::string> commentString; ///< Vector of comment strings
        Score overallScore; ///< Overall score for the comment section
        std::string intensity; ///< Intensity of sentiment
        std::string direction; ///< Direction of sentiment
        int numComments = 0; ///< Number of comments
        std::pair<std::string, std::string> conclusion; ///< Conclusion pair (intensity, direction)
        std::map<std::string, int> conclusionCounts = { ///< Map of conclusion counts
                {"numPos", 0},
                {"numNeg", 0},
                {"numNeu", 0}
        };
    };

    /**
     * @brief Constructor for Comment class with videoID and apiKey
     * @param videoID Video ID string
     * @param apiKey API key string
     */
    Comment(std::string videoID, std::string apiKey);

    /**
     * @brief Constructor for Comment class with jsonInfo
     * @param jsonInfo Reference to a json object
     */
    Comment(json& jsonInfo);

    /**
     * @brief Destructor for Comment class
     */
    ~Comment();

    /**
     * @brief Get the CommentInfo object
     * @return CommentInfo object
     */
    Comment::CommentInfo getCommentSect();

    /**
     * @brief Fetch comments for a given video ID and API key
     * @param videoID Video ID string
     * @param apiKey API key string
     * @return Vector of comment strings
     */
    std::vector<std::string> fetchComments(std::string videoID, std::string apiKey);

    /**
     * @brief Fetch page comments for a given URL
     * @param fullUrl URL string
     * @return True if page comments fetched successfully, false otherwise
     */
    bool fetchPageComments(std::string fullUrl);

    /**
     * @brief Import comments from a json object
     * @param commentJson Reference to a json object
     */
    void importComments(json& commentJson);

    /**
     * @brief Print comments to console
     */
    void printComments() const;

    /**
     * @brief Get all comment objects
     * @return Vector of SingleCommentInfo objects
     */
    std::vector<SingleComment::SingleCommentInfo> getAllCommentsObj();

    /**
     * @brief Get comment count
     * @return Number of comments
     */
    int getCommentCount() const;

    /**
     * @brief Get comment score
     * @param commentObj Reference to a SingleCommentInfo object
     * @return Score object for the associated comment
     */
    Score getCommentScore(SingleComment::SingleCommentInfo& commentObj) const;
    /**
     * @brief Set comment section score
     */
    void setCommentSectScore();

    /**
     * @brief Set conclusion based on comment scores
     */
    void setConclusion();

    // Getter methods

    /**
     * @brief Get the number of positive comments
     * @return Number of positive comments
     */
    int getPosNum();

    /**
     * @brief Get the number of negative comments
     * @return Number of negative comments
     */
    int getNegNum();

    /**
     * @brief Get the number of neutral comments
     * @return Number of neutral comments
     */
    int getNeuNum();

private:
    Comment::CommentInfo commentSect; ///< CommentInfo object
    Analysis analysis; ///< Analysis object
    std::string y_url; ///< YouTube URL string
    std::string nextPageToken = ""; ///< Next page token string
    bool nextPageAvailable; ///< Flag indicating if the next page is available
};

#endif

