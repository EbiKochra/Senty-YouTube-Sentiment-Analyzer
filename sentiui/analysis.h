/**
 * @file analysis.h
 * @brief Header for the Analysis class; formats and outputs sentiment analysis scores
 * @author Adrian, Emily, Karanvir, Muhammad, Salar
 * @ingroup 26
 */

// Guards
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <iostream>
#include <string>
#include <vector>
#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"

/**
 * @brief Structure for video's score
 */
struct Score {
    double positive = 0.0; ///< Positive sentiment score
    double negative = 0.0; ///< Negative sentiment score
    double neutral = 0.0;  ///< Neutral sentiment score
    double compound = 0.0; ///< Compound sentiment score
    int numComments = 0;   ///< Number of comments
    std::string intensity = ""; ///< Sentiment intensity
    std::string direction = ""; ///< Sentiment direction
};

/**
 * @brief Analysis class with methods to compute sentiment of an individual comment, and to print out formatted score results
 */
class Analysis {
private:
    vader::SentimentIntensityAnalyzer vader; ///< Instance of SentimentIntensityAnalyzer class
    Score score; ///< Score object to store sentiment scores

public:
    /**
     * @brief Constructor for Analysis class
     */
    Analysis();

    /**
     * @brief Destructor for Analysis class
     */
    ~Analysis();

    /**
     * @brief Sets the value of score with VADER scores
     * @param comments Reference to the address of the comment
     * @return Void, but sets a Score structure
     */
    void setAnalysis(const std::string& comments);

    /**
     * @brief Prints out the values in the score structure
     * @param score A constant reference to the score structure associated with the comment
     * @return Void, but prints out each of the elements of the score
     */
    void printTotalAnalysis(const Score& score);

    /**
     * @brief Get the Score structure object
     * @return Score object for the associated comment
     */
    Score getScore() const;
};

#endif

