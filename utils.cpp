#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>

namespace utils {

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool isValidUserID(const std::string& userID) {
    if (userID.empty() || userID.length() > 30) return false;

    for (char c : userID) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }

    return true;
}

bool isValidPassword(const std::string& password) {
    if (password.empty() || password.length() > 30) return false;

    for (char c : password) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }

    return true;
}

bool isValidUsername(const std::string& username) {
    if (username.empty() || username.length() > 30) return false;

    for (char c : username) {
        if (c < 32 || c > 126) { // ASCII printable characters except invisible
            return false;
        }
    }

    return true;
}

bool isValidPrivilege(const std::string& privilege) {
    if (privilege.length() != 1) return false;

    char c = privilege[0];
    return c == '1' || c == '3' || c == '7';
}

bool isValidISBN(const std::string& isbn) {
    if (isbn.empty() || isbn.length() > 20) return false;

    for (char c : isbn) {
        if (c < 32 || c > 126) { // ASCII printable characters
            return false;
        }
    }

    return true;
}

bool isValidBookName(const std::string& name) {
    if (name.length() > 60) return false;

    for (char c : name) {
        if (c < 32 || c > 126 || c == '"') { // ASCII printable except double quotes
            return false;
        }
    }

    return true;
}

bool isValidAuthor(const std::string& author) {
    if (author.length() > 60) return false;

    for (char c : author) {
        if (c < 32 || c > 126 || c == '"') { // ASCII printable except double quotes
            return false;
        }
    }

    return true;
}

bool isValidKeyword(const std::string& keyword) {
    if (keyword.length() > 60) return false;

    for (char c : keyword) {
        if (c < 32 || c > 126 || c == '"') { // ASCII printable except double quotes
            return false;
        }
    }

    return true;
}

bool isValidQuantity(const std::string& quantity) {
    if (quantity.empty() || quantity.length() > 10) return false;

    for (char c : quantity) {
        if (!std::isdigit(c)) {
            return false;
        }
    }

    // Check if value exceeds 2,147,483,647
    try {
        long long val = std::stoll(quantity);
        return val > 0 && val <= 2147483647;
    } catch (...) {
        return false;
    }
}

bool isValidPrice(const std::string& price) {
    if (price.empty() || price.length() > 13) return false;

    bool hasDot = false;
    int dotPos = -1;

    for (size_t i = 0; i < price.length(); ++i) {
        char c = price[i];
        if (c == '.') {
            if (hasDot) return false; // Multiple dots
            hasDot = true;
            dotPos = i;
        } else if (!std::isdigit(c)) {
            return false;
        }
    }

    // Check decimal places (should be exactly 2)
    if (hasDot) {
        int decimalPlaces = price.length() - dotPos - 1;
        if (decimalPlaces != 2) return false;
    }

    return true;
}

bool fileExists(const std::string& filename) {
    return access(filename.c_str(), F_OK) == 0;
}

void ensureDirectoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        // Directory doesn't exist, create it
        mkdir(path.c_str(), 0755);
    }
}

} // namespace utils