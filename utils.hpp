#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <sstream>

namespace utils {
    // String utilities
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
    bool isValidUserID(const std::string& userID);
    bool isValidPassword(const std::string& password);
    bool isValidUsername(const std::string& username);
    bool isValidPrivilege(const std::string& privilege);
    bool isValidISBN(const std::string& isbn);
    bool isValidBookName(const std::string& name);
    bool isValidAuthor(const std::string& author);
    bool isValidKeyword(const std::string& keyword);
    bool isValidQuantity(const std::string& quantity);
    bool isValidPrice(const std::string& price);

    // File utilities
    bool fileExists(const std::string& filename);
    void ensureDirectoryExists(const std::string& path);
}

#endif // UTILS_HPP