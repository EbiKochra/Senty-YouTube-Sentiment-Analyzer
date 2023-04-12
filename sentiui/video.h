/**
 * @brief Video class header
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
*/
// Desc: header for the Video class of the Senty program
//Group: 26


// Guards
#ifndef VIDEO_H 
#define VIDEO_H

#include <string>
#include <iostream>

#include <nlohmann/json.hpp>

#include "comment.h"
#include "request.h"

using json = nlohmann::json;

class Video {
    public:
        struct VideoInfo {
            std::string vidID;
            std::string publishedAt;
            std::string channelId;
            std::string title;
            std::string description;
            std::vector<std::string> tags;
            std::string channelTitle;
            std::string defaultAudioLanguage;
            std::string duration;
            std::string caption;
            std::string definition;
            int viewCount;
            int likeCount;
            int favoriteCount;
            std::vector<std::string> topicCategories;
            std::string privacyStatus;
            Comment::CommentInfo commentSect;
            double likeViewRatio = 0;
            double commentViewRatio = 0;
        };
        Video(std::string videoID, std::string apiKey);
        ~Video();
        std::string fetchVideo(std::string videoID, std::string apiKey);
        std::string fetchRecommended(std::string videoID, std::string apiKey);
        void setVideoData(const json& json);
        void printVideoComments() const;
        void printVideoInfo() const;
        void printAnalysis(const Video::VideoInfo v) const;
        Video::VideoInfo getVideoObj();
        void setRatioAnalysis(Video::VideoInfo& v);
        void printRatioAnalysis(Video::VideoInfo v) const;
        void suggestionAnalysis(Video::VideoInfo v) const;
    private:
        std::vector<Video::VideoInfo> videos;
        Video::VideoInfo v;
        Comment commentSection;

};

#endif 
