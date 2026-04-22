#ifndef BOOK_HPP
#define BOOK_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>

class Book {
private:
    std::string isbn;
    std::string name;
    std::string author;
    std::string keyword;
    double price;
    int quantity;

public:
    Book();
    Book(const std::string& isbn);
    Book(const std::string& isbn, const std::string& name, const std::string& author,
         const std::string& keyword, double price, int qty);

    // Getters
    std::string getISBN() const { return isbn; }
    std::string getName() const { return name; }
    std::string getAuthor() const { return author; }
    std::string getKeyword() const { return keyword; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    // Setters
    void setISBN(const std::string& isbn) { this->isbn = isbn; }
    void setName(const std::string& name) { this->name = name; }
    void setAuthor(const std::string& author) { this->author = author; }
    void setKeyword(const std::string& keyword) { this->keyword = keyword; }
    void setPrice(double price) { this->price = price; }
    void setQuantity(int quantity) { this->quantity = quantity; }

    // Operations
    void addQuantity(int qty) { quantity += qty; }
    bool reduceQuantity(int qty);

    // Serialization
    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);
};

class BookManager {
private:
    std::map<std::string, Book> books;
    std::string dataFile;
    Book* selectedBook;


public:
    BookManager(const std::string& filename = "books.dat");
    ~BookManager();

    // For internal use by BookstoreSystem
    void loadBooks();
    void saveBooks() const;

    // Book operations
    bool addBook(const Book& book);
    bool deleteBook(const std::string& isbn);
    Book* findBook(const std::string& isbn);
    bool bookExists(const std::string& isbn) const;

    // Selection
    Book* getSelectedBook() { return selectedBook; }
    void selectBook(const std::string& isbn);
    void clearSelection() { selectedBook = nullptr; }

    // Search operations
    std::vector<Book*> searchAll();
    std::vector<Book*> searchByISBN(const std::string& isbn);
    std::vector<Book*> searchByName(const std::string& name);
    std::vector<Book*> searchByAuthor(const std::string& author);
    std::vector<Book*> searchByKeyword(const std::string& keyword);

    // Buy operation
    double buyBook(const std::string& isbn, int quantity);
};

#endif // BOOK_HPP