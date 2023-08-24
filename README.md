# Senty-YouTube-Sentiment-Analyzer
Youtube comment section sentiment analyzer
Senty
CS3307b project, part 3. At this stage of our project, we have the following features:

Input handler
Basic sentiment analyzer (VADER)
Part of advanced sentiment analyzer
Basic video analyzer
Comment Thread -> Youtube API connection
Video -> Youtube API connection
Translation API -> Microsoft Azure Cognitive Services API (free limited version)
Installation
To run this project, please ensure that you have installed the following dependencies: Curl, Nlohmann/json.

To install on MacOS (using homebrew):

brew install nlohmann-json
brew install curl
To install on Ubuntu:

sudo apt-get install nlohmann-json-dev3
sudo apt-get install libcurl4-gnutls-dev
sudo apt-get install curl
Usage
This program currently runs in command line.

make
./main
make clean
After entering ./main, the user will be prompted:

Enter YouTube Link:
Testing: Links to try
> Video with over 100 comments > Comments in another language (auto translated) > Differently formatted link -> same as above > Video with 0 comments > Unlisted video > Private video

Output
FETCH COMMENTS API: [link] # the associated API link for comment thread info 

All Comments: # this will show all comments + their individual sentiment score
[comment]
[compound sentiment score of comment]
...
---------------------------------------------------------------------
Video Info:
Vid ID: []
Published Date: []
channelId: []
title: []
description: []
channelTitle: []
Tags: []
duration: []
caption: []
viewCount: []
likeCount: []
favoriteCount: []
commentCount: []
privacyStatus: []
---------------------------------------------------------------------
Sentiment Analysis:
There are [] positive comment(s), [] neutral comment(s), and [] negative comment(s).
Sentiment: [] ; Polarity: []
Score breakdown: 
        Overall: [], 
        Negative: [], 
        Neutral: [], 
        Positive: []
Likes:Views metric = [] engagement = []th percentile. ([]%).
Comments:Views = [] engagement, []th percentile. ([]%).
---------------------------------------------------------------------
Additional Info
VADER (Valence Aware Dictionary and sEntiment Reasoner): To calculate sentiment, we used a C++ port to this Python tool. Vader is able to use a variety of variables (e.g., slang, capitalizations, emojis, etc.) to determine the emotional intensity of an inputted text. Each word in the text input is given a score between -4 and 4. They are combined and normalized into a range between -1 (extremely negative sentiment) and 1 (extremely positive sentiment). There is variability in what compound score is defined as positive, neutral, and negative. We have assumed so far that values under -0.05 are negative, between -0.05 and 0.35 are neutral, and above 0.35 are positive, as defined on kaggle. > Plug used > More info on VADER

We have also defined good engagement rates on a video by their comment:view and like:view ratios. This threshold has been determined by information found here.
