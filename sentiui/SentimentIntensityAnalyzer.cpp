/* Name: SentimentIntensityAnalyzer.cpp
 * Desc: Implements the SentimentIntensityAnalyzer class, using VADER: Valence Aware Dictionary for Sentiment Reasoning
 * Authors: Adrian, Emily, Karanvir, Muhammad, Salar
 * Group: 26
 * */

// Implements SentimentIntensityAnalyzer class
#include "SentimentIntensityAnalyzer.hpp"
#include "cppemojihandler.hpp"

extern std::string from_string(const String &s);
// Namespace for VADER sentiment analyzer
namespace vader
{
    /**
     * @brief Constructor for SentimentIntensityAnalyzer class
     * @param lexicon_file Path to lexicon file for VADER sentiment analysis
     * @param emoji_lexicon Path to emoji lexicon file for VADER sentiment analysis
     */
	SentimentIntensityAnalyzer::SentimentIntensityAnalyzer(std::string lexicon_file, std::string emoji_lexicon)
	{
		this->m_lexicon_full_filepath = lexicon_file; // possibly don't need the full filepath?
		this->m_emoji_full_filepath = emoji_lexicon;
		this->make_lex_dict();
		this->make_emoji_dict();

		this->m_emoji_bank = create_emoji_bank(m_emojis);
	}

    /**
     * @brief Destructor for SentimentIntensityAnalyzer class
     */
	SentimentIntensityAnalyzer::~SentimentIntensityAnalyzer()
	{
	}

    /**
     * @brief Computes the sentiment scores for a given text
     * @param text Input text to compute sentiment scores for
     * @return A Sentiment object containing the polarity score (sentiments) and text of the comment (text)
     */
	Sentiment SentimentIntensityAnalyzer::polarity_scores(String text)
	{
		// convert emojis to their textual descriptions
		String text_no_emoji = "";
		bool prev_space = true;
		for (int i = 0; i < text.length(); i++) // Char c : text
		{
			Char c = text[i];
			String temp = ""; temp.push_back(c);
			if (this->m_emoji_bank[0].count(c))
				temp = emoji_chars_from_start(text.substr(i), this->m_emoji_bank);
			if ((temp.length() > 0) && (this->m_emojis.count(temp)))
			{
				String description = this->m_emojis[temp];
				if (!prev_space)
					text_no_emoji += " ";
				text_no_emoji += description + " "; // so that emoji sentiments can be parsed separately
				prev_space = true;
			}
			/*
			if (this->m_emojis.count(temp) > 0)
			{
				// get teh textual description
				String description = this->m_emojis[temp];
				if (!prev_space)
					text_no_emoji += " ";
				text_no_emoji += description;
				prev_space = false;
			}*/
			else if (!(prev_space && temp == " "))
			{
				text_no_emoji += temp;
				prev_space = c == ' ';
			}
			i += temp.length() - 1;
		}
		if (text_no_emoji[text_no_emoji.length() - 1] == ' ')
			text_no_emoji.erase(text_no_emoji.length() - 1);
		text = text_no_emoji;

		SentiText sentitext(text);
		std::vector<double> sentiments;

		std::vector<String> words_and_emoticons = *sentitext.get_words_and_emoticons();

		for (int i = 0; i < words_and_emoticons.size(); i++)
		{
			double valence = 0;
			// check for vader_lexicon words that may be used as modifiers or negations
			String lword = words_and_emoticons[i];
			std::transform(lword.begin(), lword.end(), lword.begin(), ::tolower);
			String lnword;
			if (i < words_and_emoticons.size() - 1)
			{
				lnword = words_and_emoticons[i + 1];
				std::transform(lnword.begin(), lnword.end(), lnword.begin(), ::tolower);
			}
			if (BOOSTER_DICT.count(lword) > 0)
				sentiments.push_back(valence);
			else if ((i < words_and_emoticons.size() - 1) && (lword == "kind") && (lnword == "of"))
				sentiments.push_back(valence);
			else
				this->sentiment_valence(valence, sentitext, words_and_emoticons[i], i, sentiments);
		}

		this->_but_check(words_and_emoticons, sentiments);
		return this->score_valence(sentiments, text);
	}


