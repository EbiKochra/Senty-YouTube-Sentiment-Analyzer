
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "bcrypt/BCrypt.hpp"

// for protecting password inputs, replacing with *
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif


#include "accountManager.h"
#include "account.h"

using json = nlohmann::json;

// -------------- LoginException -----------------
LoginException::LoginException(string message) : message{message} { }

std::string LoginException::what() const {
    return message;
}

// -------------- AccountaManagerException -----------------
AccountaManagerException::AccountaManagerException(AccountaManagerError error) : error{error} { }

std::string AccountaManagerException::what() const {
    switch(error) {
        case AccountaManagerError::AccountNotExist:
            return "Account does not exist.";
        case AccountaManagerError::InvalidSecurityAnswer:
            return "The answer to the security question is incorrect.";
        case AccountaManagerError::UsernameTaken:
            return "Username Taken.";
        case AccountaManagerError::UsernameIsEmpty:
            return "Username is mandatory.";
        case AccountaManagerError::PasswordIsEmpty:
            return "Password is mandatory.";
        case AccountaManagerError::SecurityQuesitonIsEmpty:
            return "Security Question is mandatory.";
        case AccountaManagerError::SecurityAnswerIsEmpty:
            return "Security Answer is mandatory.";

    }
    return "";
}

// ---------------- AccountManager ----------------
AccountManager::AccountManager() : loggedIn(false)  {}

bool AccountManager::createAccount(const Account *account) {
    return createAccount(account->getUsername(), account->getPassword(), account->getSecurityQuestion(), account->getSecurityAnswer());
}

bool AccountManager::createAccount(const std::string& username, const std::string& password, const std::string& securityQuestion, const std::string& securityAnswer) {
    try {

        if (username == "") {
            throw AccountaManagerException(AccountaManagerError::UsernameIsEmpty);
        } else if (password == "") {
            throw AccountaManagerException(AccountaManagerError::PasswordIsEmpty);
        } else if (securityQuestion == "") {
            throw AccountaManagerException(AccountaManagerError::SecurityQuesitonIsEmpty);
        } else if (securityAnswer == "") {
            throw AccountaManagerException(AccountaManagerError::SecurityAnswerIsEmpty);
        }
        setUsername(username);
        
        // VALIDATE USERNAME
         json accountsJson;
        // Read the existing JSON data from the file
        std::ifstream inFile("accounts.json");
        if (inFile.is_open()) {
            inFile >> accountsJson;
            inFile.close();
        }
    
        // Check if the JSON data is an array
        if (accountsJson.is_array()) {
            // Iterate through the array and find the matching account
            for (const auto& account : accountsJson) {
                if (account["username"] == username) {
                    throw AccountaManagerException(AccountaManagerError::UsernameTaken);
                }
            }
        }

        setPassword(password);
        setSecurity(securityQuestion, securityAnswer);
        
        saveToFile("accounts.json");

        return true;
    } catch (std::exception e) {
        std::cerr << "Could not create account." << std::endl;
        return false;
    }
}

void AccountManager::setUsername(const std::string& usernameIn) {
    username = usernameIn;
}
void AccountManager::setPassword(const std::string& passwordIn) {
    if (!passwordIn.empty()) {
        password = encryptPassword(passwordIn);
    }
}

std::string AccountManager::encryptPassword(const std::string& passwordIn) {
    return BCrypt::generateHash(passwordIn);
}
bool AccountManager::validateEncryptedPassword(const std::string passwordIn, const std::string & passwordStored) {
    return BCrypt::validatePassword(passwordIn, passwordStored);
}

void AccountManager::setSecurity(const std::string& securityQIn, const std::string& securityAIn) {
    securityQuestion = securityQIn;
    securityAnswer = securityAIn;
}

void AccountManager::deleteAccount(const std::string& username) {

    if (!isLoggedIn()) {
        return;
    }
    json accountsJson;

    // Read the existing JSON data from the file
    std::ifstream inFile("accounts.json");
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }

    // Check if the JSON data is an array
    if (accountsJson.is_array()) {
        // Iterate through the array and find the account to delete
        for (auto it = accountsJson.begin(); it != accountsJson.end(); ++it) {
            if ((*it)["username"] == username) {
                // Erase the account from the array
                accountsJson.erase(it);
                std::cout << "Account with username '" << username << "' has been deleted." << std::endl;
                break;
            }
        }
    }

    // Save the updated JSON data to the file
    std::ofstream outFile("accounts.json");
    outFile << accountsJson.dump(4);
    outFile.close();
}


