/*  analysis.h
    GROUP: group26
    DESC: ___
*/

// Guards
#ifndef ANALYSIS_H 
#define ANALYSIS_H 

#include <iostream>

#include <string>
#include <map>
#include <vector>
#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"
// #include "include/vader/vaderSentiment.hpp"
// #include "include/vader/SentiText.hpp"
// #include "include/vader/SentimentIntensityAnalyzer.hpp"

using namespace std;

class Analysis {
    private:
    
        //  Strcutre for video's score
        // struct Score {
        //     double positive = 0.0;
        //     double negative = 0.0;
        //     double neutral = 0.0;
        //     double compound = 0.0;
        //     int numComments = 0;
        //     string intensity = "";
        //     string direction = "";
        //     string Conclusion = "";
        // };
        vader::SentimentIntensityAnalyzer vader;
        // Score score;
        map<string, double> scoreNum;
        
        int numComments = 0;
        // map<string, string> scoreInfo;

        // string intensity = "";
        // string direction = "";
        string commentSummary = "";

    public:
        Analysis();
        ~Analysis();
        void setAnalysis(const string& comment);
        void setCommentSummary();
        map<string, double> getScoreNum() const;
        int getNumComments() const;
        string getCommentSummary() const;
        // Score getScore() const;

};

#endif 
