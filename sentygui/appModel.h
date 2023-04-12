#ifndef APPMODEL_H
#define APPMODEL_H

#include <vector>
#include <memory>
#include "videoManager.h"
#include "accountManager.h"


class IView;

class AppModel {
        AccountManager accountManager;
        VideoManager *videoManager;
        std::shared_ptr<Account> currentUser = nullptr;
        std::string analysisResult;
        int state = 0;
        int progress = 0;
    protected:
        std::vector<IView *> views;
        void notifyAll();
    public:
        AppModel();
        void addListener(IView *view);
        int getState() const;
        AccountManager &getAccountManager();
        void login(std::string username, std::string password);
        vector<string> getHistory() const;
        std::shared_ptr<Account> getCurrentUser() const;
        bool logout();
        bool deleteAccount(std::string username);
        bool createAccount(std::string username, std::string password, std::string securityQuestion, std::string securityAnswer);
        string getUserSecurityQuestion(const std::string& username) const;
        bool validateSecurityInfo(std::string username, std::string securityAnswer);
        bool resetUserPassword(std::string username, std::string newPassword, std::string securityQuestion, std::string securityAnswer);

        VideoManager *getVideoManager();
        void promptVideoInput();
        void videoCompare(int numVideos);
        std::string getAnalysisResult() const;
        void addVideoToHistoryForCurrentUser(std::string videoURL);

        void updateAnalyzingProgress(int progress);
        int getProgress() const;

};

#endif 