#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "user.hpp"
#include "book.hpp"
#include "log.hpp"
#include "utils.hpp"

class BookstoreSystem {
private:
    UserManager userManager;
    BookManager bookManager;
    LogManager logManager;

    void processCommand(const std::vector<std::string>& tokens);
    std::vector<std::string> parseCommand(const std::string& command);

    // Command handlers
    void handleQuit();
    void handleExit();
    void handleSu(const std::vector<std::string>& tokens);
    void handleLogout();
    void handleRegister(const std::vector<std::string>& tokens);
    void handlePasswd(const std::vector<std::string>& tokens);
    void handleUseradd(const std::vector<std::string>& tokens);
    void handleDelete(const std::vector<std::string>& tokens);
    void handleShow(const std::vector<std::string>& tokens);
    void handleBuy(const std::vector<std::string>& tokens);
    void handleSelect(const std::vector<std::string>& tokens);
    void handleModify(const std::vector<std::string>& tokens);
    void handleImport(const std::vector<std::string>& tokens);
    void handleShowFinance(const std::vector<std::string>& tokens);
    void handleLog();
    void handleReportFinance();
    void handleReportEmployee();

public:
    BookstoreSystem();
    void run();
};

BookstoreSystem::BookstoreSystem() {
    // Initialize root user if first run
    userManager.initializeRootUser();
}

std::vector<std::string> BookstoreSystem::parseCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;

    for (size_t i = 0; i < command.length(); ++i) {
        char c = command[i];

        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

void BookstoreSystem::processCommand(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return;

    const std::string& cmd = tokens[0];

    if (cmd == "quit") {
        handleQuit();
    } else if (cmd == "exit") {
        handleExit();
    } else if (cmd == "su") {
        handleSu(tokens);
    } else if (cmd == "logout") {
        handleLogout();
    } else if (cmd == "register") {
        handleRegister(tokens);
    } else if (cmd == "passwd") {
        handlePasswd(tokens);
    } else if (cmd == "useradd") {
        handleUseradd(tokens);
    } else if (cmd == "delete") {
        handleDelete(tokens);
    } else if (cmd == "show" && tokens.size() > 1 && tokens[1] == "finance") {
        handleShowFinance(tokens);
    } else if (cmd == "show") {
        handleShow(tokens);
    } else if (cmd == "buy") {
        handleBuy(tokens);
    } else if (cmd == "select") {
        handleSelect(tokens);
    } else if (cmd == "modify") {
        handleModify(tokens);
    } else if (cmd == "import") {
        handleImport(tokens);
    } else if (cmd == "log") {
        handleLog();
    } else if (cmd == "report" && tokens.size() > 1 && tokens[1] == "finance") {
        handleReportFinance();
    } else if (cmd == "report" && tokens.size() > 1 && tokens[1] == "employee") {
        handleReportEmployee();
    } else {
        std::cout << "Invalid" << std::endl;
    }
}

void BookstoreSystem::handleQuit() {
    exit(0);
}

void BookstoreSystem::handleExit() {
    exit(0);
}

void BookstoreSystem::handleSu(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string userID = tokens[1];
    std::string password;

    if (tokens.size() >= 3) {
        password = tokens[2];
    }

    int currentPrivilege = userManager.getCurrentPrivilege();

    if (!userManager.login(userID, password, currentPrivilege)) {
        std::cout << "Invalid" << std::endl;
    }
}

void BookstoreSystem::handleLogout() {
    if (!userManager.logout()) {
        std::cout << "Invalid" << std::endl;
    }
}

void BookstoreSystem::handleRegister(const std::vector<std::string>& tokens) {
    if (tokens.size() != 4) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string userID = tokens[1];
    std::string password = tokens[2];
    std::string username = tokens[3];

    if (!utils::isValidUserID(userID) || !utils::isValidPassword(password) ||
        !utils::isValidUsername(username)) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    User newUser(userID, password, username, 1);

    if (!userManager.addUser(newUser)) {
        std::cout << "Invalid" << std::endl;
    } else {
        logManager.addOperationLog(userID, "register", "Registered new account");
    }
}

void BookstoreSystem::handlePasswd(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string userID = tokens[1];
    std::string newPassword;
    std::string currentPassword;

    int currentPrivilege = userManager.getCurrentPrivilege();
    User* user = userManager.findUser(userID);

    if (!user) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    if (tokens.size() == 3) {
        // Admin changing password without current password
        if (currentPrivilege != 7) {
            std::cout << "Invalid" << std::endl;
            return;
        }
        newPassword = tokens[2];
    } else {
        // User changing their own password
        currentPassword = tokens[2];
        newPassword = tokens[3];

        User* currentUser = userManager.getCurrentUser();
        if (!currentUser || currentUser->getUserID() != userID ||
            currentUser->getPassword() != currentPassword) {
            std::cout << "Invalid" << std::endl;
            return;
        }
    }

    if (!utils::isValidPassword(newPassword)) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    user->setPassword(newPassword);
    userManager.saveUsers();
    logManager.addOperationLog(userID, "passwd", "Changed password");
}

void BookstoreSystem::handleUseradd(const std::vector<std::string>& tokens) {
    if (tokens.size() != 5) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege < 3) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string userID = tokens[1];
    std::string password = tokens[2];
    std::string privilegeStr = tokens[3];
    std::string username = tokens[4];

    if (!utils::isValidUserID(userID) || !utils::isValidPassword(password) ||
        !utils::isValidPrivilege(privilegeStr) || !utils::isValidUsername(username)) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int privilege = std::stoi(privilegeStr);
    if (privilege >= currentPrivilege) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    User newUser(userID, password, username, privilege);

    if (!userManager.addUser(newUser)) {
        std::cout << "Invalid" << std::endl;
    } else {
        User* currentUser = userManager.getCurrentUser();
        logManager.addOperationLog(currentUser->getUserID(), "useradd",
                                  "Created user " + userID);
    }
}

