// VADER SENTIMENT
// Translated into C++ by Aathreya Kadambi
// Edits made by Aathreya Kadambi
// See README in the repository for more information
// Uses C++ 17 to avoid C++ 20 utf-8 formatting issues?

/*
Original vaderSentiment.py Header:

coding: utf-8
# Author: C.J. Hutto
# Thanks to George Berry for reducing the time complexity from something like O(N^4) to O(N).
# Thanks to Ewan Klein and Pierpaolo Pantone for bringing VADER into NLTK. Those modifications were awesome.
# For license information, see LICENSE.TXT

"""
If you use the VADER sentiment analysis tools, please cite:
Hutto, C.J. & Gilbert, E.E. (2014). VADER: A Parsimonious Rule-based Model for
Sentiment Analysis of Social Media Text. Eighth International Conference on
Weblogs and Social Media (ICWSM-14). Ann Arbor, MI, June 2014.
"""
*/ 

#pragma once

#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>


// char8_t backwards compatibility https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1423r2.html
#if defined(__cpp_lib_char8_t)
typedef std::string String;
typedef char8_t Char;

inline std::string from_string(const String &s) 
{
    return std::string(s.begin(), s.end());
}

inline std::vector<String> split(String s)
{
    std::vector<String> res;
    bool n = true;
    unsigned int i = -1; 
    for (Char c : s)
    {
        if (n)
        {
            String strversion = "";
            strversion.push_back(c);
            res.push_back(strversion);
            i++;
            n = false;
        }
        else
        {
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v')
                n = true;
            else
                res[i].push_back(c); // possibly better to get rid of i and just do res.size() - 1
        }
    }
    return res;
}

inline std::vector<String> split(String s, Char splitter)
{
	std::vector<String> res;
	bool n = true;
	unsigned int i = -1;
	for (Char c : s)
	{
		if (n)
		{
			String strversion = "";
			strversion.push_back(c);
			res.push_back(strversion);
			i++;
			n = false;
		}
		else
		{
			if (c == splitter)
				n = true;
			else
				res[i].push_back(c); // possibly better to get rid of i and just do res.size() - 1
		}
	}
	return res;
}
#else
typedef unsigned char Char;
typedef std::string String;

inline std::string from_string(const String &s) 
{
    return s;
}
#endif


inline std::vector<std::string> split(std::string s)
{
    std::vector<std::string> res;
    bool n = true;
    unsigned int i = -1; 
    for (Char c : s)
    {
        if (n)
        {
            std::string strversion = "";
            strversion.push_back(c);
            res.push_back(strversion);
            i++;
            n = false;
        }
        else
        {
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v')
                n = true;
            else
                res[i].push_back(c); // possibly better to get rid of i and just do res.size() - 1
        }
    }
    return res;
}

inline std::vector<std::string> split(std::string s, unsigned char splitter)
{
	std::vector<std::string> res;
	bool n = true;
	unsigned int i = -1;
	for (Char c : s)
	{
		if (n)
		{
			std::string strversion = "";
			strversion.push_back(c);
			res.push_back(strversion);
			i++;
			n = false;
		}
		else
		{
			if (c == splitter)
				n = true;
			else
				res[i].push_back(c); // possibly better to get rid of i and just do res.size() - 1
		}
	}
	return res;
}


inline bool isupper(String word)
{
	return std::all_of(word.begin(), word.end(), [](unsigned char c) { return (!::isalpha(c)) || ::isupper(c); }) && std::any_of(word.begin(), word.end(), [](unsigned char c) { return ::isalpha(c); });
}

namespace vader
{
    // Precomiled Constants

    // (empirically derived mean sentiment intensity rating increase for booster words)
    #define B_INCR 0.293
    #define B_DECR -0.293

    // (empirically derived mean sentiment intensity rating increase for using ALLCAPs to emphasize a word)
    #define C_INCR 0.733
    #define N_SCALAR -0.74

    // Below we use unordered sets/maps to improve speed; vaderSentiment.py uses the list structure which has average O(n) lookups when in is used
    // but here we can use std::unordered_set to get a time complexity of O(1)
    static std::unordered_set<String> NEGATE {"aint", "arent", "cannot", "cant", "couldnt", "darent", "didnt", "doesnt",
        "ain't", "aren't", "can't", "couldn't", "daren't", "didn't", "doesn't",
        "dont", "hadnt", "hasnt", "havent", "isnt", "mightnt", "mustnt", "neither",
        "don't", "hadn't", "hasn't", "haven't", "isn't", "mightn't", "mustn't",
        "neednt", "needn't", "never", "none", "nope", "nor", "not", "nothing", "nowhere",
        "oughtnt", "shant", "shouldnt", "uhuh", "wasnt", "werent",
        "oughtn't", "shan't", "shouldn't", "uh-uh", "wasn't", "weren't",
        "without", "wont", "wouldnt", "won't", "wouldn't", "rarely", "seldom", "despite"};

    // booster/dampener 'intensifiers' or 'degree adverbs'
    // http://en.wiktionary.org/wiki/Category:English_degree_adverbs

