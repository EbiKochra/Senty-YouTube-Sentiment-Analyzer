/*  analysis.cpp
    GROUP: group26
    DESC: ___
*/

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"
// #include "include/vader/vaderSentiment.hpp"
// #include "include/vader/SentiText.hpp"
// #include "include/vader/SentimentIntensityAnalyzer.hpp"

#include "analysis.h"

using namespace std;

//  Constructor
Analysis::Analysis() {
    scoreNum = {
        {"positive", 0.0},
        {"negative", 0.0},
        {"neutral", 0.0},
        {"compound", 0.0}
    };
    // scoreInfo = {
    //     // {"intensity", ""},
    //     // {"direction", ""},
    //     {"commentSummary", ""}
    // };
}

//  Deconstructor
Analysis::~Analysis() {}

//  DESC: Sets the value of score with VADER scores
//  PARAM: location of comment
//  RETURN: void, but has a new set score object
void Analysis::setAnalysis(const string& comment) {
    // cout << comment << endl;
    // The comment has words that can be checked
    if (!comment.empty()) {
        // Polarity
        vader::Sentiment vs = vader.polarity_scores(comment);
        // cout <<"not empty" << endl;
        // cout << vs.pos <<endl;
        //  Add score of comment to total for video score
        scoreNum["positive"] += vs.pos;
        scoreNum["negative"] += vs.neg;
        scoreNum["neutral"] += vs.neu;
        scoreNum["compound"] += vs.compound;
        numComments += 1;
        // cout << vs.pos <<endl;
        // https://www.youtube.com/watch?v=PkPfdR80u_8&ab_channel=VickyZhao%5BBEEAMP%5D
        setCommentSummary();
    }
    // cout << "finally ehre" << endl;
}

//  DESC: Printing out values in structure
//  PARAM: the full score obj
//  RETURN: void, but prints out each of the elements of score
// void Analysis::setConclusion() {
//     commentSummary += "# COMMENTS = " + to_string(numComments) + "\n";
//     // cout << "# COMMENTS = " << numComments << endl;
//     commentSummary += "AGGREGATE SUM: Compound = " + to_string(scoreNum["compound"]) + ", Negative = " + to_string(scoreNum["negative"]) + ", Neutral = " + to_string(scoreNum["neutral"]) + ", Positive = " + to_string(scoreNum["positive"]) + "\n";
//     commentSummary += "AVERAGE SENTIMENT: Compound = " + to_string(scoreNum["compound"]/numComments) + ", Negative = " + to_string(scoreNum["negative"]/numComments) + ", Neutral = " + to_string(scoreNum["neutral"]/numComments) + ", Positive = " + to_string(scoreNum["positive"]/numComments);
// }
void Analysis::setCommentSummary() {
    // cout << "setting comment summary..." << endl;
    commentSummary += "# COMMENTS = " + to_string(numComments) + "\n";
    // cout << "# COMMENTS = " << numComments << endl;
    commentSummary += "AGGREGATE SUM: Compound = " + to_string(scoreNum["compound"]) + ", Negative = " + to_string(scoreNum["negative"]) + ", Neutral = " + to_string(scoreNum["neutral"]) + ", Positive = " + to_string(scoreNum["positive"]) + "\n";
    commentSummary += "AVERAGE SENTIMENT: Compound = " + to_string(scoreNum["compound"]/numComments) + ", Negative = " + to_string(scoreNum["negative"]/numComments) + ", Neutral = " + to_string(scoreNum["neutral"]/numComments) + ", Positive = " + to_string(scoreNum["positive"]/numComments);
}

map<string, double> Analysis::getScoreNum() const {
    // cout << "in analysis" << endl;
    return scoreNum;
}
int Analysis::getNumComments() const {
    return numComments;
}
string Analysis::getCommentSummary() const {
    return commentSummary;
}

//  DESC: Get the score structure object
//  PARAM: NAN
//  RETURN: score object for associated comment
// Score Analysis::getScore() const {
//     return score;
// }

