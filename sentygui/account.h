#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <vector>

using namespace std;

class Account {
        string username;
        string password;
        string securityQuestion;
        string securityAnswer;
        vector<string> videoHistory;
    public:
        Account();
        // Account(string username, string password, string securityQuestion, string securityAnswer);
        Account(string username, string password, string securityQuestion, string securityAnswer, vector<string> videoHistory);

        string getUsername() const;
        void setUsername(string username);
        string getPassword() const;
        void setPassword(string password);
        string getSecurityQuestion() const;
        void setSecurityQuestion(string securityQuestion);
        string getSecurityAnswer() const;
        void setSecurityAnswer(string securityAnswer);
        vector<string> getHistory() const;
        void addHistory(string videoURL);
};

#endif
