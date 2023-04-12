/**

@file comment.cpp
@brief Comment class source file; fetches comments for a given YouTube video, getting sentiment scores of those comments, and getting total sentiment score.
@authors Adrian, Emily, Karanvir, Muhammad, Salar
@group 26
*/
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"
#include <nlohmann/json.hpp>

#include "comment.h"
#include "singlecomment.h"
#include "request.h"
#include "translator.h"

using json = nlohmann::json;

// Constructor for Comment class
Comment::Comment(std::string videoID, std::string apiKey) {
/**
* @brief Given a video ID extracted by Request class and an apiKey, retrieves all comments from a video.
*
* @param videoID A string representing the video ID of the YouTube video to fetch comments for.
* @param apiKey A string representing the API key to authenticate with the YouTube API.
*/
    commentSect.commentString = fetchComments(videoID, apiKey);
}

// Constructor for Comment class
Comment::Comment(json& jsonInfo) {
/**
* @brief Constructor for Comment class, given JSON info.
*
* @param jsonInfo A JSON object containing information related to a comment.
*/
    importComments(jsonInfo);
}

// Destructor for Comment class
Comment::~Comment() {}

// Getter for all comments
Comment::CommentInfo Comment::getCommentSect() {
/**
* @brief Getter for all comments in video from CommentInfo structure.
*
* @return Returns a CommentInfo object representing all comments for a given video.
*/
    return commentSect;
}

// Gets all the video's comments from API, for all pages (max 100 comments/page)
std::vector<std::string> Comment::fetchComments(std::string videoID, std::string apiKey) {
/**
* @brief Given a video ID extracted by Request class and an apiKey, retrieves all comments from a video.
*
* @param videoID A string representing the video ID of the YouTube video to fetch comments for.
* @param apiKey A string representing the API key to authenticate with the YouTube API.
*
* @return Returns a vector of all comments associated with the given video ID.
*/
    std::string commentsAPI = "https://www.googleapis.com/youtube/v3/commentThreads";
    std::string commentsQuery = "?part=snippet&maxResults=100&videoId=" + videoID + "&key=" + apiKey + "&order=time";
    std::string urlComments = commentsAPI + commentsQuery;
    nextPageAvailable = true; // Check if there are more than 100 comments (more than 1 page)

    // There is a link
    if (!urlComments.empty()) {
        // There are more comments to check
        while (nextPageAvailable) {
            // Add the page token label to the url
            urlComments += "&pageToken=" + nextPageToken;
            // Get the comments for the page
            nextPageAvailable = fetchPageComments(urlComments);
        }
    }
    return commentSect.commentString;
}

/**

@brief Gets the comments for one specific page of comments (100 comments max)

@param fullUrl The URL for the API call

@return True if there are still more comments to get, False if there are no more comments to fetch
*/
bool Comment::fetchPageComments(std::string fullUrl) {
    std::string result = ""; // retrieved API text
    Request request(fullUrl); // Collect information from API

    if (request.execute()) { // Request succeeded
// Add the retrieved text to variable
        result += request.result();
        // Convert to JSON to parse
        json response = request.getJson();

        if (response["pageInfo"]["totalResults"] != 0) {    // ie, if the video has comments
            importComments(response);
            //  If there is another page of comments:
            if (response.contains("nextPageToken")) {
                nextPageToken = response["nextPageToken"].get<std::string>();
                // Indicate that next page should be checked
                nextPageAvailable = true;
                return true;
            }
        } else {
            // There isn't another page of comments
            nextPageAvailable = false;
        }
    } else {
// Request did not execute
        nextPageAvailable = false;
        commentSect.commentString.push_back("");
    }
// Do not check next page
    return false;
}

/**

@brief Adds comments to the comments section from the JSON text

@param commentJson JSON text of comment thread

@return void, but sets Comments section scores and assigns values to single comments
*/
void Comment::importComments(json& commentJson) {
    analysis = Analysis();
// Create a single comment for all comments in thread
    for (const auto& singleComment : commentJson["items"]) {
        try {
            SingleComment comment = SingleComment();
            SingleComment::SingleCommentInfo c = comment.setSingleComment(singleComment);
// Push single comment into comments section
            commentSect.comments.push_back(c);
            commentSect.numComments ++;
            //  Manually add to count of positive/negative/neutral
            if (c.commentScore.compound >= 0.35) {
                commentSect.conclusionCounts["numPos"]++;
            } else if (c.commentScore.compound < -0.05) {
                commentSect.conclusionCounts["numNeg"]++;
            } else {
                commentSect.conclusionCounts["numNeu"]++;
            }
        } catch (std::exception e){
            std::cerr << "Could not create and push object" << std::endl;
        }
    }
    setCommentSectScore();
}

