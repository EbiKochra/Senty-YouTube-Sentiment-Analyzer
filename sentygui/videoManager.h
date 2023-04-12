/*  videoManager.h
    GROUP: group26
    DESC: ___
*/

// Guards
#ifndef VIDEOMANAGER_H 
#define VIDEOMANAGER_H

#include <string>
#include <vector>
#include <map>

#include "video.h"
// #include "appModel.h"

using namespace std;

class AppModel;

class VideoManager {
public:
    VideoManager(AppModel *appModel);
    ~VideoManager();
    void addVideo(const string &url);
    void fetchVideos();
    void setLikeViewCompare(const int v1, const int v2);
    void setCommentViewCompare(const int v1, const int v2);
    void setSentimentCompare(const int v1, const int v2);
    bool compareVideosByCompoundScore(const Video& v1, const Video& v2);
    void sortVideosBySentiment();
    vector<Video> getVideosSortedBySentiment();
    string getLikeViewCompare() const;
    string getCommentViewCompare() const;
    string getSentimentCompare() const;
    vector<Video> getVideos() const;
    int getNumVideos() const;

private:
    AppModel *appModel;
    vector<Video> videos;
    string likeViewCompare = "";
    string commentViewCompare = "";
    string sentimentCompare = "";
    int numVideos = 0;
    
};
#endif

