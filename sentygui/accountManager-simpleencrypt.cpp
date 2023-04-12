
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

// #include "bcrypt/BCrypt.hpp"

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
                    // std::cerr << "Username already exists." << std::endl;
                    throw AccountaManagerException(AccountaManagerError::UsernameTaken);
                    return false;
                    // return;
                // if (account["username"] == username && account["password"] == password) {
                    // return true;
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
        // password = BCrypt::generateHash(passwordIn);
        // password = passwordIn;
        password = encryptPassword(passwordIn);
    }
}

std::string AccountManager::encryptPassword(const std::string& passwordIn) {
    const char key = 'k';
    // return passwordIn;

    std::string encrypted_password = passwordIn;
    for (char& c : encrypted_password) {
        c ^= key;
    }
    return encrypted_password;
    // return BCrypt::generateHash(passwordIn);
}
bool AccountManager::validateEncryptedPassword(const std::string passwordIn, const std::string & passwordStored) {
    const char key = 'k';

    std::string decrypted_password = passwordStored;
    for (char& c : decrypted_password) {
        c ^= key;
    }
    // return password;
    if (passwordIn == decrypted_password) {
        return true;
    }
    return false;
    // return BCrypt::validatePassword(passwordIn, passwordStored);
}

void AccountManager::setSecurity(const std::string& securityQIn, const std::string& securityAIn) {
    securityQuestion = securityQIn;
    securityAnswer = securityAIn;
}

void AccountManager::deleteAccount(const std::string& username) {

    if (!isLoggedIn()) {
         // std::cout << "You must be logged in to delete your account." << std::endl;
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

// bool AccountManager::matchUser(const std::string& username) {
//     json accountsJson;

//     // Read the existing JSON data from the file
//     std::ifstream inFile("accounts.json");
//     if (inFile.is_open()) {
//         inFile >> accountsJson;
//         inFile.close();
//     }
    
//     // Check if the JSON data is an array
//     if (accountsJson.is_array()) {
//         // Iterate through the array and find the matching account
//         for (const auto& account : accountsJson) {
//             if (account["username"] == username && BCrypt::validatePassword(password, account["password"])) {
//             // if (account["username"] == username && account["password"] == password) {
//                 return true;
//             }
//         }
//     }
//     return false;
// }

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
            // if (account["username"] == username && BCrypt::validatePassword(password, account["password"])) {
            // std::cout << "username = " << username << ", " << account["username"] << std::endl;
            // std::cout << "password = " << password << ", " << account["password"] << std::endl;
            if (account["username"] == username && validateEncryptedPassword(password, account["password"])) {
            // if (account["username"] == username && password == account["password"]) {
            // if (account["username"] == username && account["password"] == password) {
                loggedIn = true; // Set loggedIn to true when login is successful
                // return true;
                // return new Account(account["username"], "", account["securityQuestion"], account["securityAnswer"]);
                return std::make_shared<Account>(account["username"], "", account["securityQuestion"], account["securityAnswer"]);
            }
        }
    }
    // std::cout << "no account with these credentials exists!" << std::endl;
    // promptLogin(username, password);
    // return false;
    throw LoginException("Login failed: invalid username or password!");
}

bool AccountManager::isLoggedIn() const {
    return loggedIn;
}

// void AccountManager::logout() {
//     if (isLoggedIn()) {
//         loggedIn = false;
//         std::cout << "You have been logged out." << std::endl;
//     } 
// }

std::string AccountManager::getSecurityQuestion(std::string username) const {
    json accountsJson;
    // Read the existing JSON data from the file
    std::ifstream inFile("accounts.json");
    if (inFile.is_open()) {
        inFile >> accountsJson;
        inFile.close();
    }

    // if (accountsJson.empty()) {
    //     std::cerr << "DEBUG: No accounts to delete." << std::endl;
    //     // return "";
    //     throw AccountaManagerException(AccountaManagerException::AccountaManagerException::AccountNotExist);
    // }
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
        // std::cout << "Account not found." << std::endl;
        // continue;
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

    // json* account = nullptr;
    if (accountsJson.is_array()) {
        for (auto& acc : accountsJson) {
            auto usernameIt = acc.find("username");
            if (usernameIt != acc.end() && usernameIt->get<std::string>() == username) {
                string secQuestion = acc.find("securityQuestion")->get<std::string>();
                string secAnswer = acc.find("securityAnswer")->get<std::string>();
                return securityAnswer == secAnswer;
            }
        }
    }
    return false;
}