    /**
     * @brief Computes the number of bytes used to store a char from the specified value ranges
     * @param val A character to determine the number of bytes to store
     * @return Returns an integer representing the corresponding number of bytes it takes to store that Char
     */
	int SentimentIntensityAnalyzer::char_byte_count(Char val)
	{
		if (val < 128) {
			return 1;
		}
		else if (val < 224) {
			return 2;
		}
		else if (val < 240) {
			return 3;
		}
		else {
			return 4;
		}
	}

    /**
   * @brief Reads in the file 'vader_lexicon.txt' and builds a dictionary (vector<String> object)
   * @note N/A, reads in a .txt file
   * @return N/A, but builds a vector of type String to be used as a lexical dictionary
   */
	void SentimentIntensityAnalyzer::make_lex_dict() // TODO: in the future maybe switch to a C-style file reading implementation if possible
	{
		std::ifstream in_file(m_lexicon_full_filepath);
		String line;
		while (std::getline(in_file, line))
		{
			if (line == "")
				continue;
			std::vector<String> tokens = split(line, '\t');
			String word = tokens[0];
			String measure = tokens[1];

			m_lexicon[word] = std::stod(from_string(measure));
		}
	}
    /**
      * @brief Reads in the file 'emoji_utf8_lexicon.txt' and builds a dictionary (vector<String> object)
      * @note N/A, reads in a .txt file
      * @return N/A, but builds a vector of type String to be used as an emoji dictionary
      */
	void SentimentIntensityAnalyzer::make_emoji_dict()
	{
		std::ifstream in_file(m_emoji_full_filepath);
		String line;
		while (std::getline(in_file, line))
		{
			if (line == "")
				continue;
			std::vector<String> tokens = split(line, '\t');
			String emoji = tokens[0];
			String description = tokens[1];

			m_emojis[emoji] = description;
		}
	}

    /**
     * @brief Computes the sentiment valence of a given word or emoticon
     * @param valence The sentiment valence to be computed
     * @param sentitext A SentiText object containing the text to analyze
     * @param item A word or emoticon to analyze
     * @param i The index of the word or emoticon in the text
     * @param sentiments A reference to a vector of double values representing the sentiments
     */
	void SentimentIntensityAnalyzer::sentiment_valence(double valence, SentiText sentitext, String item, int i, std::vector<double> &sentiments)
	{
		bool is_cap_diff = sentitext.isCapDiff();
		std::vector<String> words_and_emoticons = *sentitext.get_words_and_emoticons();
		String item_lowercase = item;
		std::transform(item_lowercase.begin(), item_lowercase.end(), item_lowercase.begin(), ::tolower);
		if (this->m_lexicon.count(item_lowercase) > 0)
		{
			// get the sentiment valence
			valence = this->m_lexicon[item_lowercase];

			String next_word;
			if (i != words_and_emoticons.size() - 1)
			{
				next_word = words_and_emoticons[i + 1];
				std::transform(next_word.begin(), next_word.end(), next_word.begin(), ::tolower);
			}

			// check for "no" as negation for an adjacent lexicon item vs "no" as its own stand-alone lexicon item
			if ((item_lowercase == "no") && (i != words_and_emoticons.size() - 1) && (this->m_lexicon.count(next_word) > 0))
				// don't use valence of "no" as a lexicon item. Instead set it's valence to 0.0 and negate the next item
				valence = 0.0;
			// check if sentiment laden word is in ALL CAPS (while others aren't)
			if ((isupper(item)) && (is_cap_diff) && (!sentitext.get_is_emoticon()[i]))
			{
				if (valence > 0)
					valence += C_INCR;
				else
					valence -= C_INCR;
			}

			for (int start_i = 0; start_i < 3; start_i++)
			{
				// dampen the scalar modifier of preceding words and emotions
				// (excluding the ones that immediately preceed the item) based
				// on their distance from the current item.

				// TOOD: consider switching SentiText to include a m_l_words_and_emoticons so everything is already in loewr case
				if (i > start_i)
				{
					String temp = words_and_emoticons[i - (start_i + 1)];
					std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
					if (m_lexicon.count(temp) == 0)
					{
						double s = scalar_inc_dec(words_and_emoticons[i - (start_i + 1)], valence, is_cap_diff);
						if (s != 0)
						{
							if (start_i == 1)
								s *= 0.95;
							else if (start_i == 2)
								s *= 0.9;
						}
						valence = valence + s;
						valence = this->_negation_check(valence, words_and_emoticons, start_i, i);
						if (start_i == 2)
							valence = this->_special_idioms_check(valence, words_and_emoticons, i);
					}
				}
			}
			valence = this->_least_check(valence, words_and_emoticons, i);
		}
		sentiments.push_back(valence);
	}

