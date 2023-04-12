/*  comment.cpp
    GROUP:  group26
    DATE:   March 14
    DESC:   Fetches comments for a given YouTube video, getting sentiment scores of those comments, and getting total sentiment score. 
*/

#include <iostream>
#include <string>
// #include <pthread.h>
#include <mutex>
#include <vector>
#include <functional>
#include <algorithm>
#include <future>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"
#include "commentSect.h"
#include "singlecomment.h"
#include "request.h"
#include "translator.h"
#include "appModel.h"

using json = nlohmann::json;
using namespace std;

//  Constructor
CommentSect::CommentSect(string videoID, AppModel *appModel) : appModel{appModel} { 
    cout << "CommentSect() constructor #1 starts." << endl;
    commentString = fetchComments(videoID); 
}

//  Deconstructor
CommentSect::~CommentSect() {}


vector<string> CommentSect::getAllPageTokens(string videoID) {
    vector<string> pageTokens;
    string nextPageToken;
    bool morePages = true;

    while (morePages) {
        string urlCommentSects = "https://www.googleapis.com/youtube/v3/commentThreads?part=snippet&maxResults=100&videoId=" + videoID + "&key=" + apiKey + "&order=time";
        // cout << urlCommentSects << endl;
        if (!nextPageToken.empty()) {
            urlCommentSects += "&pageToken=" + nextPageToken;
        }
        Request request(urlCommentSects);
        if (request.execute() == 1) {
            json response = request.getJson();
            if (response.contains("nextPageToken")) {
                nextPageToken = response["nextPageToken"].get<string>();
                cout << nextPageToken << endl;
                pageTokens.push_back(nextPageToken);
            } else {
                morePages = false;
            }
        } else {
            morePages = false;
        }
    }
    

    return pageTokens;
}

bool CommentSect::fetchComments(string videoID) {
    
    // Collect all page tokens
    cout << "before getAllPageTokens()" << endl;
    vector<string> pageTokens = getAllPageTokens(videoID);
    // cout << "after getAllPageTokens()" << endl;
    string urlCommentSects = "https://www.googleapis.com/youtube/v3/commentThreads?part=snippet&maxResults=100&videoId=" + videoID + "&key=" + apiKey + "&order=time";
    // std::cout << "FETCH COMMENTS API: "<< urlCommentSects << std::endl;

    nextPageAvailable = true;   // Check if there are more than 100 comments (more than 1 page)

    //  There is a link
    if (!urlCommentSects.empty()) {
        //  There are more comments to check
        // thread thread_name();
        // thread threads[pageTokens.size()];

        std::cout << "before for-loop" << std::endl;
        std::cout << "urlCommentSects = " << urlCommentSects << std::endl;
        vector<json> responses;
        json response = fetchPageComments(urlCommentSects);
        responses.push_back(response);

        // while (nextPageAvailable) {
        for (const auto& token:pageTokens) {
            std::cout << "inside for-loop" << std::endl;
            //  Add the page token label to the url
            urlCommentSects += "&pageToken=" + token;
            //  Get the comments for the page
            // nextPageAvailable = fetchPageComments(urlCommentSects);
            response = fetchPageComments(urlCommentSects);
            responses.push_back(response);
        // }
        }
        std::cout << "responses.size() = " << responses.size() << std::endl;
        for (auto response : responses) {
            importComments(response);
        }
    }
    setCommentSectScore();
    setConclusion();
    setCommentSectAnalysis();
    // cout << numComments <<endl;
    // return commentString;
    return true;

}


//  DESC: Gets the comments for 1 specific page of comments (100 comments max)
//  PARAM: URL for API
//  RETURN: True (there are still more comments to get)/False (no more comments to fetch)
json CommentSect::fetchPageComments(string fullUrl) {
    
    string result = "";    // retrieved API text
    Request request(fullUrl);   // Collect information from API
    int r = request.execute();
    if (r > -1) {
        return request.getJson();
    }
    return false;
}

void CommentSect::importComments(json& commentPageJson) {
// void CommentSect::importComments(int commentPageJsonIndex) {
    cout << "importComments() starts" << endl;
    // json& commentPageJson = responses.at(commentPageJsonIndex);
    // json commentPageJson = json::parse(responses.at(commentPageJsonIndex));
    // cout << "commentPageJson = " << commentPageJson.dump() << endl;
    // cout << "setting analysis" << endl;
    analysis = Analysis();
    if (commentPageJson.contains("error") && commentPageJson["error"].contains("code") && commentPageJson["error"]["code"] == 403) {
        commentString += commentPageJson["error"]["message"];
    } else {
        int j = 0;
        for (const auto& commentJson : commentPageJson["items"]) {
            // cout << "commentJson = " << commentJson << endl;
            // commentJson.dump(2);
            comment = SingleComment();
            // cout << "step #1" << endl;
            comment.setSingleComment(commentJson);
            // cout << "step #2" << endl;
            comments.push_back(comment);
            // cout << "step #3" << endl;
            numComments++;
            cout << numComments << endl;
            map<string, double> singleCommentScore = comment.getCommentScore();
            if (singleCommentScore["compound"] >= 0.35) {
                conclusionCounts["numPos"]++;
            } else if (singleCommentScore["compound"] < -0.05) {
                conclusionCounts["numNeg"]++;
            } else {
                conclusionCounts["numNeu"]++;
            }
            cout << "ImportComments: step " << j++ << " completed." << endl;
            appModel->updateAnalyzingProgress(j);
            // appModel->updateAnalyzingProgress(numComments);

            // cout << "ImportComments: step " << j++ << " of thread #" << commentPageJsonIndex << " completed." << endl;
        }
    }
    // cout << "importComments() ends" << endl;
}