std::shared_ptr<Account> AccountManager::login(const std::string& username, const std::string& password) {
    json accountsJson;

    // Read the existing JSON data from the file
    std::ifstream inFile("accounts.json");
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }
    
    // Check if the JSON data is an array
    if (accountsJson.is_array()) {
        // Iterate through the array and find the matching account
        for (const auto& account : accountsJson) {
            if (account["username"] == username && validateEncryptedPassword(password, account["password"])) {
                loggedIn = true; // Set loggedIn to true when login is successful
                return std::make_shared<Account>(account["username"], "", account["securityQuestion"], account["securityAnswer"], account["history"]);
            }
        }
    }
    throw LoginException("Login failed: invalid username or password!");
}

bool AccountManager::isLoggedIn() const {
    return loggedIn;
}


std::string AccountManager::getSecurityQuestion(std::string username) const {
    json accountsJson;
    // Read the existing JSON data from the file
    std::ifstream inFile("accounts.json");
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }

    json* account = nullptr;
    if (accountsJson.is_array()) {
        // std::cout << "hi" << std::endl;
        for (auto& acc : accountsJson) {
            auto usernameIt = acc.find("username");
            if (usernameIt != acc.end() && usernameIt->get<std::string>() == username) {
                account = &acc;
                break;
            }
        }
    }
    if (account == nullptr) {
        throw AccountaManagerException(AccountaManagerError::AccountNotExist);
    }
    return account->find("securityQuestion")->get<std::string>();

}

// Pre-condition: username exists
bool AccountManager::validateSecurityAnswer(std::string username, std::string securityAnswer) const {
    json accountsJson;
    std::ifstream inFile("accounts.json");
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }

    if (securityAnswer == "") {
        throw AccountaManagerException(AccountaManagerError::SecurityAnswerIsEmpty);
    }

    // json* account = nullptr;
    if (accountsJson.is_array()) {
        for (auto& acc : accountsJson) {
            auto usernameIt = acc.find("username");
            if (usernameIt != acc.end() && usernameIt->get<std::string>() == username) {
                string secQuestion = acc.find("securityQuestion")->get<std::string>();
                string secAnswer = acc.find("securityAnswer")->get<std::string>();
                // return securityAnswer == secAnswer;
                if (securityAnswer != secAnswer) {
                    throw AccountaManagerException(AccountaManagerError::InvalidSecurityAnswer);
                }
                return true;
            }
        }
    }
    throw AccountaManagerException(AccountaManagerError::AccountNotExist);
    // return false;
}

void AccountManager::resetPassword(std::string username, std::string newPassword) {
    if (username == "") {
        throw AccountaManagerException(AccountaManagerError::UsernameIsEmpty);
    } else if (newPassword == "") {
        throw AccountaManagerException(AccountaManagerError::PasswordIsEmpty);
    }

    json accountsJson;
    std::ifstream inFile("accounts.json");
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }
    if (accountsJson.is_array()) {
        for (auto& acc : accountsJson) {
            auto usernameIt = acc.find("username");
            if (usernameIt != acc.end() && usernameIt->get<std::string>() == username) {
                acc["password"] = encryptPassword(newPassword);
                
                // Save the updated JSON data to the file
                std::ofstream outFile("accounts.json");
                outFile << accountsJson.dump(4);
                outFile.close();
                return;
            }
        }
    }

}

void AccountManager::addVideoHistory(string videoURL, Account &account) {
    account.addHistory(videoURL);

    json accountsJson;
    std::ifstream inFile("accounts.json");
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }
    string username = account.getUsername();
    if (accountsJson.is_array()) {
        for (auto& acc : accountsJson) {
            auto usernameIt = acc.find("username");
            if (usernameIt != acc.end() && usernameIt->get<std::string>() == username) {
                // acc["history"] = account.getHistory();
                acc["history"].push_back(videoURL);
                
                // Save the updated JSON data to the file
                std::ofstream outFile("accounts.json");
                outFile << accountsJson.dump(4);
                outFile.close();
                return;
            }
        }
    }
}


std::string AccountManager::getUsername() const {
    return username;
}

std::string AccountManager::getPassword() const {
    return password;
}

void AccountManager::saveToFile(const std::string& filename) {
    json accountsJson;

    // Read the existing JSON data from the file
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }

    // Check if the JSON data is an array; if not, create a new array
    if (!accountsJson.is_array()) {
        accountsJson = json::array();
    }
    for (const auto& account : accountsJson) {
        if (account["username"] == username) {
            std::cout << "An account with this username already exists. Please choose another username." << std::endl;
            // promptLogin();
            return;
        }
    }
    // Create a JSON object for the new account
    json newAccount;
    newAccount["username"] = username;
    newAccount["password"] = password;
    newAccount["securityQuestion"] = securityQuestion;
    newAccount["securityAnswer"] = securityAnswer;
    vector<string> history;
    newAccount["history"] = history;
    // Add the new account to the JSON array
    accountsJson.push_back(newAccount);

    // Save the updated JSON data to the file
    std::ofstream outFile(filename);
    outFile << accountsJson.dump(4);
    outFile.close();
}