    /**
 * @brief Checks for negation case using "least" and updates valence accordingly.
 * @param valence The current valence value.
 * @param words_and_emoticons The vector of words and emoticons.
 * @param i The index of the current word/emoticon.
 * @return Updated valence value after considering the "least" negation case.
 */
	double SentimentIntensityAnalyzer::_least_check(double valence, const std::vector<String> &words_and_emoticons, int i)
	{
		// check for negation case using "least"
		String lword, llword;
		if (i > 0)
		{
			lword = words_and_emoticons[i - 1];
			std::transform(lword.begin(), lword.end(), lword.begin(), ::tolower);
		}
		if (i > 1)
		{
			llword = words_and_emoticons[i - 2];
			std::transform(llword.begin(), llword.end(), llword.begin(), ::tolower);
		}
		if (/*this->m_lexicon.count(lword) == 0 &&*/ lword == "least") // I don't know why they check if its not in the lexicon
		{
			if (i > 1)
			{
				if ((llword != "at") && (llword != "very"))
					valence *= N_SCALAR;
			}
			else if (i > 0)
				valence *= N_SCALAR;
		}
		return valence;
	}

    /**
    * @brief Modifies sentiment values in the presence of the contrastive conjunction 'but'.
    * @param words_and_emoticons The vector of words and emoticons.
    * @param sentiments The vector of sentiment values.
    */
	void SentimentIntensityAnalyzer::_but_check(const std::vector<String> &words_and_emoticons, std::vector<double> &sentiments)
	{
		// check for modification in sentiment due to contrastive conjunction 'but'
		std::vector<int> bi;
		for (int i = 0; i < words_and_emoticons.size(); i++)
		{
			String temp = words_and_emoticons[i];
			std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
			if (temp == "but")
				bi.push_back(i);
		}
		if (bi.size() > 0)
		{
			for (int i = 0; i < sentiments.size(); i++) // Original vaderSentiment only uses first 'but' instance, TODO use more
			{
				if (i < bi[0])
				{
					sentiments.insert(sentiments.begin() + i, sentiments[i] * 0.5); // wait isn't this a bit arbitrary/should it negate it or make it void or smth else
					sentiments.erase(sentiments.begin() + i + 1);
				}
				else if (i > bi[0])
				{
					sentiments.insert(sentiments.begin() + i, sentiments[i] * 1.5); // wait isn't this a bit arbitrary/should it negate it or make it void or smth else
					sentiments.erase(sentiments.begin() + i + 1);
				}
			}
		}
	}

    /**
     * @brief Checks for special idioms and updates valence accordingly.
     * @param valence The current valence value.
     * @param words_and_emoticons The vector of words and emoticons.
     * @param i The index of the current word/emoticon.
     * @return Updated valence value after considering the special idioms.
     */
	double SentimentIntensityAnalyzer::_special_idioms_check(double valence, const std::vector<String> &words_and_emoticons, int i)
	{
		std::vector<String> words_and_emoticons_lower;
		for (String word : words_and_emoticons)
		{
			std::transform(word.begin(), word.end(), word.begin(), ::tolower);
			words_and_emoticons_lower.push_back(word);
		}

		String onezero = words_and_emoticons_lower[i - 1] + ' ' + words_and_emoticons_lower[i];
		String twoonezero = words_and_emoticons_lower[i - 2] + ' ' + onezero;
		String twoone = words_and_emoticons_lower[i - 2] + ' ' + words_and_emoticons_lower[i - 1];
		String threetwoone = words_and_emoticons_lower[i - 3] + ' ' + twoone;
		String threetwo = words_and_emoticons_lower[i - 3] + ' ' + words_and_emoticons_lower[i - 2];
		String sequences[5] = { onezero, twoonezero, twoone, threetwoone, threetwo };

		for (String seq : sequences)
		{
			if (SPECIAL_CASES.count(seq) > 0)
			{
				valence = SPECIAL_CASES[seq];
				break; // TODO: code without breaks
			}
		}

		if (words_and_emoticons_lower.size() - 1 > i)
		{
			String zeroone = words_and_emoticons_lower[i] + ' ' + words_and_emoticons_lower[i + 1];
			if (SPECIAL_CASES.count(zeroone))
				valence = SPECIAL_CASES[zeroone];
			if (words_and_emoticons_lower.size() - 1 > i + 1)
			{
				String zeroonetwo = zeroone + ' ' + words_and_emoticons[i + 2];
				if (SPECIAL_CASES.count(zeroonetwo))
					valence = SPECIAL_CASES[zeroone];
			}
		}

		// check for booster/dampener bi-grams such as 'sort of' or 'kind of'
		String n_grams[3] = { threetwoone, threetwo, twoone };
		for (String n_gram : n_grams)
			if (BOOSTER_DICT.count(n_gram) > 0)
				valence = valence + BOOSTER_DICT[n_gram];

		return valence;
	}
    /**
     * @brief Placeholder for sentiment laden idioms check.
     * @param valence The current valence value.
     * @param senti_text_lower The SentiText object with lowercase text.
     * @return The unchanged valence value, as this function is not implemented.
     */
	double SentimentIntensityAnalyzer::_sentiment_laden_idioms_check(double valence, SentiText senti_text_lower) // TODO
	{
		return valence;
	}

