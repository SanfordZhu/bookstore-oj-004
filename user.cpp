#include "user.hpp"
#include <algorithm>
#include <iostream>

User::User() : privilege(0) {}

User::User(const std::string& uid, const std::string& pwd, const std::string& name, int priv)
    : userID(uid), password(pwd), username(name), privilege(priv) {}

void User::serialize(std::ofstream& out) const {
    size_t uidLen = userID.length();
    size_t pwdLen = password.length();
    size_t nameLen = username.length();

    out.write(reinterpret_cast<const char*>(&uidLen), sizeof(uidLen));
    out.write(userID.c_str(), uidLen);
    out.write(reinterpret_cast<const char*>(&pwdLen), sizeof(pwdLen));
    out.write(password.c_str(), pwdLen);
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    out.write(username.c_str(), nameLen);
    out.write(reinterpret_cast<const char*>(&privilege), sizeof(privilege));
}

void User::deserialize(std::ifstream& in) {
    size_t uidLen, pwdLen, nameLen;

    in.read(reinterpret_cast<char*>(&uidLen), sizeof(uidLen));
    userID.resize(uidLen);
    in.read(&userID[0], uidLen);

    in.read(reinterpret_cast<char*>(&pwdLen), sizeof(pwdLen));
    password.resize(pwdLen);
    in.read(&password[0], pwdLen);

    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    username.resize(nameLen);
    in.read(&username[0], nameLen);

    in.read(reinterpret_cast<char*>(&privilege), sizeof(privilege));
}

UserManager::UserManager(const std::string& filename) : dataFile(filename) {
    loadUsers();
}

UserManager::~UserManager() {
    saveUsers();
}

void UserManager::loadUsers() {
    std::ifstream in(dataFile, std::ios::binary);
    if (!in) return;

    users.clear();
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        User user;
        user.deserialize(in);
        users.push_back(user);
    }
}

void UserManager::saveUsers() const {
    std::ofstream out(dataFile, std::ios::binary);
    if (!out) return;

    size_t count = users.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& user : users) {
        user.serialize(out);
    }
}

bool UserManager::addUser(const User& user) {
    if (userExists(user.getUserID())) {
        return false;
    }

    users.push_back(user);
    saveUsers();
    return true;
}

bool UserManager::deleteUser(const std::string& userID) {
    // Check if user is logged in
    for (const auto& loggedInUser : loginStack) {
        if (loggedInUser.getUserID() == userID) {
            return false;
        }
    }

    auto it = std::find_if(users.begin(), users.end(),
                          [&userID](const User& u) { return u.getUserID() == userID; });

    if (it == users.end()) {
        return false;
    }

    users.erase(it);
    saveUsers();
    return true;
}

User* UserManager::findUser(const std::string& userID) {
    auto it = std::find_if(users.begin(), users.end(),
                          [&userID](const User& u) { return u.getUserID() == userID; });

    if (it != users.end()) {
        return &(*it);
    }
    return nullptr;
}

bool UserManager::userExists(const std::string& userID) const {
    return std::find_if(users.begin(), users.end(),
                       [&userID](const User& u) { return u.getUserID() == userID; }) != users.end();
}

bool UserManager::login(const std::string& userID, const std::string& password, int currentPrivilege) {
    User* user = findUser(userID);
    if (!user) {
        return false;
    }

    // If current privilege is higher, password can be omitted
    if (currentPrivilege > user->getPrivilege()) {
        loginStack.push_back(*user);
        return true;
    }

    // Otherwise, password is required
    if (password.empty() || user->getPassword() != password) {
        return false;
    }

    loginStack.push_back(*user);
    return true;
}

bool UserManager::logout() {
    if (loginStack.empty()) {
        return false;
    }

    loginStack.pop_back();
    return true;
}

User* UserManager::getCurrentUser() {
    if (loginStack.empty()) {
        return nullptr;
    }
    return &loginStack.back();
}

int UserManager::getCurrentPrivilege() const {
    if (loginStack.empty()) {
        return 0;
    }
    return loginStack.back().getPrivilege();
}

bool UserManager::isLoggedIn() const {
    return !loginStack.empty();
}

void UserManager::initializeRootUser() {
    if (users.empty()) {
        User root("root", "sjtu", "root", 7);
        addUser(root);
    }
}