void BookstoreSystem::handleDelete(const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege != 7) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string userID = tokens[1];

    if (!userManager.deleteUser(userID)) {
        std::cout << "Invalid" << std::endl;
    } else {
        User* currentUser = userManager.getCurrentUser();
        logManager.addOperationLog(currentUser->getUserID(), "delete",
                                  "Deleted user " + userID);
    }
}

void BookstoreSystem::handleShow(const std::vector<std::string>& tokens) {
    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege < 1) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::vector<Book*> results;

    if (tokens.size() == 1) {
        // Show all books
        results = bookManager.searchAll();
    } else if (tokens.size() == 2) {
        std::string param = tokens[1];

        if (param.find("-ISBN=") == 0) {
            std::string isbn = param.substr(6);
            if (!utils::isValidISBN(isbn)) {
                std::cout << "Invalid" << std::endl;
                return;
            }
            results = bookManager.searchByISBN(isbn);
        } else if (param.find("-name=\"") == 0 && param.back() == '"') {
            std::string name = param.substr(7, param.length() - 8);
            if (!utils::isValidBookName(name) || name.empty()) {
                std::cout << "Invalid" << std::endl;
                return;
            }
            results = bookManager.searchByName(name);
        } else if (param.find("-author=\"") == 0 && param.back() == '"') {
            std::string author = param.substr(10, param.length() - 11);
            if (!utils::isValidAuthor(author) || author.empty()) {
                std::cout << "Invalid" << std::endl;
                return;
            }
            results = bookManager.searchByAuthor(author);
        } else if (param.find("-keyword=\"") == 0 && param.back() == '"') {
            std::string keyword = param.substr(11, param.length() - 12);
            if (!utils::isValidKeyword(keyword) || keyword.empty() || keyword.find('|') != std::string::npos) {
                std::cout << "Invalid" << std::endl;
                return;
            }
            results = bookManager.searchByKeyword(keyword);
        } else {
            std::cout << "Invalid" << std::endl;
            return;
        }
    } else {
        std::cout << "Invalid" << std::endl;
        return;
    }

    // Sort by ISBN
    std::sort(results.begin(), results.end(), [](Book* a, Book* b) {
        return a->getISBN() < b->getISBN();
    });

    // Output results
    if (results.empty()) {
        std::cout << std::endl;
    } else {
        for (Book* book : results) {
            std::cout << book->getISBN() << "\t" << book->getName() << "\t"
                      << book->getAuthor() << "\t" << book->getKeyword() << "\t"
                      << std::fixed << std::setprecision(2) << book->getPrice() << "\t"
                      << book->getQuantity() << std::endl;
        }
    }
}

void BookstoreSystem::handleBuy(const std::vector<std::string>& tokens) {
    if (tokens.size() != 3) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege < 1) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string isbn = tokens[1];
    std::string quantityStr = tokens[2];

    if (!utils::isValidISBN(isbn) || !utils::isValidQuantity(quantityStr)) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int quantity = std::stoi(quantityStr);
    if (quantity <= 0) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    try {
        double total = bookManager.buyBook(isbn, quantity);
        std::cout << std::fixed << std::setprecision(2) << total << std::endl;

        User* currentUser = userManager.getCurrentUser();
        logManager.addFinanceRecord(total, 0.0, currentUser->getUserID(), "buy");
        logManager.addOperationLog(currentUser->getUserID(), "buy",
                                  "Bought " + std::to_string(quantity) + " of " + isbn);
    } catch (...) {
        std::cout << "Invalid" << std::endl;
    }
}

void BookstoreSystem::handleSelect(const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege < 3) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string isbn = tokens[1];
    if (!utils::isValidISBN(isbn)) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    bookManager.selectBook(isbn);
}

