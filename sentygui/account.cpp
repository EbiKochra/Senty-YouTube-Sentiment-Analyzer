#include <vector>
#include "account.h"

Account::Account() { }

// Account::Account(string username, string password, string securityQuestion, string securityAnswer) 
//     : username{username}, password{password}, securityQuestion{securityQuestion}, securityAnswer{securityAnswer} { }

Account::Account(string username, string password, string securityQuestion, string securityAnswer, vector<string> videoHistory) 
    : username{username}, password{password}, securityQuestion{securityQuestion}, securityAnswer{securityAnswer}, videoHistory{videoHistory} { }


string Account::getUsername() const {
    return username;
}

void Account::setUsername(string username) {
    this->username = username;
}

string Account::getPassword() const {
    return password;
}

void Account::setPassword(string password) {
    this->password = password;
}

string Account::getSecurityQuestion() const {
    return securityQuestion;
}

void Account::setSecurityQuestion(string securityQuestion) {
    this->securityQuestion = securityQuestion;
}

string Account::getSecurityAnswer() const {
    return securityAnswer;
}

void Account::setSecurityAnswer(string securityAnswer) {
    this->securityAnswer = securityAnswer;
}

vector<string> Account::getHistory() const {
    return videoHistory;
}
void Account::addHistory(string videoURL) {
    videoHistory.push_back(videoURL);
}
