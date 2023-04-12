/**
 * @brief Creates Video obj for the given videoID
 * @authors: Adrian, Emily, Karanvir, Muhammad, Salar
*/
// DESC: Video class of the Senty program. fetches and builds a Video object with all required metadata by querying
// youtube api with the given video id
// Group: 26

#include <string>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <map>

#include "video.h"

using json = nlohmann::json;

/*  Constructor   */
Video::Video(std::string videoID, std::string apiKey) :  commentSection(videoID, apiKey) {
    fetchVideo(videoID, apiKey);
}

/*  Deconstructor   */
Video::~Video() {}

/**
 * @brief forms an API request given the video's id and the api key
 * @param videoID, apiKey
 * @returns returns a string containing the results of the API call
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: forms an API request given the videoID and apiKey, and uses the request class to retreive the json data
std::string Video::fetchVideo(std::string videoID, std::string apiKey) {
    std::string videoAPI = "https://www.googleapis.com/youtube/v3/videos";
    std::string videoQuery = "?id=" + videoID + "&part=snippet,contentDetails,status,statistics,topicDetails&key=" + apiKey;
    std::string urlVideo = videoAPI + videoQuery;
    std::cout << urlVideo << std::endl;
    
    std::string result = "";

    Request request(urlVideo);

    if (request.execute()) {
        result = request.result();
        json response = request.getJson();
        setVideoData(response);
    }
    return result;
}
/**
 * @brief forms an API request and fetches videos related to the video with this ID
 * @param videoID, apiKey
 * @returns returns a string containing the results of the search API call
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: forms an API request to the youtube search api given the videoID and apiKey, retrieves 5 related videos
std::string Video::fetchRecommended(std::string videoID, std::string apiKey) {
    std::string searchAPI = "https://youtube.googleapis.com/youtube/v3/search?part=snippet&relatedToVideoId=";
    std::string searchQuery = videoID + "&type=video&key=" + apiKey;
    std::string urlSearch = searchAPI + searchQuery;
    std::cout << "FETCH RECOMMENDED VIDEOS: " << urlSearch << std::endl;
    std::cout << "Here are some similar videos to the video you analyzed:" << std::endl;
    std::string recVidUrl = "https://www.youtube.com/watch?v=" + videoID;
    std::string result = "";

    Request search_request(urlSearch);
    //std::vector<std::string>recVideos;
    if (search_request.execute()) {
        result = search_request.result();
        json response = search_request.getJson();
        //setVideoData(response);
        try {
            int counter = 1;
            for (const auto& item: response["items"]) {
                if (item["snippet"].find("title") != item["snippet"].end() && !item["snippet"]["title"].empty()) {
                    std::cout << std::to_string(counter) << ": " << item["snippet"]["title"] << std::endl;
                    std::cout << "from: " << item["snippet"]["channelTitle"] << std::endl;
                    std::cout << recVidUrl << std::endl;
                    //v.tags.push_back(tag);
                }
                counter += 1;
            }
        }
        catch (std::exception e) {
            std::cerr << "Could not retrieve suggested videos" << std::endl;
        }

    }

    return result;
}

/**
 * @brief Builds the VideoInfo structure
 * @param videoJson
 * @returns none, but implicitly builds a VideoInfo structure within an object of Video class
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: takes the json output from the API request and builds the VideoInfo structure
void Video::setVideoData(const json& videoJson) {
    v = Video::VideoInfo();
    try {
        for (const auto& item : videoJson["items"]) {
            v.vidID = item["id"];
            v.publishedAt = item["snippet"]["publishedAt"];
            v.channelId = item["snippet"]["channelId"];
            v.title = item["snippet"]["title"];
            v.description = item["snippet"]["description"];
            if (item["snippet"].find("tags") != item["snippet"].end() && !item["snippet"]["tags"].empty()) {
                for (const auto& tag : item["snippet"]["tags"]) {
                    v.tags.push_back(tag);
                }
            }
            v.channelTitle = item["snippet"]["channelTitle"];
            v.duration = item["contentDetails"]["duration"];
            v.definition = item["contentDetails"]["definition"];
            v.caption = item["contentDetails"]["caption"];
            v.viewCount = std::stoi(item["statistics"]["viewCount"].get<std::string>());
            v.likeCount = std::stoi(item["statistics"]["likeCount"].get<std::string>());
            v.favoriteCount = std::stoi(item["statistics"]["favoriteCount"].get<std::string>());
            v.privacyStatus = item["status"]["privacyStatus"];
            v.commentSect = commentSection.getCommentSect();
            setRatioAnalysis(v);
        }
    } catch (std::exception e){
        std::cerr << "Could not create and push object" << std::endl;
    }
}
/**
 * @brief prints all the comments from the video's comments section
 * @param none
 * @returns none, but implicitly prints all the retrieved comments
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: prints out all the comments in the commentSect attribute (a vector of singleComments) of the Video object
void Video::printVideoComments() const {
        if (!v.commentSect.comments.empty()) {
            for (const auto& comment : v.commentSect.comments) {
                std::cout << comment.text << std::endl;
                std::cout << comment.commentScore.compound << std::endl;
            }
        } else {
            std::cout << "There are no comments." << std::endl;
        }
}

/**
 * @brief prints the retrieved metadata from the video's attributes
 * @param none
 * @returns none, but implicitly prints the public attributes of the Video object
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: prints out most of the attributes of the Video object to the console
void Video::printVideoInfo() const {
    std::cout << "Vid ID: " << v.vidID << std::endl;
    std::cout << "Published Date: " << v.publishedAt<< std::endl;
    std::cout << "channelId: " << v.channelId<< std::endl;
    std::cout << "title: " << v.title<< std::endl;
    std::cout << "description: " << v.description<< std::endl;
    std::cout << "channelTitle: " << v.channelTitle<< std::endl;
    std::cout << "Tags: " << std::endl;
    std::string tagslist = "";
    for (const auto& tag : v.tags) {
        tagslist = tagslist + tag + " ";
    }
    if (!tagslist.empty()) {
        std::cout << tagslist<< std::endl;
    }
    std::cout << "duration: " << v.duration<< std::endl;
    std::cout << "definition: " << v.definition << std::endl;
    std::cout << "caption: " << v.caption<< std::endl;
    std::cout << "viewCount: " << v.viewCount<< std::endl;
    std::cout << "likeCount: " << v.likeCount<< std::endl;
    std::cout << "favoriteCount: " << v.favoriteCount<< std::endl;
    std::cout << "commentCount: " << v.commentSect.numComments<< std::endl;
    std::cout << "privacyStatus: " << v.privacyStatus<< std::endl;
}

/**
 * @brief prints the results of the analysis of the video's comments
 * @param v
 * @returns none, but implicitly prints the results of the analysis
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: prints out the sentiment scores computed for the comments of the Video object
void Video::printAnalysis(const Video::VideoInfo v) const {
    std::cout << "There are " << v.commentSect.conclusionCounts.at("numPos") << " positive comment(s), " << v.commentSect.conclusionCounts.at("numNeu") << " neutral comment(s), and " << v.commentSect.conclusionCounts.at("numNeg") << " negative comment(s)." << std::endl;

    if (v.commentSect.numComments > 0) {
        std::cout << "Sentiment: " <<  v.commentSect.conclusion.first << " ; Polarity: " << v.commentSect.conclusion.second<< std::endl;
        std::cout << "Score breakdown: " << std::endl;
        
        std::cout << "\tOverall: "<< v.commentSect.overallScore.compound << ", " << std::endl;
        std::cout << "\tNegative: " << v.commentSect.overallScore.negative << ", " << std::endl;
        std::cout << "\tNeutral: " << v.commentSect.overallScore.neutral << ", " << std::endl;
        std::cout << "\tPositive: " << v.commentSect.overallScore.positive << std::endl;

        printRatioAnalysis(v);
    } else {
        std::cout << "There is no comment data - Cannot provide an accurate judgement on the sentiment." <<  std::endl;
    }

}

/**
 * @brief getter for the VideoInfo class
 * @param none
 * @returns an object of type VideoInfo; containing attributes pertaining to the video
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: returns the VideoInfo object
Video::VideoInfo Video::getVideoObj() {
    return v;
}

/**
 * @brief setter for the likeViewRatio and commentViewRatio attributes of the VideoInfo class
 * @param v
 * @returns none
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: sets the values for the likeViewRatio and commentViewRatio attributes of the VideoInfo class
void Video::setRatioAnalysis(Video::VideoInfo& v) {
    v.likeViewRatio = static_cast<double>(v.likeCount)/static_cast<double>(v.viewCount);
    v.commentViewRatio = static_cast<double>(v.commentSect.numComments)/static_cast<double>(v.viewCount);
}

/**
 * @brief prints the results of the ratio analysis of the video's comments
 * @param v
 * @returns none, but implicitly prints the results of the ratio analysis
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: prints out the ratio analysis results computed for the comments of the Video object
void Video::printRatioAnalysis(Video::VideoInfo v) const {
    if (v.viewCount != 0) {
        if (v.likeViewRatio >= 0.0372) {
            std::cout << "Likes:Views = good engagement = 60-90th percentile. (" << v.likeViewRatio*100 << "%)." << std::endl;
        } else if (v.likeViewRatio >= 0.0149 && v.commentViewRatio < 0.0372) {
            std::cout << "Likes:Views = average engagement = 30-60th percentile. (" << v.likeViewRatio*100 << "%)." << std::endl;
        } else {
            std::cout << "Likes:Views metric = poor engagement = 10-30th percentile. (" << v.likeViewRatio*100 << "%)." << std::endl;
        }

        if (v.commentViewRatio >= 0.0004) {
            std::cout << "Comments:Views = good engagement, 60-90th percentile. (" << v.commentViewRatio*100 << "%)." << std::endl;
        } else if (v.commentViewRatio >= 0.0001 && v.commentViewRatio < 0.0004) {
            std::cout << "Comments:Views = average engagement, 30-60th percentile. (" << v.commentViewRatio*100 << "%)." << std::endl;
        } else {
            std::cout << "Comments:Views = poor engagement, 10-30th percentile. (" << v.commentViewRatio*100 << "%)." << std::endl;
        }
    } else {
        std::cout << "There are no views on this video, so view ratio cannot be calculated." << std::endl;
    }
    
}

/**
 * @brief analyzes video's attributes and outputs suggestions to user to improve response
 * @param v
 * @returns none, but implicitly prints some assortment of suggestions for the user
 * @authors Adrian, Emily, Karanvir, Muhammad, Salar
 * */