    static std::unordered_map<String, double> BOOSTER_DICT {{"absolutely", B_INCR}, {"amazingly", B_INCR}, {"awfully", B_INCR},
        {"completely", B_INCR}, {"considerable", B_INCR}, {"considerably", B_INCR},
        {"decidedly", B_INCR}, {"deeply", B_INCR}, {"effing", B_INCR}, {"enormous", B_INCR}, {"enormously", B_INCR},
        {"entirely", B_INCR}, {"especially", B_INCR}, {"exceptional", B_INCR}, {"exceptionally", B_INCR},
        {"extreme", B_INCR}, {"extremely", B_INCR},
        {"fabulously", B_INCR}, {"flipping", B_INCR}, {"flippin", B_INCR}, {"frackin", B_INCR}, {"fracking", B_INCR},
        {"fricking", B_INCR}, {"frickin", B_INCR}, {"frigging", B_INCR}, {"friggin", B_INCR}, {"fully", B_INCR},
        {"fuckin", B_INCR}, {"fucking", B_INCR}, {"fuggin", B_INCR}, {"fugging", B_INCR},
        {"greatly", B_INCR}, {"hella", B_INCR}, {"highly", B_INCR}, {"hugely", B_INCR},
        {"incredible", B_INCR}, {"incredibly", B_INCR}, {"intensely", B_INCR},
        {"major", B_INCR}, {"majorly", B_INCR}, {"more", B_INCR}, {"most", B_INCR}, {"particularly", B_INCR},
        {"purely", B_INCR}, {"quite", B_INCR}, {"really", B_INCR}, {"remarkably", B_INCR},
        {"so", B_INCR}, {"substantially", B_INCR},
        {"thoroughly", B_INCR}, {"total", B_INCR}, {"totally", B_INCR}, {"tremendous", B_INCR}, {"tremendously", B_INCR},
        {"uber", B_INCR}, {"unbelievably", B_INCR}, {"unusually", B_INCR}, {"utter", B_INCR}, {"utterly", B_INCR},
        {"very", B_INCR},
        {"almost", B_DECR}, {"barely", B_DECR}, {"hardly", B_DECR}, {"just enough", B_DECR},
        {"kind of", B_DECR}, {"kinda", B_DECR}, {"kindof", B_DECR}, {"kind-of", B_DECR},
        {"less", B_DECR}, {"little", B_DECR}, {"marginal", B_DECR}, {"marginally", B_DECR},
        {"occasional", B_DECR}, {"occasionally", B_DECR}, {"partly", B_DECR},
        {"scarce", B_DECR}, {"scarcely", B_DECR}, {"slight", B_DECR}, {"slightly", B_DECR}, {"somewhat", B_DECR},
        {"sort of", B_DECR}, {"sorta", B_DECR}, {"sortof", B_DECR}, {"sort-of", B_DECR}};
    
    // check for sentiment laden idioms that do not contain lexicon words (future work, not yet implemented)
	static std::unordered_map<String, double> SENTIMENT_LADEN_IDIOMS{ {"cut the mustard", 2}, {"hand to mouth", -2},
		{"back handed", -2}, {"blow smoke", -2}, {"blowing smoke", -2},
		{"upper hand", 1}, {"break a leg", 2},
		{"cooking with gas", 2}, {"in the black", 2}, {"in the red" , -2},
		{"on the ball", 2}, {"under the weather", -2} };

    // check for special case idioms and phrases containing lexicon words
	static std::unordered_map<String, double> SPECIAL_CASES {{"the shit", 3}, {"the bomb", 3}, {"bad ass", 1.5}, {"badass", 1.5}, {"bus stop", 0.0},
					{"yeah right", -2}, {"kiss of death", -1.5}, {"to die for", 3},
					{"beating heart", 3.1}, {"broken heart", -2.9}};

    

    // Static Methods

    static bool negated(std::vector<String> input_words, bool include_nt=true)
    {
        // Determine if input contains negation words
        for (String &word : input_words)
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);//[](unsigned char c){ return std::tolower(c); });
        for (String word : input_words)
            if (NEGATE.count(word))
                return true;
        if (include_nt)
            for (String word : input_words)
                if (word.find("n't") != -1)
                    return true;
        /* Semi-Untranslated portion that was commented: [Possible TODO]
        '''if "least" in input_words:
            i = input_words.index("least")
            if i > 0 and input_words[i - 1] != "at":
                return True'''
        */
        return false;
    }

    static double normalize(double score, double alpha=15)
    {
        // Normalize the score to be between -1 and 1 using an alpha that approximates the max expected value
        double norm_score = score / sqrt((score * score) + alpha); // TODO: Look into fastest implementations of ISR
        if (norm_score < -1.0)
            return -1;
        else if (norm_score > 1.0)
            return 1;
        return norm_score;
    }

    static bool allcap_differential(std::vector<String> words)
    {
        /*
        Check whether just some words in the input are ALL CAPS
        :param vector words: The words to inspect
        :returns: `true` if some but not all items in `words` are ALL CAPS
        */
        int allcap_words = 0;
        for (String word : words)
            if (isupper(word))
                allcap_words++;
        int cap_differential = words.size() - allcap_words; // this is actually possibly buggy, behavior on emojis/emoticons are unknown TODO: investigate
        return 0 < cap_differential && cap_differential < words.size(); // more space efficient than storing in a variable
    }

    static double scalar_inc_dec(String word, double valence, bool is_cap_diff)
    {
        // Check if the preceding words increase, decrease, or negate/nullify the valence
        double scalar = 0.0;
        String oword = word;
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (BOOSTER_DICT.count(word))
        {
            scalar = BOOSTER_DICT[word];
            if (valence < 0)
                scalar *= -1;
            // check if booster/dampener word is ALLCAPS (while others aren't)
            if (is_cap_diff && isupper(oword))
            {
                if (valence > 0) // should be scalar > 0?
                    scalar += C_INCR;
                else
                    scalar -= C_INCR;
            }
        }
        return scalar;
    }

    struct Sentiment
    {
        double neg = 0.0;
        double neu = 0.0;
        double pos = 0.0;
        double compound = 0.0;
    };
}
