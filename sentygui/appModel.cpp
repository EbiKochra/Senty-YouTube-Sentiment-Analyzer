#include "appModel.h"
#include "iView.h"

using namespace std;

AppModel::AppModel() {
    videoManager = new VideoManager(this);
}

void AppModel::addListener(IView *view) {
    views.push_back(view);
}

void AppModel::notifyAll() {
    cout << "AppModel::notify() starts" << endl;
    for (auto view : views) {
        view->updateView(*this);
    }
}


AccountManager & AppModel::getAccountManager() {
    return accountManager;
}

void AppModel::login(string username, string password) {
    cout << "AppModel::login() starts" << endl;
    currentUser =  accountManager.login(username, password);
    notifyAll();
}

bool AppModel::logout() {
    if (currentUser != nullptr) {
        currentUser = nullptr;
        return true;
    } else {
        return false;
    }
}

bool AppModel::deleteAccount(std::string username) {
    accountManager.deleteAccount(username);
    return true;
}



bool AppModel::createAccount(std::string username, std::string password, std::string securityQuestion, std::string securityAnswer) {
    cout << "create account starting..." << endl;
    bool created = accountManager.createAccount(username, password, securityQuestion, securityAnswer);
    // notifyAll();
    return created;
 }

string AppModel::getUserSecurityQuestion(const std::string& username) const {
    json accountsJson;
    std::ifstream inFile("accounts.json");
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }

    if (accountsJson.is_array()) {
        for (const auto& acc : accountsJson) {
            auto usernameIt = acc.find("username");
            if (usernameIt != acc.end() && usernameIt->get<std::string>() == username) {
                return acc["securityQuestion"].get<std::string>();
            }
        }
    }
    return "";
}
bool AppModel::validateSecurityInfo(std::string username, std::string securityAnswer) {
    return accountManager.validateSecurityAnswer(username, securityAnswer);
}

bool AppModel::resetUserPassword(std::string username, std::string newPassword, std::string securityQuestion, std::string securityAnswer) {
    if (validateSecurityInfo(username, securityAnswer)) {
        accountManager.resetPassword(username, newPassword);
        return true;
    }
    return false;
}


vector<string> AppModel::getHistory() const {
    std::shared_ptr<Account> currentUser = getCurrentUser();
    if (currentUser != nullptr) {
        return currentUser->getHistory();
    }
    return {};
}


shared_ptr<Account> AppModel::getCurrentUser() const {
    return currentUser;
}



int AppModel::getState() const {
    return state;
}

void AppModel::promptVideoInput() {
    // ...
    // TODO: copy from textUI
    state = 2;
    notifyAll();
}

VideoManager *AppModel::getVideoManager() {
    return videoManager;
}
void AppModel::videoCompare(int numVideos) {
    // videoManager.analysis(url1, url2);
    // .... analyzing ...
    // analysisResult = "The result of video " + url1 + " and " + url2;
    state = 2;
    notifyAll();
}

string AppModel::getAnalysisResult() const {
    return analysisResult;
}

void AppModel::addVideoToHistoryForCurrentUser(string videoURL) {
    accountManager.addVideoHistory(videoURL, *currentUser);

}

void AppModel::updateAnalyzingProgress(int progress) {
    this->progress = progress;
    notifyAll();
}

int AppModel::getProgress() const {
    return progress;
}
