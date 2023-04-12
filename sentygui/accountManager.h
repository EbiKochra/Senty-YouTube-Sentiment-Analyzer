#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>
#include <unordered_map>

#include "account.h"

// RSA
// #include "cryptopp/rsa.h"
// #include "cryptopp/osrng.h"
// #include "cryptopp/base64.h"
// #include "cryptopp/files.h"
// #include "account.h"

class LoginException {
        string message;
    public:
        LoginException(string message);
        string what() const;
};

enum AccountaManagerError {AccountNotExist, InvalidSecurityAnswer, UsernameTaken, UsernameIsEmpty, PasswordIsEmpty, SecurityQuesitonIsEmpty, SecurityAnswerIsEmpty};

class AccountaManagerException {
        AccountaManagerError error;
    public:
        AccountaManagerException(AccountaManagerError error);
        string what() const;
};

class AccountManager {
public:
    AccountManager();
    // void promptCreateAccount();
    // void promptLogin();
    bool createAccount(const std::string& username, const std::string& password, const std::string& securityQuestion, const std::string& securityAnswer);
    bool createAccount(const Account *account);
    void setUsername(const std::string& usernameIn);
    void setPassword(const std::string& passwordIn);
    string encryptPassword(const std::string& passwordIn);
    bool validateEncryptedPassword(const std::string passwordIn, const std::string & passwordStored);

    void setSecurity(const std::string& securityQIn, const std::string& securityAIn);
    void deleteAccount(const std::string& username);
    // bool matchUser(const std::string& username);
    std::shared_ptr<Account> login(const std::string& username, const std::string& password);

   
    bool isLoggedIn() const;
    // void logout();
    // void resetPassword(); 

    std::string getUsername() const;
    // std::string getPasswordInput();
    std::string getPassword() const;
    void saveToFile(const std::string& filename);

    void addVideoHistory();
    // void GenerateRSAKeyPair(unsigned int keyLength, CryptoPP::RSA::PrivateKey& privateKey, CryptoPP::RSA::PublicKey& publicKey);
    // std::string RSAEncryptString(const std::string& plainText, const CryptoPP::RSA::PublicKey& publicKey);
    // std::string RSADecryptString(const std::string& cipherText, const CryptoPP::RSA::PrivateKey& privateKey);

    std::string getSecurityQuestion(std::string username) const;
    bool validateSecurityAnswer(std::string username, std::string securityAnswer) const;
    void resetPassword(std::string username, std::string newPassword);
    void addVideoHistory(string videoURL, Account &account);



private:
    std::string username;
    std::string password;
    bool loggedIn;
    std::string securityQuestion;
    std::string securityAnswer;
    // CryptoPP::RSA::PrivateKey privateKey;
    // CryptoPP::RSA::PublicKey publicKey;
    // AccountaManager
//     bool createAccount(const std::string& username, const std::string& password, const std::string& securityQuestion, const std::string& securityAnswer);
//     bool deleteAccount(const std::string& username, const std::string& password);
//     bool login(const std::string& username, const std::string& password);
//     bool forgotPassword(const std::string& username, const std::string& securityAnswer, const std::string& newPassword);

// private:
//     std::unordered_map<std::string, Account> accounts;
//     std::string accountsFile;

//     void saveAccounts();
//     void loadAccounts();
};

#endif
