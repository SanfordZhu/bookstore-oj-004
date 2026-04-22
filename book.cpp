#include "book.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>

Book::Book() : price(0.0), quantity(0) {}

Book::Book(const std::string& isbn) : isbn(isbn), price(0.0), quantity(0) {}

Book::Book(const std::string& isbn, const std::string& name, const std::string& author,
           const std::string& keyword, double price, int qty)
    : isbn(isbn), name(name), author(author), keyword(keyword), price(price), quantity(qty) {}

bool Book::reduceQuantity(int qty) {
    if (quantity < qty) {
        return false;
    }
    quantity -= qty;
    return true;
}

void Book::serialize(std::ofstream& out) const {
    size_t isbnLen = isbn.length();
    size_t nameLen = name.length();
    size_t authorLen = author.length();
    size_t keywordLen = keyword.length();

    out.write(reinterpret_cast<const char*>(&isbnLen), sizeof(isbnLen));
    out.write(isbn.c_str(), isbnLen);
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    out.write(name.c_str(), nameLen);
    out.write(reinterpret_cast<const char*>(&authorLen), sizeof(authorLen));
    out.write(author.c_str(), authorLen);
    out.write(reinterpret_cast<const char*>(&keywordLen), sizeof(keywordLen));
    out.write(keyword.c_str(), keywordLen);
    out.write(reinterpret_cast<const char*>(&price), sizeof(price));
    out.write(reinterpret_cast<const char*>(&quantity), sizeof(quantity));
}

void Book::deserialize(std::ifstream& in) {
    size_t isbnLen, nameLen, authorLen, keywordLen;

    in.read(reinterpret_cast<char*>(&isbnLen), sizeof(isbnLen));
    isbn.resize(isbnLen);
    in.read(&isbn[0], isbnLen);

    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    name.resize(nameLen);
    in.read(&name[0], nameLen);

    in.read(reinterpret_cast<char*>(&authorLen), sizeof(authorLen));
    author.resize(authorLen);
    in.read(&author[0], authorLen);

    in.read(reinterpret_cast<char*>(&keywordLen), sizeof(keywordLen));
    keyword.resize(keywordLen);
    in.read(&keyword[0], keywordLen);

    in.read(reinterpret_cast<char*>(&price), sizeof(price));
    in.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));
}

BookManager::BookManager(const std::string& filename) : dataFile(filename), selectedBook(nullptr) {
    loadBooks();
}

BookManager::~BookManager() {
    saveBooks();
}

void BookManager::loadBooks() {
    std::ifstream in(dataFile, std::ios::binary);
    if (!in) return;

    books.clear();
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        Book book;
        book.deserialize(in);
        books[book.getISBN()] = book;
    }
}

void BookManager::saveBooks() const {
    std::ofstream out(dataFile, std::ios::binary);
    if (!out) return;

    size_t count = books.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& pair : books) {
        pair.second.serialize(out);
    }
}

bool BookManager::addBook(const Book& book) {
    if (bookExists(book.getISBN())) {
        return false;
    }

    books[book.getISBN()] = book;
    saveBooks();
    return true;
}

bool BookManager::deleteBook(const std::string& isbn) {
    auto it = books.find(isbn);
    if (it == books.end()) {
        return false;
    }

    books.erase(it);
    if (selectedBook && selectedBook->getISBN() == isbn) {
        selectedBook = nullptr;
    }
    saveBooks();
    return true;
}

Book* BookManager::findBook(const std::string& isbn) {
    auto it = books.find(isbn);
    if (it != books.end()) {
        return &it->second;
    }
    return nullptr;
}

bool BookManager::bookExists(const std::string& isbn) const {
    return books.find(isbn) != books.end();
}

void BookManager::selectBook(const std::string& isbn) {
    Book* book = findBook(isbn);
    if (!book) {
        // Create new book with just ISBN
        Book newBook(isbn);
        books[isbn] = newBook;
        selectedBook = &books[isbn];
        saveBooks();
    } else {
        selectedBook = book;
    }
}

std::vector<Book*> BookManager::searchAll() {
    std::vector<Book*> results;
    for (auto& pair : books) {
        results.push_back(&pair.second);
    }
    return results;
}

std::vector<Book*> BookManager::searchByISBN(const std::string& isbn) {
    std::vector<Book*> results;
    auto it = books.find(isbn);
    if (it != books.end()) {
        results.push_back(&it->second);
    }
    return results;
}

std::vector<Book*> BookManager::searchByName(const std::string& name) {
    std::vector<Book*> results;
    for (auto& pair : books) {
        if (pair.second.getName().find(name) != std::string::npos) {
            results.push_back(&pair.second);
        }
    }
    return results;
}

std::vector<Book*> BookManager::searchByAuthor(const std::string& author) {
    std::vector<Book*> results;
    for (auto& pair : books) {
        if (pair.second.getAuthor().find(author) != std::string::npos) {
            results.push_back(&pair.second);
        }
    }
    return results;
}

std::vector<Book*> BookManager::searchByKeyword(const std::string& keyword) {
    std::vector<Book*> results;
    for (auto& pair : books) {
        const std::string& bookKeyword = pair.second.getKeyword();
        std::vector<std::string> keywords = utils::split(bookKeyword, '|');

        for (const std::string& kw : keywords) {
            if (kw == keyword) {
                results.push_back(&pair.second);
                break;
            }
        }
    }
    return results;
}

double BookManager::buyBook(const std::string& isbn, int quantity) {
    auto it = books.find(isbn);
    if (it == books.end()) {
        throw std::runtime_error("Book not found");
    }

    Book& book = it->second;
    if (!book.reduceQuantity(quantity)) {
        throw std::runtime_error("Insufficient quantity");
    }

    double total = book.getPrice() * quantity;
    saveBooks();
    return total;
}