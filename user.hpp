#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <fstream>

class User {
private:
    std::string userID;
    std::string password;
    std::string username;
    int privilege;

public:
    User();
    User(const std::string& uid, const std::string& pwd, const std::string& name, int priv);

    // Getters
    std::string getUserID() const { return userID; }
    std::string getPassword() const { return password; }
    std::string getUsername() const { return username; }
    int getPrivilege() const { return privilege; }

    // Setters
    void setPassword(const std::string& pwd) { password = pwd; }
    void setUsername(const std::string& name) { username = name; }
    void setPrivilege(int priv) { privilege = priv; }

    // Serialization
    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);
};

class UserManager {
private:
    std::vector<User> users;
    std::vector<User> loginStack;
    std::string dataFile;


public:
    UserManager(const std::string& filename = "users.dat");
    ~UserManager();

    // For internal use by BookstoreSystem
    void loadUsers();
    void saveUsers() const;

    // User operations
    bool addUser(const User& user);
    bool deleteUser(const std::string& userID);
    User* findUser(const std::string& userID);
    bool userExists(const std::string& userID) const;

    // Login operations
    bool login(const std::string& userID, const std::string& password, int currentPrivilege);
    bool logout();
    User* getCurrentUser();
    int getCurrentPrivilege() const;
    bool isLoggedIn() const;

    // Initialization
    void initializeRootUser();
};

#endif // USER_HPP