void AccountManager::resetPassword(std::string username, std::string newPassword) {
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

// void AccountManager::resetPassword() {
//     // json accountsJson;
//     json accountsJson;

//     // Read the existing JSON data from the file
//     std::ifstream inFile("accounts.json");
//     if (inFile.is_open()) {
//         inFile >> accountsJson;
//         inFile.close();
//     }

//     if (accountsJson.empty()) {
//         std::cout << "No accounts to delete." << std::endl;
//         return;
//     }

//     while (true) {
//         std::cout << "Enter your username: ";
//         std::cin >> username;
        
//         json* account = nullptr;
//         if (accountsJson.is_array()) {
//             // std::cout << "hi" << std::endl;
//             for (auto& acc : accountsJson) {
//                 auto usernameIt = acc.find("username");
//                 if (usernameIt != acc.end() && usernameIt->get<std::string>() == username) {
//                     account = &acc;
//                     break;
//                 }
//             }
//         }
//         if (account == nullptr) {
//             std::cout << "Account not found." << std::endl;
//             continue;
//         }

//         std::string securityAnswerIn;
//         // Prompt user to answer security question
//         // std::cout << "Security Question: " << (*account)["security_question"] << std::endl;
//         // std::cout << "Enter your answer: ";
//         // std::getline(std::cin, securityAnswerIn);
//         while (true) {
//             auto securityQuestionIt = account->find("securityQuestion");
//             auto securityAnswerIt = account->find("securityAnswer");
//             if (securityQuestionIt == account->end() || securityAnswerIt == account->end()) {
//                 std::cout << "You can only reset an account with a security question." << std::endl;
//                 return;
//             }
//             // Prompt user to answer security question
//             std::cout << "Security Question: " << securityQuestionIt->get<std::string>() << std::endl;
//             std::cout << "Enter your answer: ";
//             std::getline(std::cin, securityAnswerIn);

//             if (securityAnswerIt->get<std::string>() == securityAnswerIn) {
//                 break;
//             }
//             std::cout << "Incorrect security answer." << std::endl;
//         }
//         // Prompt user to enter new password
//         std::string newPassword, newPasswordConfirm;
//         std::cout << "Enter your new password: ";
//         // newPassword = getPasswordInput();    // TODO: uncomment after moving to TextUI
//         std::cout << "Confirm your new password: ";
//         // newPasswordConfirm = getPasswordInput(); // TODO: uncomment after moving to TextUI

//         // Check if passwords match
//         if (newPassword != newPasswordConfirm) {
//             std::cout << "Passwords do not match." << std::endl;
//             return;
//         }

//         // Hash the new password and update the user's account in the JSON data
//         // std::string hashedPassword = BCrypt::generateHash(newPassword, 12);
//         // (*account)["password"] = hashedPassword;
//         (*account)["password"] = newPassword;

//         // Save the updated JSON data to the file
//         std::ofstream outFile("accounts.json");
//         outFile << accountsJson.dump(4);
//         outFile.close();

//         std::cout << "Password reset successfully." << std::endl;
//         return;
//     }
// }

std::string AccountManager::getUsername() const {
    return username;
}

// std::string AccountManager::getPasswordInput() {
//     std::string password;

//     #ifdef _WIN32
//         char ch;
//         while ((ch = _getch()) != '\r') {
//             if (ch == '\b') {
//                 if (!password.empty()) {
//                     password.pop_back();
//                     std::cout << "\b \b";
//                 }
//             } else {
//                 password.push_back(ch);
//                 std::cout << '*';
//             }
//         }
//     #else
//         termios oldt, newt;
//         tcgetattr(STDIN_FILENO, &oldt);
//         newt = oldt;
//         newt.c_lflag &= ~ECHO;
//         tcsetattr(STDIN_FILENO, TCSANOW, &newt);

//         char ch;
//         while ((ch = getchar()) != '\n') {
//             if (ch == 127 || ch == 8) {
//                 if (!password.empty()) {
//                     password.pop_back();
//                     std::cout << "\b \b";
//                 }
//             } else {
//                 password.push_back(ch);
//                 std::cout << '*';
//             }
//         }

//         tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
//     #endif

//     std::cout << std::endl;
//     return password;
// }
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
    // Add the new account to the JSON array
    accountsJson.push_back(newAccount);

    // Save the updated JSON data to the file
    std::ofstream outFile(filename);
    outFile << accountsJson.dump(4);
    outFile.close();
}