    /**
     * @brief Checks for negation in the given context and updates valence accordingly.
     * @param valence The current valence value.
     * @param words_and_emoticons The vector of words and emoticons.
     * @param start_i The starting index for checking negation.
     * @param i The index of the current word/emoticon.
     * @return Updated valence value after considering negation.
     */
	double SentimentIntensityAnalyzer::_negation_check(double valence, const std::vector<String> &words_and_emoticons, int start_i, int i)
	{
		std::vector<String> words_and_emoticons_lower;
		for (String word : words_and_emoticons)
		{
			std::transform(word.begin(), word.end(), word.begin(), ::tolower);
			words_and_emoticons_lower.push_back(word);
		}
		std::vector<String> temp;
		if ((0 <= i - (start_i+1)) && (i - (start_i+1) < words_and_emoticons_lower.size()))
			temp.push_back(words_and_emoticons_lower[i - (start_i+1)]);
		if (start_i == 0)
		{
			if (negated(temp)) // 1 word preceding lexicon word (w/o stopwords)
				valence *= N_SCALAR;
		}
		else if (start_i == 1)
		{
			if ((words_and_emoticons_lower[i - 2] == "never") && (words_and_emoticons_lower[i - 1] == "so" || words_and_emoticons_lower[i - 1] == "this"))
				valence *= 1.25;
			else if ((words_and_emoticons_lower[i - 2] == "without") && (words_and_emoticons_lower[i - 1] == "doubt"))
				valence += 0;
			else if (negated(temp)) // 2 words preceding the lexicon word position
				valence *= N_SCALAR;
		}
		else if (start_i == 2)
		{
			if ((words_and_emoticons_lower[i - 3] == "never") 
				&& ((words_and_emoticons_lower[i - 2] == "so") 
				|| (words_and_emoticons_lower[i - 2] == "this") 
				|| (words_and_emoticons_lower[i - 1] == "so") 
				|| (words_and_emoticons_lower[i - 1] == "this")))
				valence *= 1.25;
			else if ((words_and_emoticons_lower[i - 3] == "without") &&
				((words_and_emoticons_lower[i - 2] == "doubt") || (words_and_emoticons_lower[i - 1] == "doubt")))
				valence += 0;
			else if (negated(temp)) // 3 words preceding the lexicon word position
				valence *= N_SCALAR;
		}
		return valence;
	}

    /**
     * @brief Computes the punctuation emphasis for the given text.
     * @param text The input text.
     * @return The punctuation emphasis value for the text.
     */
	double SentimentIntensityAnalyzer::_punctuation_emphasis(String text)
	{
		// add emphasis from exclamation points and question marks
		double ep_amplifier = this->_amplify_ep(text);
		double qm_amplifier = this->_amplify_qm(text);
		double punct_emph_amplifier = ep_amplifier + qm_amplifier;
		return punct_emph_amplifier;
	}

