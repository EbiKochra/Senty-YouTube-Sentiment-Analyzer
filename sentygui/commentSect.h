/*  comment.h
    GROUP: group26
    DESC: ___
*/

// Guards
#ifndef COMMENTSECT_H 
#define COMMENTSECT_H 

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <map>
#include "singlecomment.h"
#include "request.h"
#include "analysis.h"
// #include "appModel.h"

using json = nlohmann::json;
using namespace std;

class AppModel;

class CommentSect {
    public:
        
        CommentSect(string videoID, AppModel *appModel);
        ~CommentSect();

        vector<string> getAllPageTokens(string videoID);
        bool fetchComments(string videoID);
        json fetchPageComments(string fullUrl);
        void importComments(json& commentJson);
        void setCommentSectScore();

        void setConclusion();
        void setCommentSectAnalysis();

        // Getters
        vector<SingleComment> getComments() const;
        string getCommentsString() const;
        map<string, double> getOverallScore() const;
        string getIntensity() const;
        string getDirection() const;
        int getNumComments() const;
        pair<string, string> getConclusion() const;
        map<string, int> getConclusionCounts() const;
        string getCommentSectAnalysis() const;

    private:
        AppModel *appModel;
        string apiKey = "AIzaSyAJfrZ_lK-cymNCsndagxrsVZJXjWqSJM8";

        // CommentSect commentSect;
        Analysis analysis;
        SingleComment comment;
        string y_url;
        string nextPageToken = "";
        bool nextPageAvailable;


        vector<SingleComment> comments;
        string commentString = "";
        map<string, double> overallScore;
        string intensity;
        string direction;
        int numComments = 0;
        pair<string, string> conclusion;
        map<string, int> conclusionCounts = {
            {"numPos", 0},
            {"numNeg", 0},
            {"numNeu", 0}
        };
        string commentsAnalysis = "";
        // vector<string> responses;

};

#endif 
