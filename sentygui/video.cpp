
/*  video.cpp
    GROUP: group26
    DESC: ___
*/

#include <string>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <map>

#include "video.h"
#include "appModel.h"

using json = nlohmann::json;
using namespace std;

// -------------- VideoException -----------------
VideoException::VideoException(string message) : message{message} { }

std::string VideoException::what() const {
    return message;
}


/*  Constructor   */
Video::Video(string videoID, AppModel *appModel) :  commentSect(videoID, appModel) {
    cout << "Video() construcgtor starts." << endl;
    fetchVideo(videoID);
}

/*  Deconstructor   */
Video::~Video() {}

/*  Fetch Video   */
string Video::fetchVideo(string videoID) {
    vId = videoID;
    string videoAPI = "https://www.googleapis.com/youtube/v3/videos";
    string videoQuery = "?id=" + videoID + "&part=snippet,contentDetails,status,statistics,topicDetails&key=" + apiKey;
    string urlVideo = videoAPI + videoQuery;
    cout << urlVideo << endl;
    
    string result = "";

    Request request(urlVideo);

    if (request.execute()) {
        result = request.result();
        json response = request.getJson();
        setVideoData(response);
    }
    return result;
}

void Video::setRecommended() {
    std::string searchAPI = "https://youtube.googleapis.com/youtube/v3/search?part=snippet&relatedToVideoId=";
    std::string searchQuery = vId + "&type=video&key=" + apiKey;
    std::string urlSearch = searchAPI + searchQuery;
    recommended += "\nFETCH RECOMMENDED VIDEOS: " + urlSearch + "\n\n";
    recommended += "Here are some similar videos to the video you analyzed:\n\n";
    std::string recVidUrl = "https://www.youtube.com/watch?v=" + vId;
    std::string result = "";

    Request search_request(urlSearch);
    if (search_request.execute()) {
        result = search_request.result();
        json response = search_request.getJson();
        try {
            int counter = 1;
            for (const auto& item: response["items"]) {
                if (item["snippet"].find("title") != item["snippet"].end() && !item["snippet"]["title"].empty()) {
                    recommended += std::to_string(counter) + ": " + item["snippet"]["title"].get<std::string>() + "\n\ns";
                    recommended += "from: " + item["snippet"]["channelTitle"].get<std::string>() + "\n\n";
                    recVidUrl = "https://www.youtube.com/watch?v=" + item["id"]["videoId"].get<std::string>();
                    recommended += recVidUrl + "\n\n";
                }
                counter += 1;
            }
        }
        catch (std::exception e) {
            throw VideoException(e.what());
        }

    }

}


const string Video::getRecommended() const {
    return recommended;
}

/*  Set Video Data   */
void Video::setVideoData(const json& videoJson) {
    // v = Video::VideoInfo();
    try {
        for (const auto& item : videoJson["items"]) {
            vidID = item["id"];
            publishedAt = item["snippet"]["publishedAt"];
            channelId = item["snippet"]["channelId"];
            title = item["snippet"]["title"];
            description = item["snippet"]["description"];
            if (item["snippet"].find("tags") != item["snippet"].end() && !item["snippet"]["tags"].empty()) {
                for (const auto& tag : item["snippet"]["tags"]) {
                    tags += tag.get<string>() + "  ";
                }
            }
            definition = item["contentDetails"]["definition"];
            channelTitle = item["snippet"]["channelTitle"];
            duration = item["contentDetails"]["duration"];
            caption = item["contentDetails"]["caption"];
            viewCount = stoi(item["statistics"]["viewCount"].get<string>());
            likeCount = stoi(item["statistics"]["likeCount"].get<string>());
            favoriteCount = stoi(item["statistics"]["favoriteCount"].get<string>());
            privacyStatus = item["status"]["privacyStatus"];

        }
        suggestionAnalysis();
        setRecommended();
        setRatioAnalysis();
    } catch (exception &e){
        throw VideoException(e.what());
    }

}

/*  Set Ratio Analysis   */
void Video::setRatioAnalysis() {
    likeViewRatio = static_cast<double>(likeCount)/static_cast<double>(viewCount);
    commentViewRatio = static_cast<double>(commentSect.getNumComments())/static_cast<double>(viewCount);
    if (viewCount != 0) {
        ratioAnalysis += "Likes:Views\n";
        if (likeViewRatio >= 0.0372) {
            ratioAnalysis += "You have good engagement, 60-90th percentile ";
        } else if (likeViewRatio >= 0.0149 && commentViewRatio < 0.0372) {
            ratioAnalysis +=  "You have average engagement, 30-60th percentile ";
        } else {
            ratioAnalysis += "You have poor engagement, 10-30th percentile ";
        }
        ratioAnalysis += "(" + to_string(likeViewRatio*100) + "%).\n\n";

        ratioAnalysis += "Comments:Views\n";
        if (commentViewRatio >= 0.0004) {
            ratioAnalysis += "You have good engagement, 60-90th percentile ";
        } else if (commentViewRatio >= 0.0001 && commentViewRatio < 0.0004) {
            ratioAnalysis += "You have average engagement, 30-60th percentile. ";
        } else {
            ratioAnalysis += "You have poor engagement, 10-30th percentile. ";
        }
        ratioAnalysis += "(" + to_string(commentViewRatio*100) + "%).";
    } else {
        ratioAnalysis += "There are no views on this video, so view ratio cannot be calculated.\n";
    }

    cout << ratioAnalysis << endl;
}

