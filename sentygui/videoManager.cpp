/*  videoManager.cpp
    GROUP: group26
    DESC: ___
*/
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include "videoManager.h"
#include "video.h"

using namespace std;

/*  Constructor   */
VideoManager::VideoManager(AppModel *appModel) : appModel{appModel} {
    // this->apiKey = apiKey;
}

/*  Deconstructor   */
VideoManager::~VideoManager() {}

/*  Add Video   */
void VideoManager::addVideo(const string &url) {
    Request request(url);
    string videoID = request.extractVideoID();
    // cout << videoID <<endl;
    numVideos++;
    Video video(videoID, appModel);
    videos.push_back(video);
}
/*  Fetch Videos   */
void VideoManager::fetchVideos() {
    for (auto& video : videos) {
        video.fetchVideo(video.getVidID());
    }
}


void VideoManager::setLikeViewCompare(const int v1, const int v2) {
    CommentSect v1Comments = videos[v1].getCommentSect();
    CommentSect v2Comments = videos[v2].getCommentSect();
    // double v1LikeViewRatio = videos[v1].getLikeViewRatio();
    // double v2LikeViewRatio = videos[v2].getLikeViewRatio();
    
    if (v1Comments.getNumComments() > 0 && v2Comments.getNumComments() > 0) {
        if (videos[v1].getLikeViewRatio() == videos[v2].getLikeViewRatio()) {
            likeViewCompare += "The likes:view ratio on all videos is the same.\n";
        } else if (videos[v1].getLikeViewRatio() < videos[v2].getLikeViewRatio()) {
            likeViewCompare += "The likes:view ratio on \"" + videos[v2].getTitle() + "\" is highest.\n";
        } else {
            likeViewCompare += "The likes:view ratio on \""+ videos[v1].getTitle() + "\" is highest.\n";
        }
    } else {
        likeViewCompare += "There is not enough data on at least one of the videos to make this analysis.\n";
    }
    cout << "Like View Compare: " << likeViewCompare << endl;

}

void VideoManager::setCommentViewCompare(const int v1, const int v2) {
    CommentSect v1Comments = videos[v1].getCommentSect();
    CommentSect v2Comments = videos[v2].getCommentSect();
    // double v1CommentViewRatio = videos[v1].getCommentViewRatio();
    // double v2CommentViewRatio = videos[v2].getCommentViewRatio();

    if (v1Comments.getNumComments() > 0 && v2Comments.getNumComments() > 0) {
        if (v1Comments.getNumComments() == videos[v2].getCommentViewRatio()) {
        // if (v1CommentInfo. == videos[v2].likeViewRatio) {
            commentViewCompare += "The comments:view ratio on all videos is the same.\n";
        } else if (videos[v1].getCommentViewRatio() < videos[v2].getCommentViewRatio()) {
            commentViewCompare += "The comments:view ratio on \""+ videos[v2].getTitle() + "\" is highest.\n";
        } else {
            commentViewCompare += "The comments:view ratio on \""+ videos[v1].getTitle() + "\" is highest.\n";
        }

    } else {
        commentViewCompare += "There is not enough data on at least one of the videos to make this analysis.\n";
    }
}

void VideoManager::setSentimentCompare(const int v1, const int v2) {
    CommentSect v1Comments = videos[v1].getCommentSect();
    CommentSect v2Comments = videos[v2].getCommentSect();

    if (v1Comments.getNumComments() > 0 && v2Comments.getNumComments() > 0) {
        if (v1Comments.getOverallScore()["compound"] == v1Comments.getOverallScore()["compound"]) {
            sentimentCompare += "All videos are equally as positive.\n";
            
        } else if (v1Comments.getOverallScore()["compound"] > v1Comments.getOverallScore()["compound"]) {
            sentimentCompare += "\"" + videos[v1].getTitle() + "\" is most positive.\n";
        } else {
            sentimentCompare += "\"" + videos[v2].getTitle() + "\" is most positive.\n";
        }
    } else {
        sentimentCompare += "There is not enough data on at least one of the videos to make this analysis.\n";
    }
}

bool VideoManager::compareVideosByCompoundScore(const Video& v1, const Video& v2) {
    CommentSect v1Comments = v1.getCommentSect();
    CommentSect v2Comments = v2.getCommentSect();

    double v1CompoundScore = v1Comments.getOverallScore()["compound"];
    double v2CompoundScore = v2Comments.getOverallScore()["compound"];

    return v1CompoundScore > v2CompoundScore;
}

void VideoManager::sortVideosBySentiment() {
    std::sort(videos.begin(), videos.end(), [this](const Video& v1, const Video& v2) {
        return compareVideosByCompoundScore(v1, v2);
    });
}


// highest to lowest positivity
vector<Video> VideoManager::getVideosSortedBySentiment() {
    sortVideosBySentiment();
    return videos;
}


string VideoManager::getLikeViewCompare() const {
    return likeViewCompare;
}
string VideoManager::getCommentViewCompare() const {
    return commentViewCompare;
}
string VideoManager::getSentimentCompare() const {
    return sentimentCompare;
}
vector<Video> VideoManager::getVideos() const {
    return videos;
}
int VideoManager::getNumVideos() const {
    return numVideos;
}
