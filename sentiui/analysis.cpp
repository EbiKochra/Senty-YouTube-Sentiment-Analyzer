/**
 * @file analysis.cpp
 * @brief Analysis class; formats and outputs sentiment analysis scores
 * @author Adrian, Emily, Karanvir, Muhammad, Salar
 * @ingroup 26
 */

#include <vector>
#include <iostream>
#include <sstream>

#include "vaderSentiment.hpp"
#include "analysis.h"

/**
 * @brief Constructor for Analysis class
 */
Analysis::Analysis() {}

/**
 * @brief Destructor for Analysis class
 */
Analysis::~Analysis() {}

/**
 * @brief Sets the value of score with VADER scores
 * @param comment Reference to the address of the comment
 * @return Void, but sets a Score structure
 */
void Analysis::setAnalysis(const std::string& comment) {
    // creating a new score object for the new comment
    score = Score();

    // The comment has words/emojis that can be checked (ie, non-empty)
    if (!comment.empty()) {
        // computes polarity score, assigns to Sentiment object 'vs'
        vader::Sentiment vs = vader.polarity_scores(comment);
        //  Add value of analysed comment's sentiment and polarity to total aggregate score variables
        score.positive += vs.pos;
        score.negative += vs.neg;
        score.neutral += vs.neu;
        score.compound += vs.compound;
        score.numComments += 1;
    }
}

/**
 * @brief Prints out the values in the score structure
 * @param score A constant reference to the score structure associated with the comment
 * @return Void, but prints out each of the elements of the score
 */
void Analysis::printTotalAnalysis(const Score& score) {
    std::cout << "# COMMENTS = " << score.numComments << std::endl;
    std::cout << "AGGREGATE SUM: Compound = "<< score.compound << ", Negative = " << score.negative << ", Neutral = " << score.neutral << ", Positive = " << score.positive << std::endl;
    std::cout << "AVERAGE SENTIMENT: Compound = "<< score.compound/score.numComments << ", Negative = " << score.negative/score.numComments << ", Neutral = " << score.neutral/score.numComments << ", Positive = " << score.positive/score.numComments << std::endl;
}

/**
 * @brief Get the Score structure object
 * @return Score object for the associated comment
 */
Score Analysis::getScore() const {
    return score;
}

