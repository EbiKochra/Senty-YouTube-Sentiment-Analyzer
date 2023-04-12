
/*  video.h
    GROUP: group26
    DESC: ___
*/

// Guards
#ifndef VIDEO_H 
#define VIDEO_H

#include <string>
#include <iostream>

#include <nlohmann/json.hpp>

#include "commentSect.h"
#include "request.h"

using json = nlohmann::json;
using namespace std;

class AppModel;

class VideoException {
        string message;
    public:
        VideoException(string message);
        string what() const;
};


class Video {
    public:
        Video(string videoID, AppModel *appModel);
        ~Video();
        string fetchVideo(string videoID);
        void setRecommended();
        const string getRecommended() const;
        void setVideoData(const json& json);
        void setRatioAnalysis();
        string getRatioAnalysis() const;
        double getLikeViewRatio() const;
        double getCommentViewRatio() const;

        const string& getVidID() const;
        const string& getPublishedAt() const;
        const string& getChannelId() const;
        const string& getTitle() const;
        const string& getDescription() const;
        const string& getTags() const;
        const string& getChannelTitle() const;
        const string& getDefaultAudioLanguage() const;
        const string& getDuration() const;
        const string& getCaption() const;
        int getViewCount() const;
        int getLikeCount() const;
        int getFavoriteCount() const;
        const vector<string>& getTopicCategories() const;
        const string& getPrivacyStatus() const;
        const CommentSect& getCommentSect() const;
        void suggestionAnalysis();
        const string& getSuggestionAnalysis() const;
        
    private:
        string vId;
        string apiKey = "AIzaSyAJfrZ_lK-cymNCsndagxrsVZJXjWqSJM8";
        CommentSect commentSect;
        string vidID;
        string publishedAt;
        string channelId;
        string title;
        string description;
        string tags = "";
        string definition;
        string channelTitle;
        string defaultAudioLanguage;
        string duration;
        string caption;
        int viewCount;
        int likeCount;
        int favoriteCount;
        vector<string> topicCategories;
        string privacyStatus;
        string recommended;
        
        string suggestions;
        // string commentAnalysis;
        double likeViewRatio = 0;
        double commentViewRatio = 0;
        string ratioAnalysis;
        

};

#endif 