    /**
     * @brief Amplify sentiment score based on exclamation points in the text.
     *
     * @param text Input text to be analyzed for exclamation points.
     * @return Amplified sentiment score for exclamation points.
     */
	double SentimentIntensityAnalyzer::_amplify_ep(String text)
	{
		// check for added emphasis resulting from exclamation points (up to 4 of them)
		int ep_count = std::count(text.begin(), text.end(), '!');
		if (ep_count > 4)
			ep_count = 4;
		// (empirically derived mean sentiment intensity rating increase for exclamation points)
		//double ep_amplifier = ep_count * 0.292;
		return ep_count * 0.292; //ep_amplifier;
	}

    /**
     * @brief Amplify sentiment score based on question marks in the text.
     *
     * @param text Input text to be analyzed for question marks.
     * @return Amplified sentiment score for question marks.
     */
	double SentimentIntensityAnalyzer::_amplify_qm(String text)
	{
		// check for added emphasis resulting from question marks(2 or 3 + )
		int qm_count = std::count(text.begin(), text.end(), '?');
		double qm_amplifier = 0;
		if (qm_count > 1)
		{
			if (qm_count <= 3)
				// (empirically derived mean sentiment intensity rating increase for question marks)
				qm_amplifier = qm_count * 0.18;
			else
				qm_amplifier = 0.96;
		}
		return qm_amplifier;
	}

    /**
     * @brief Separate positive, negative and neutral sentiment scores from sentiments.
     *
     * @param sentiments Vector of sentiment scores to be sifted.
     * @return Pointer to an array with pos_sum, neg_sum, and neu_count.
     */
	double * SentimentIntensityAnalyzer::_sift_sentiment_scores(std::vector<double> &sentiments)
	{
		// want separate positive versus negative sentiment scores
		double pos_sum = 0.0;
		double neg_sum = 0.0;
		double neu_count = 0;
		for (double sentiment_score : sentiments)
		{
			if (sentiment_score > 0)
				pos_sum += (sentiment_score + 1); // compensates for neutral words that are counted as 1
			else if (sentiment_score < 0)
				neg_sum += (sentiment_score - 1); // when used with abs(), compensates for neutrals
			else
				neu_count += 1;
		}
		// double* packet[3] = new double[3];
		// packet[0] = pos_sum;
		// packet[1] = neg_sum;
		// packet[2] = neu_count;
		// double packet[3] = { pos_sum, neg_sum, neu_count };
		double* packet = (double*)malloc(3 * sizeof(double));
		// 		if (packet == NULL) {
		//     // handle error if allocation fails
		// }
		packet[0] = pos_sum;
		packet[1] = neg_sum;
		packet[2] = neu_count;

		return packet;
	}

    /**
     * @brief Calculate sentiment valence scores from given sentiments and text.
     *
     * @param sentiments Vector of sentiment scores to be processed.
     * @param text Input text to be analyzed for punctuation emphasis.
     * @return Sentiment object containing compound, pos, neg, and neu sentiment scores.
     */
	Sentiment SentimentIntensityAnalyzer::score_valence(std::vector<double> &sentiments, String text)
	{
		Sentiment sentiment_dict;

		sentiment_dict.compound = 0.0;
		sentiment_dict.pos = 0.0;
		sentiment_dict.neg = 0.0;
		sentiment_dict.neu = 0.0;
		if (sentiments.size() > 0)
		{
			double sum_s = 0;
			for (double sentiment_score : sentiments)
				sum_s += sentiment_score;
			// compute and add emphasis from punctuation in text
			double punct_emph_amplifier = this->_punctuation_emphasis(text);
			if (sum_s > 0)
				sum_s += punct_emph_amplifier;
			else if (sum_s < 0)
				sum_s -= punct_emph_amplifier;

			sentiment_dict.compound = normalize(sum_s); // vader normalize
			// discriminate between positive, negative and neutral sentiment scores
			double * packet = this->_sift_sentiment_scores(sentiments);
			double pos_sum = packet[0];
			double neg_sum = packet[1];
			double neu_count = packet[2];

			if (pos_sum > abs(neg_sum))
				pos_sum += punct_emph_amplifier;
			else if (pos_sum < abs(neg_sum))
				neg_sum -= punct_emph_amplifier;

			double total = pos_sum + abs(neg_sum) + neu_count;
			sentiment_dict.pos = abs(pos_sum / total);
			sentiment_dict.neg = abs(neg_sum / total);
			sentiment_dict.neu = abs(neu_count / total);
		}

		return sentiment_dict;
	}

}