/**

@brief Gets all comment objects ('SingleComment's)
@return returns all retrieved comments as a vector of SingleComment objects
*/
std::vector<SingleComment::SingleCommentInfo> Comment::getAllCommentsObj() {
    return commentSect.comments;
}
/**

@brief Prints all comments from vector of SingleComments
@return void, however comments are outputted to the console
*/
void Comment::printComments() const {
    for (const auto& comment : commentSect.comments) {
        std::cout << comment.text << std::endl;
    }
}
/**

@brief Getter method for number of comments in the comments section of a video
@return integer representing the number of comments in a comment section
*/
int Comment::getCommentCount() const {
    return commentSect.numComments;
}
/**

@brief Getter for comment score
@param commentObj reference to a SingleCommentInfo object, accesses and retrieves its score
@return the Score object representing the overall score of a comment
*/
Score Comment::getCommentScore(SingleComment::SingleCommentInfo& commentObj) const {
    return commentObj.commentScore;
}

/**

@brief Gets the overall score of the comment section by using the individual comments' scores

@param none

@return void; sets the comment section scores and assigns values to single comments
*/
void Comment::setCommentSectScore() {
    if (commentSect.numComments > 0) { // There are comments
        for (const auto& commentObj : commentSect.comments) { // Check every comment
// Add to the total for all variables
            commentSect.overallScore.compound += commentObj.commentScore.compound;
            commentSect.overallScore.positive += commentObj.commentScore.positive;
            commentSect.overallScore.neutral += commentObj.commentScore.neutral;
            commentSect.overallScore.negative += commentObj.commentScore.negative;
        }
// Divide to find average
        commentSect.overallScore.compound /= commentSect.numComments;
        commentSect.overallScore.positive /= commentSect.numComments;
        commentSect.overallScore.neutral /= commentSect.numComments;
        commentSect.overallScore.negative /= commentSect.numComments;
        setConclusion();
    }
}

/**

@brief Sets the conclusion for the comments section

@param none; takes comments section information

@return void; comment section object score is set
*/
void Comment::setConclusion() {
// There are comments to check
    if (commentSect.numComments > 0) {std::map<double, std::pair<std::string, std::string>> sentimentMap = {
                {-1.00, {"negative", "extreme"}},
                {-0.50, {"negative", "moderate"}},
                {-0.05, {"neutral", "slightly negative"}},
                {0.00, {"neutral", "extreme"}},
                {0.20, {"neutral", "slightly positive"}},
                {0.35, {"positive", "moderate"}},
                {0.50, {"positive", "extreme"}}
        };
        // Compound score will determine the direction and polarity
        double compound = commentSect.overallScore.compound;

        // Establish the boundaries for map to check against
        auto it = sentimentMap.upper_bound(compound);
        // Decrement pointer to align with map
        --it;
        // Create pair for the direction and polarity
        std::pair<std::string, std::string> sentimentInfo = it->second;

        // Make the conclusion object the values
        commentSect.conclusion = std::make_pair(sentimentInfo.first, sentimentInfo.second);
    } else { // There aren't enough comments to establish an accurate score
        commentSect.conclusion = std::make_pair("There is not enough comment data to generate a score breakdown.", "");
    }

}

/**

@brief Gets the number of positive comments attribute of the comment section object
@param none; accesses and returns an attribute of the object
@return an integer representing the number of 'positive comments' (comments w/ a positive conclusion)
*/
int Comment::getPosNum() {
    return commentSect.conclusionCounts["numPos"];
}
/**

@brief Gets the number of negative comments attribute of the comment section object
@param none; accesses and returns an attribute of the object
@return an integer representing the number of 'negative comments' (comments w/ a negative conclusion)
*/
int Comment::getNegNum() {
    return commentSect.conclusionCounts["numNeg"];
}

/**

@brief Getter method for the number of neutral comments attribute of the comment section object
@param None
@return An integer representing the number of 'neutral comments' (comments w/ a neutral conclusion)
*/
int Comment::getNeuNum() {
    return commentSect.conclusionCounts["numNeu"];
}

