/**
 * @brief Main method for Senty program
   @authors Adrian, Emily, Karanvir, Muhammad, Salar
 */

// Desc: Main method for Senty program
// Group: 26
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

#include "video.h"

#include "request.h"

using json = nlohmann::json;

int main() {

	// Key for YouTube API access
	std::string apiKey = "AIzaSyAJfrZ_lK-cymNCsndagxrsVZJXjWqSJM8";
	std::string videoUrl;
	// Requesting user to enter url
	std::cout << "Enter YouTube Link: ";
    std::getline(std::cin, videoUrl);

	// Forming a request from the user-inputted link
	Request request(videoUrl);
	// Extracting video ID from request
	std::string videoID = request.extractVideoID();

	// Create an object of the Video class from the requested video ID
	Video video(videoID, apiKey);

	std::cout << "----------------------------------------------------" << std::endl;
	//	Outputs all comments(, if any), and the sentiment score associated with each comment(, if any)
	std::cout << "All Comments:" << std::endl;
	video.printVideoComments();

	std::cout << "----------------------------------------------------" << std::endl;
	// Showing all video info; ID, published date, title, description, tags, duration, captioning, view & like count,
    // comment count, and privacy status
	std::cout << "Video Info:" << std::endl;
	video.printVideoInfo();
	
	std::cout << "----------------------------------------------------" << std::endl;
	// Showing Sentiment analysis: number of pos, neutral, neg comments, respectively,
    // and assessing sentiment (pos/neg/neutral) and polarity: whether the comments are more agreeable or more extreme
	std::cout << "Sentiment Analysis:" << std::endl;
	Video::VideoInfo v = video.getVideoObj();
	video.printAnalysis(v);
	
	std::cout << "----------------------------------------------------" << std::endl;
    // Shows top 5 videos closely related to the video analyzed.
    // retrieves their video title, channel title, and url to each video
    video.fetchRecommended(videoID, apiKey);
    std::cout << "----------------------------------------------------" << std::endl;
    // Analyzes certain video attributes and gives suggestions to the user to improve video reception/performance.
    // if no captions -> suggests captioning, unlisted -> suggests publishing, low quality -> suggests HD uploading
    video.suggestionAnalysis(v);
    std::cout << "----------------------------------------------------" << std::endl;
    return 0;
}