void BookstoreSystem::handleModify(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege < 3) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    Book* selectedBook = bookManager.getSelectedBook();
    if (!selectedBook) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string originalISBN = selectedBook->getISBN();
    bool modified = false;

    for (size_t i = 1; i < tokens.size(); ++i) {
        const std::string& param = tokens[i];

        if (param.find("-ISBN=") == 0) {
            std::string newISBN = param.substr(6);
            if (!utils::isValidISBN(newISBN) || newISBN == originalISBN ||
                bookManager.bookExists(newISBN)) {
                std::cout << "Invalid" << std::endl;
                return;
            }
            selectedBook->setISBN(newISBN);
            modified = true;
        } else if (param.find("-name=\"") == 0 && param.back() == '"') {
            std::string name = param.substr(7, param.length() - 8);
            if (!utils::isValidBookName(name) || name.empty()) {
                std::cout << "Invalid" << std::endl;
                return;
            }
            selectedBook->setName(name);
            modified = true;
        } else if (param.find("-author=\"") == 0 && param.back() == '"') {
            std::string author = param.substr(10, param.length() - 11);
            if (!utils::isValidAuthor(author) || author.empty()) {
                std::cout << "Invalid" << std::endl;
                return;
            }
            selectedBook->setAuthor(author);
            modified = true;
        } else if (param.find("-keyword=\"") == 0 && param.back() == '"') {
            std::string keyword = param.substr(11, param.length() - 12);
            if (!utils::isValidKeyword(keyword) || keyword.empty()) {
                std::cout << "Invalid" << std::endl;
                return;
            }

            // Check for duplicate keywords
            std::vector<std::string> keywords = utils::split(keyword, '|');
            std::sort(keywords.begin(), keywords.end());
            for (size_t j = 1; j < keywords.size(); ++j) {
                if (keywords[j] == keywords[j-1]) {
                    std::cout << "Invalid" << std::endl;
                    return;
                }
            }

            selectedBook->setKeyword(keyword);
            modified = true;
        } else if (param.find("-price=") == 0) {
            std::string priceStr = param.substr(7);
            if (!utils::isValidPrice(priceStr)) {
                std::cout << "Invalid" << std::endl;
                return;
            }
            double price = std::stod(priceStr);
            selectedBook->setPrice(price);
            modified = true;
        } else {
            std::cout << "Invalid" << std::endl;
            return;
        }
    }

    if (!modified) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    bookManager.saveBooks();
    User* currentUser = userManager.getCurrentUser();
    logManager.addOperationLog(currentUser->getUserID(), "modify",
                              "Modified book " + originalISBN);
}

void BookstoreSystem::handleImport(const std::vector<std::string>& tokens) {
    if (tokens.size() != 3) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege < 3) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    Book* selectedBook = bookManager.getSelectedBook();
    if (!selectedBook) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::string quantityStr = tokens[1];
    std::string totalCostStr = tokens[2];

    if (!utils::isValidQuantity(quantityStr) || !utils::isValidPrice(totalCostStr)) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int quantity = std::stoi(quantityStr);
    double totalCost = std::stod(totalCostStr);

    if (quantity <= 0 || totalCost <= 0) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    selectedBook->addQuantity(quantity);
    bookManager.saveBooks();

    User* currentUser = userManager.getCurrentUser();
    logManager.addFinanceRecord(0.0, totalCost, currentUser->getUserID(), "import");
    logManager.addOperationLog(currentUser->getUserID(), "import",
                              "Imported " + std::to_string(quantity) + " of " +
                              selectedBook->getISBN() + " for " + totalCostStr);
}

void BookstoreSystem::handleShowFinance(const std::vector<std::string>& tokens) {
    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege != 7) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    int count = -1;
    if (tokens.size() > 2) {
        std::string countStr = tokens[2];
        if (!utils::isValidQuantity(countStr)) {
            std::cout << "Invalid" << std::endl;
            return;
        }
        count = std::stoi(countStr);
        if (count < 0) {
            std::cout << "Invalid" << std::endl;
            return;
        }
    }

    auto summary = logManager.getFinanceSummary(count);
    std::cout << "+ " << std::fixed << std::setprecision(2) << summary.first
              << " - " << summary.second << std::endl;
}

void BookstoreSystem::handleLog() {
    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege != 7) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::cout << logManager.generateFullLog();
}

void BookstoreSystem::handleReportFinance() {
    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege != 7) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::cout << logManager.generateFinanceReport();
}

void BookstoreSystem::handleReportEmployee() {
    int currentPrivilege = userManager.getCurrentPrivilege();
    if (currentPrivilege != 7) {
        std::cout << "Invalid" << std::endl;
        return;
    }

    std::cout << logManager.generateEmployeeReport();
}

void BookstoreSystem::run() {
    std::string line;
    while (std::getline(std::cin, line)) {
        // Trim the line
        line = utils::trim(line);

        if (line.empty()) continue;

        std::vector<std::string> tokens = parseCommand(line);

        if (!tokens.empty()) {
            processCommand(tokens);
        }
    }
}

int main() {
    BookstoreSystem system;
    system.run();
    return 0;
}