//  DESC: gets overall comment section score
//  PARAM: NAN, uses the individual comments' scores to determine overall
//  RETURN: NAN, sets conclusion on direction and polarity, and the average score for all Score object variables
void CommentSect::setCommentSectScore() {
    if (numComments > 0) {  // There are comments
        for (const auto& commentObj : comments) {   // Check every comment
            map<string, double> singleCommentScore = commentObj.getCommentScore();
            // Add to the total for all variables
            overallScore["compound"] += singleCommentScore["compound"];
            overallScore["positive"] += singleCommentScore["positive"];
            overallScore["neutral"] += singleCommentScore["neutral"];
            overallScore["negative"] += singleCommentScore["negative"];
        }
        //  Divide to find average
        overallScore["compound"] /= numComments;
        overallScore["positive"] /= numComments;
        overallScore["neutral"] /= numComments;
        overallScore["negative"] /= numComments;
        
    }
}

//  DESC: sets the conclusion for the comments section
//  PARAM: NAN, takes comments section information
//  RETURN: NAN, comment section object score is set
void CommentSect::setConclusion() {
    //  There are comments to check
    if (numComments > 0) {
        
        map<double, pair<string, string>> sentimentMap = {
            {-1.00, {"negative", "extreme"}},
            {-0.50, {"negative", "moderate"}},
            {-0.05, {"neutral", "slightly negative"}},
            {0.00, {"neutral", "extreme"}},
            {0.20, {"neutral", "slightly positive"}},
            {0.35, {"positive", "moderate"}},
            {0.50, {"positive", "extreme"}}
        };
        // Compound score will determine the direction and polarity
        double compound = overallScore["compound"];

        //  Establish the boundaries for map to check against
        auto it = sentimentMap.upper_bound(compound);
        //  Decrement pointer to align with map
        --it;
        //  Create pair for the direction and polarity
        pair<string, string> sentimentInfo = it->second;

        //  Make the conclusion object the values
        conclusion = make_pair(sentimentInfo.first, sentimentInfo.second);

    } else {    // THere aren't enough comments to establish an accurate score
        conclusion = make_pair("There is not enough comment data to generate a score breakdown.", "");
    }

    
}

void CommentSect::setCommentSectAnalysis() {
    commentsAnalysis += "There are " + to_string(conclusionCounts["numPos"])+ " positive comment(s), " + to_string(conclusionCounts["numNeu"]) + " neutral comment(s), and " + to_string(conclusionCounts["numNeg"]) + " negative comment(s).\n";

    if (numComments > 0) {
        commentsAnalysis += "Sentiment: " + conclusion.first + " ; Polarity: " + conclusion.second + "\n";
        commentsAnalysis += "Score breakdown: \n";
        
        commentsAnalysis += "\tOverall: " + to_string(overallScore["compound"]) + ", \n";
        commentsAnalysis += "\tNegative: " + to_string(overallScore["negative"]) + ", \n";
        commentsAnalysis += "\tNeutral: " + to_string(overallScore["neutral"]) + ", \n";
        commentsAnalysis += "\tPositive: " + to_string(overallScore["positive"]) + "\n";

        // commentsAnalysis += getRatioAnalysis(v);
    } else {
        commentsAnalysis += "There is no comment data - Cannot provide an accurate judgement on the sentiment.";
    }
    // return commentsAnalysis;

}

vector<SingleComment> CommentSect::getComments() const {
    return comments;
}

string CommentSect::getCommentsString() const {
    return commentString;
}

map<string, double>  CommentSect::getOverallScore() const {
    return overallScore;
}

string CommentSect::getIntensity() const {
    return conclusion.second;
}

string CommentSect::getDirection() const {
    return conclusion.first;
}

int CommentSect::getNumComments() const {
    return numComments;
}

pair<string, string> CommentSect::getConclusion() const {
    return conclusion;
}

map<string, int> CommentSect::getConclusionCounts() const {
    return conclusionCounts;
}

string CommentSect::getCommentSectAnalysis() const {
    return commentsAnalysis;
}