/**
 * @file SentimentIntensityAnalyzer.hpp
 * @brief Header for the SentimentIntensityAnalyzer class.
 * @author Adrian, Emily, Karanvir, Muhammad, Salar
 * @ingroup 26
 */
#pragma once
// #pragma execution_character_set("utf-8")

#include "SentiText.hpp"

namespace vader
{
    /**
     * @class SentimentIntensityAnalyzer
     * @brief Provide sentiment intensity scores for sentences.
     */
    class SentimentIntensityAnalyzer // Give a sentiment intensity score to sentences.
    {
    private: 
        String m_lexicon_full_filepath;
        String m_emoji_full_filepath;

        std::unordered_map<String, double> m_lexicon;
        std::unordered_map<String, String> m_emojis;
		std::vector<std::unordered_set<Char>> m_emoji_bank;

    public:
        /**
       * @brief Construct a new Sentiment Intensity Analyzer object.
       *
       * @param lexicon_file Path to the lexicon file (default: "vader_lexicon.txt").
       * @param emoji_lexicon Path to the emoji lexicon file (default: "emoji_utf8_lexicon.txt").
       */
        SentimentIntensityAnalyzer(std::string lexicon_file="vader_lexicon.txt", std::string emoji_lexicon="emoji_utf8_lexicon.txt");
        ~SentimentIntensityAnalyzer();

        /**
         * @brief Calculate sentiment polarity scores for a given text.
         *
         * @param text The input text to analyze.
         * @return Sentiment object containing polarity scores.
         */
        Sentiment polarity_scores(String text);

        /**
         * @brief Calculate sentiment valence for a given item in the text.
         *
         * @param valence Current sentiment valence.
         * @param sentitext SentiText object representing the input text.
         * @param item Current item (word or emoticon) being analyzed.
         * @param i Index of the current item in the text.
         * @param sentiments Vector of sentiment scores to be updated.
         */
        void sentiment_valence(double valence, SentiText sentitext, String item, int i, std::vector<double> &sentiments);

    private:
        static int char_byte_count(Char val);

        /**
        * @brief Initialize the lexicon dictionary.
        */
        void make_lex_dict();
        /**
         * @brief Initialize the emoji dictionary.
         */
        void make_emoji_dict();

        // The following methods are documented in their respective implementations.
        double _least_check(double valence, const std::vector<String> &words_and_emoticons, int i);
        static void _but_check(const std::vector<String> &words_and_emoticons, std::vector<double> &sentiments);
        static double _special_idioms_check(double valence, const std::vector<String> &words_and_emoticons, int i);
        static double _sentiment_laden_idioms_check(double valence, SentiText senti_text_lower); // future work
        static double _negation_check(double valence, const std::vector<String> &words_and_emoticons, int start_i, int i);
        
        double _punctuation_emphasis(String text);
        static double _amplify_ep(String text);
        static double _amplify_qm(String text);
        
        static double * _sift_sentiment_scores(std::vector<double> &sentiments);
        Sentiment score_valence(std::vector<double> &sentiments, String text);
    };
}