/*  Print Ratio Analysis   */
string Video::getRatioAnalysis() const {
    return ratioAnalysis;
}

double Video::getLikeViewRatio() const {
    return likeViewRatio;
}
double Video::getCommentViewRatio() const {
    return commentViewRatio;
}


const string& Video::getVidID() const { return vidID; }
const string& Video::getPublishedAt() const { return publishedAt; }
const string& Video::getChannelId() const { return channelId; }
const string& Video::getTitle() const { return title; }
const string& Video::getDescription() const { return description; }
const string& Video::getTags() const { 
    return tags;
}
const string& Video::getChannelTitle() const { return channelTitle; }
const string& Video::getDefaultAudioLanguage() const { return defaultAudioLanguage; }
const string& Video::getDuration() const { return duration; }
const string& Video::getCaption() const { return caption; }
int Video::getViewCount() const { return viewCount; }
int Video::getLikeCount() const { return likeCount; }
int Video::getFavoriteCount() const { return favoriteCount; }
const vector<string>& Video::getTopicCategories() const { return topicCategories; }
const string& Video::getPrivacyStatus() const { return privacyStatus; }
const CommentSect& Video::getCommentSect() const { return commentSect; }

void Video::suggestionAnalysis() {
    suggestions +=  "The following are some suggestions to improve audience reach, retention, and video performance: \n";

    int suggestionCounter = 0;
    if (caption.compare("false") == 0) {
        // no captions are provided for video; suggest user to create and upload captions
        suggestionCounter += 1;
        suggestions +=  "\tSuggestion #" + to_string(suggestionCounter) + ": Closed Captioning:\n"
                     "The analysed video appears to have either no captions, or auto-generated captions.\n"
                     "Ensuring adequate manually transcribed captions exist for your videos can greatly \n"
                     "enable you to reach a wider audience. Using a paid transcription services such as \n"
                     "www.recom may be a good idea.";
    }
    if (privacyStatus.compare("unlisted") == 0) {
        // if video is unlisted; suggest user to make it public for more views
        suggestionCounter += 1;
        suggestions +=  "\tSuggestion #"+ to_string(suggestionCounter) +": Publish Video:\n"
                     "The analyzed video is set to 'unlisted'. To ensure your audience can find and watch your video\n"
                     "without having to know the url, consider making the video 'public' by clicking 'publish' on the\n"
                     "Creator Studio Dashboard.";
    }
    if (definition.compare("sd") == 0) {
        // if video is in standard definition (sd), suggest user to publish to youtube in a higher resolution, or
        // get higher quality recording equipment, if necessary
        suggestionCounter += 1;
        suggestions +=  "\tSuggestion #" +to_string(suggestionCounter) +": Higher Video Quality:\n"
                     "The analyzed video is only in standard definition, 480p. To improve the viewing experience of\n"
                     "your audience, consider rendering and uploading to youtube in 720p, 1080p, or higher. If your\n"
                     "recording equipment is unable to record in high definition, consider purchasing gear to improve\n"
                     "the production quality of your channel.";
    }
    if (tags.empty()) {
        // if there are no tags, suggest add tags
        suggestionCounter += 1;
        suggestions +=  "\tSuggestion #" +to_string(suggestionCounter)+ ": Keyword Optimization:\n"
                     "The analyzed video did not have any tags in its metadata. When publishing a video, the creator\n"
                     "can write several keywords, or 'tags' relating to the video. This way, youtube is able to form a\n"
                     "better understanding of how to categorize your video and how to show it to the users who watch \n"
                     "similar material. It's important to have a mix of broad and more specific keywords";
    }

    if (description.empty()) {
        // if there is no description, suggest add description
        suggestionCounter += 1;
        suggestions +=  "\tSuggestion #" + to_string(suggestionCounter) + ": Description Optimization:\n"
                     "The analyzed video was not given a description. When publishing a video, the creator can write a\n"
                     "description, offering additional information or context to the video. This way, the audience is\n"
                     "better informed about your video and youtube is more accurately able to recommend the video to \n"
                     "viewers. It's important to have a clear, and preferably not too long description";
    }

    if (suggestionCounter == 0) {
        suggestions += "\nWe have no suggestions for you!\n";
    }

}

const string& Video::getSuggestionAnalysis() const {
    return suggestions;
}