// DESC: analyzes attributes of the VideoInfo object and offers suggestions based on value of said attributes
void Video::suggestionAnalysis(Video::VideoInfo v) const{
    std::cout << "The following are some suggestions to improve audience reach, retention, and video performance: " <<
    std::endl;

    int suggestionCounter = 0;
    if (v.caption.compare("false") == 0) {
        // no captions are provided for video; suggest user to create and upload captions
        suggestionCounter += 1;
        std::cout << "\tSuggestion #" << suggestionCounter << ": Closed Captioning:\n"
                     "The analysed video appears to have either no captions, or auto-generated captions.\n"
                     "Ensuring adequate manually transcribed captions exist for your videos can greatly \n"
                     "enable you to reach a wider audience. Using a paid transcription services such as \n"
                     "www.rev.com may be a good idea." << std::endl;
    }
    if (v.privacyStatus.compare("unlisted") == 0) {
        // if video is unlisted; suggest user to make it public for more views
        suggestionCounter += 1;
        std::cout << "\tSuggestion #" << suggestionCounter << ": Publish Video:\n"
                     "The analyzed video is set to 'unlisted'. To ensure your audience can find and watch your video\n"
                     "without having to know the url, consider making the video 'public' by clicking 'publish' on the\n"
                     "Creator Studio Dashboard." << std::endl;
    }
    if (v.definition.compare("sd") == 0) {
        // if video is in standard definition (sd), suggest user to publish to youtube in a higher resolution, or
        // get higher quality recording equipment, if necessary
        suggestionCounter += 1;
        std::cout << "\tSuggestion #" << suggestionCounter << ": Higher Video Quality:\n"
                     "The analyzed video is only in standard definition, 480p. To improve the viewing experience of\n"
                     "your audience, consider rendering and uploading to youtube in 720p, 1080p, or higher. If your\n"
                     "recording equipment is unable to record in high definition, consider purchasing gear to improve\n"
                     "the production quality of your channel." << std::endl;
    }
    if (v.tags.size() == 0) {
        // if there are no tags, suggest add tags
        suggestionCounter += 1;
        std::cout << "\tSuggestion #" << suggestionCounter << ": Keyword Optimization:\n"
                     "The analyzed video did not have any tags in its metadata. When publishing a video, the creator\n"
                     "can write several keywords, or 'tags' relating to the video. This way, youtube is able to form a\n"
                     "better understanding of how to categorize your video and how to show it to the users who watch \n"
                     "similar material. It's important to have a mix of broad and more specific keywords" << std::endl;
    }

    if (v.description.empty()) {
        // if there is no description, suggest add description
        suggestionCounter += 1;
        std::cout << "\tSuggestion #" << suggestionCounter << ": Description Optimization:\n"
                     "The analyzed video was not given a description. When publishing a video, the creator can write a\n"
                     "description, offering additional information or context to the video. This way, the audience is\n"
                     "better informed about your video and youtube is more accurately able to recommend the video to \n"
                     "viewers. It's important to have a clear, and preferably not too long description" << std::endl;
    }


}
