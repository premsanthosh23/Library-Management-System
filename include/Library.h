#ifndef LIBRARY_H
#define LIBRARY_H

#include "User.h"
#include "Book.h"
#include "Account.h"
#include <vector>
#include <map>
#include <string>

class Library {
private:
    std::string dataDir;
    std::vector<User*> users;
    std::vector<Book*> books;
    std::vector<Account*> accounts;
    
    std::map<std::string, User*> userMap;
    std::map<int, Book*> bookMap;
    std::map<std::string, Account*> accountMap;

    // Data loading and saving
    void loadData();
    void loadUsers();
    void loadBooks();
    void loadAccounts();
    void saveData();
    void saveUsers();
    void saveBooks();
    void saveAccounts();
    void initializeDefaultData();

    // Helper methods
    int generateBookId() const;
    bool fileExistsAndHasContent(const std::string& filename) const;

public:
    Library(const std::string& dir = "data");
    ~Library();

    // User management
    User* addUser(const std::string& name, const std::string& email, 
                 const std::string& password, const std::string& type, const std::string& id);
    bool removeUser(const std::string& userId);
    User* getUser(const std::string& userId) const;
    const std::vector<User*>& getAllUsers() const;

    // Book management
    Book* addBook(const std::string& title, const std::string& author,
                 const std::string& publisher, int year, const std::string& isbn);
    bool removeBook(int bookId);
    Book* getBook(int bookId) const;
    const std::vector<Book*>& getAllBooks() const;
    bool updateBook(int bookId, const std::string& title, const std::string& author,
                   const std::string& publisher, int year, const std::string& isbn);

    // Account management
    Account* createAccount(User* user);
    bool removeAccount(const std::string& accountId);
    Account* getAccount(const std::string& userId) const;
    Account* getAccountByUserId(const std::string& userId) const;

    // Library operations
    bool borrowBook(const std::string& userId, int bookId);
    bool returnBook(const std::string& userId, int bookId);
    double calculateFine(const std::string& userId, int bookId) const;
    void payFine(const std::string& userId, double amount);
    void updateFines();
    void calculateFines();

    // Display methods
    void displayAllBooks() const;
    void displayUserDetails(const std::string& userId) const;
    void displayUserBorrowHistory(const std::string& userId) const;

    // Authentication
    User* authenticateUser(const std::string& userId, const std::string& password);
    bool changePassword(const std::string& userId, const std::string& oldPassword, const std::string& newPassword);

    // Reservation operations
    bool reserveBook(const std::string& userId, int bookId);
    bool cancelReservation(const std::string& userId, int bookId);
    void checkAndUpdateReservations();
    std::vector<Book*> getReservedBooks(const std::string& userId) const;
    bool isBookReservedForUser(const std::string& userId, int bookId) const;
    void notifyUserAboutReservation(const std::string& userId, int bookId) const;
};

#endif 