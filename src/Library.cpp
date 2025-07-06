#include "Library.h"
#include "Student.h"
#include "Faculty.h"
#include "Librarian.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h> // for getcwd
#include <chrono>   // for modern time handling
#include <filesystem>

// Helper function for getting current timestamp as string
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

Library::Library(const std::string& dir) : dataDir(dir) {
    // Convert relative path to absolute path if needed
    if (dataDir == "data") {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            dataDir = std::string(cwd) + "/" + dataDir;
        }
    }
    
    // Create data directory if it doesn't exist
    std::string mkdirCmd = "mkdir -p " + dataDir;
    system(mkdirCmd.c_str());
    
    // Check if any of the data files exist
    bool hasExistingData = fileExistsAndHasContent(dataDir + "/users.txt") ||
                          fileExistsAndHasContent(dataDir + "/books.txt") ||
                          fileExistsAndHasContent(dataDir + "/accounts.txt");

    if (hasExistingData) {
        loadData();  // Load data only once
        std::cout << "Successfully loaded existing database with: " << users.size() << " users, " 
                  << books.size() << " books, and "
                  << accounts.size() << " accounts." << std::endl;
    } else {
        std::cout << "No existing data found. Creating new database with default data..." << std::endl;
        initializeDefaultData();
    }
}

Library::~Library() {
    saveData();
    // Clean up memory
    for (auto& user : users) delete user;
    for (auto& book : books) delete book;
    for (auto& account : accounts) delete account;
}

void Library::loadData() {
    loadUsers();
    loadBooks();
    loadAccounts();
    
    if (users.empty()) {
        std::cout << "No existing users found. Initializing with default data...\n";
        initializeDefaultData();
    }
}

void Library::loadUsers() {
    std::ifstream file(dataDir + "/users.txt");
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id, name, email, password, role;
        
        if (std::getline(iss, id, '|') && 
            std::getline(iss, name, '|') && 
            std::getline(iss, email, '|') && 
            std::getline(iss, password, '|') && 
            std::getline(iss, role)) {
            
            User* user = nullptr;
            if (role == "Student") {
                user = new Student(name, email, id);
            } else if (role == "Faculty") {
                user = new Faculty(name, email, id);
            } else if (role == "Librarian") {
                user = new Librarian(name, email, id);
            }
            
            if (user) {
                user->setPassword(password);
                users.push_back(user);
                userMap[id] = user;
            }
        }
    }
}

void Library::loadBooks() {
    std::ifstream file(dataDir + "/books.txt");
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id, title, author, publisher, yearStr, isbn, status, borrowedBy;
        
        if (std::getline(iss, id, '|') && 
            std::getline(iss, title, '|') && 
            std::getline(iss, author, '|') && 
            std::getline(iss, publisher, '|') && 
            std::getline(iss, yearStr, '|') && 
            std::getline(iss, isbn, '|') && 
            std::getline(iss, status, '|') && 
            std::getline(iss, borrowedBy)) {
            
            try {
                int bookId = std::stoi(id);
                int year = std::stoi(yearStr);
                Book* book = new Book(bookId, title, author, publisher, year, isbn);
                book->setStatus(status);
                if (!borrowedBy.empty() && borrowedBy != "None") {
                    book->setBorrowedBy(borrowedBy);
                }
                books.push_back(book);
                bookMap[bookId] = book;
            } catch (const std::exception& e) {
                std::cerr << "Error loading book: " << e.what() << std::endl;
            }
        }
    }
}

void Library::loadAccounts() {
    std::ifstream file(dataDir + "/accounts.txt");
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string userId, totalFineStr, finePaidStr;
        
        if (std::getline(iss, userId, '|') && 
            std::getline(iss, totalFineStr, '|') && 
            std::getline(iss, finePaidStr)) {
            
            try {
                Account* account = new Account(userId);
                account->updateFine(std::stod(totalFineStr));
                account->setFinePaid(finePaidStr == "1");
                
                // Read borrowed books on next line
                if (std::getline(file, line)) {
                    std::istringstream bookIss(line);
                    std::string bookId;
                    while (std::getline(bookIss, bookId, '|')) {
                        if (!bookId.empty()) {
                            // Get current time for borrow date
                            time_t now = time(0);
                            char* dt = ctime(&now);
                            std::string borrowDate(dt);
                            borrowDate = borrowDate.substr(0, borrowDate.length() - 1);

                            // Calculate due date
                            now += 15 * 10; // 15 days (10 seconds = 1 day for testing)
                            dt = ctime(&now);
                            std::string dueDate(dt);
                            dueDate = dueDate.substr(0, dueDate.length() - 1);

                            account->addBorrowedBook(std::stoi(bookId), borrowDate, dueDate);
                        }
                    }
                }
                
                accounts.push_back(account);
                accountMap[userId] = account;
            } catch (const std::exception& e) {
                std::cerr << "Error loading account: " << e.what() << std::endl;
            }
        }
    }
}

void Library::saveData() {
    saveUsers();
    saveBooks();
    saveAccounts();
}

void Library::saveUsers() {
    std::ofstream file(dataDir + "/users.txt");
    for (const User* user : users) {
        file << user->getUserId() << "|"
             << user->getName() << "|"
             << user->getEmail() << "|"
             << user->getPassword() << "|"
             << user->getRole() << "\n";
    }
}

void Library::saveBooks() {
    std::ofstream file(dataDir + "/books.txt");
    for (const Book* book : books) {
        file << book->getBookId() << "|"
             << book->getTitle() << "|"
             << book->getAuthor() << "|"
             << book->getPublisher() << "|"
             << book->getYear() << "|"
             << book->getIsbn() << "|"
             << book->getStatus() << "|"
             << (book->getBorrowedBy().empty() ? "None" : book->getBorrowedBy()) << "\n";
    }
}

void Library::saveAccounts() {
    std::ofstream file(dataDir + "/accounts.txt");
    for (const Account* account : accounts) {
        // Save account details
        file << account->getUserId() << "|"
             << account->getTotalFine() << "|"
             << (account->isFinePaid() ? "1" : "0") << "\n";
        
        // Save borrowed books on next line
        const auto& borrowedBooks = account->getCurrentlyBorrowedBooks();
        bool first = true;
        for (const auto& bookId : borrowedBooks) {
            if (!first) file << "|";
            file << bookId;
            first = false;
        }
        file << "\n";
    }
}

void Library::initializeDefaultData() {
    // Add default users
    addUser("Admin", "admin@library.com", "admin123", "Librarian", "L001");
    addUser("John Doe", "john@example.com", "student123", "Student", "S001");
    addUser("Jane Smith", "jane@example.com", "student123", "Student", "S002");
    addUser("Dr. Brown", "brown@example.com", "faculty123", "Faculty", "F001");

    // Add sample books
    addBook("Introduction to C++", "Bjarne Stroustrup", "Addison-Wesley", 2013, "978-0321563842");
    addBook("Data Structures", "Robert Sedgewick", "Pearson", 2011, "978-0321573513");
    addBook("Operating Systems", "Abraham Silberschatz", "Wiley", 2012, "978-1118063330");

    // Create accounts for users
    for (User* user : users) {
        createAccount(user);
    }

    // Save all data
    saveData();
}

User* Library::addUser(const std::string& name, const std::string& email, 
                      const std::string& password, const std::string& type, const std::string& id) {
    User* user = nullptr;
    
    if (type == "Student") {
        user = new Student(name, email, id);
    } else if (type == "Faculty") {
        user = new Faculty(name, email, id);
    } else if (type == "Librarian") {
        user = new Librarian(name, email, id);
    }

    if (user) {
        user->setPassword(password);
        users.push_back(user);
        userMap[id] = user;
        saveUsers();
    }
    return user;
}

Book* Library::addBook(const std::string& title, const std::string& author,
                      const std::string& publisher, int year, const std::string& isbn) {
    int bookId = generateBookId();
    Book* book = new Book(bookId, title, author, publisher, year, isbn);
    books.push_back(book);
    bookMap[bookId] = book;
    saveBooks();
    return book;
}

Account* Library::createAccount(User* user) {
    if (!user) return nullptr;
    
    Account* account = new Account(user->getUserId());
    accounts.push_back(account);
    accountMap[user->getUserId()] = account;
    saveAccounts();
    return account;
}

bool Library::borrowBook(const std::string& userId, int bookId) {
    User* user = getUser(userId);
    Book* book = getBook(bookId);
    Account* account = getAccount(userId);

    if (!user || !book || !account) return false;
    if (!book->isAvailable()) return false;

    // Check if the book is reserved for someone else
    std::string nextReservation = book->getNextReservation();
    if (!nextReservation.empty() && nextReservation != userId) {
        std::cout << "This book is reserved for another user.\n";
        return false;
    }

    // If the book was reserved for this user and the reservation expired, remove it
    if (book->isReservationExpired() && book->getNextReservation() == userId) {
        book->removeExpiredReservation();
    }

    // For faculty members, check if they have any currently borrowed books
    if (user->getRole() == "Faculty" && !account->getCurrentlyBorrowedBooks().empty()) {
        return false;  // Faculty cannot borrow if they have any unreturned books
    }

    // For other users, check if they can borrow
    if (!user->canBorrowBook()) return false;

    // Get current time for borrow date
    time_t now = time(0);
    char* dt = ctime(&now);
    std::string borrowDate(dt);
    borrowDate = borrowDate.substr(0, borrowDate.length() - 1);

    // Calculate due date based on user type (10 seconds = 1 day for testing)
    now += user->getMaxBorrowPeriod() * 10; // 10 seconds per day for testing
    dt = ctime(&now);
    std::string dueDate(dt);
    dueDate = dueDate.substr(0, dueDate.length() - 1);

    book->markAsBorrowed(userId);
    account->addBorrowedBook(bookId, borrowDate, dueDate);
    
    // If this user had reserved the book, remove the reservation
    if (book->hasReservation(userId)) {
        book->cancelReservation(userId);
    }
    
    saveBooks();
    saveAccounts();
    return true;
}

bool Library::returnBook(const std::string& userId, int bookId) {
    Book* book = getBook(bookId);
    Account* account = getAccount(userId);

    if (!book || !account) return false;
    if (book->getBorrowedBy() != userId) return false;

    book->markAsReturned();
    account->removeBorrowedBook(bookId);
    
    // Check if there are any reservations
    std::string nextUser = book->getNextReservation();
    if (!nextUser.empty()) {
        notifyUserAboutReservation(nextUser, bookId);
    }
    
    saveBooks();
    saveAccounts();
    return true;
}

User* Library::authenticateUser(const std::string& userId, const std::string& password) {
    User* user = getUser(userId);
    if (user && user->getPassword() == password) {
        return user;
    }
    return nullptr;
}

// Helper methods
User* Library::getUser(const std::string& userId) const {
    auto it = userMap.find(userId);
    return it != userMap.end() ? it->second : nullptr;
}

Book* Library::getBook(int bookId) const {
    auto it = bookMap.find(bookId);
    return it != bookMap.end() ? it->second : nullptr;
}

Account* Library::getAccount(const std::string& userId) const {
    auto it = accountMap.find(userId);
    return it != accountMap.end() ? it->second : nullptr;
}

int Library::generateBookId() const {
    int maxId = 0;
    for (const Book* book : books) {
        maxId = std::max(maxId, book->getBookId());
    }
    return maxId + 1;
}

// User management
bool Library::removeUser(const std::string& userId) {
    auto userIt = std::find_if(users.begin(), users.end(),
        [userId](const User* u) { return u->getUserId() == userId; });
    
    if (userIt != users.end()) {
        // Check if user has borrowed books or unpaid fines
        Account* account = getAccount(userId);
        if (account) {
            if (!account->getCurrentlyBorrowedBooks().empty()) {
                std::cout << "Error: Cannot remove user. User has borrowed books that need to be returned first." << std::endl;
                return false;
            }
            if (account->getTotalFine() > 0) {
                std::cout << "Error: Cannot remove user. User has unpaid fines of Rs. " 
                         << account->getTotalFine() << " that need to be cleared first." << std::endl;
                return false;
            }
        }

        delete *userIt;
        users.erase(userIt);
        userMap.erase(userId);
        
        auto accountIt = std::find_if(accounts.begin(), accounts.end(),
            [userId](const Account* a) { return a->getUserId() == userId; });
        
        if (accountIt != accounts.end()) {
            delete *accountIt;
            accounts.erase(accountIt);
        }
        saveUsers();
        saveAccounts();
        return true;
    }
    return false;
}

const std::vector<User*>& Library::getAllUsers() const {
    return users;
}

// Book management
bool Library::removeBook(int bookId) {
    auto bookIt = std::find_if(books.begin(), books.end(),
        [bookId](const Book* b) { return b->getBookId() == bookId; });
    
    if (bookIt != books.end()) {
        Book* book = *bookIt;
        // Check if the book is currently borrowed
        if (!book->isAvailable()) {
            std::cout << "Error: Cannot remove book. The book is currently borrowed by user: " 
                     << book->getBorrowedBy() << ". Book must be returned first." << std::endl;
            return false;
        }

        delete book;
        books.erase(bookIt);
        bookMap.erase(bookId);
        saveBooks();
        return true;
    }
    return false;
}

const std::vector<Book*>& Library::getAllBooks() const {
    return books;
}

// Account management
bool Library::removeAccount(const std::string& accountId) {
    auto accountIt = std::find_if(accounts.begin(), accounts.end(),
        [accountId](const Account* a) { return a->getUserId() == accountId; });
    
    if (accountIt != accounts.end()) {
        delete *accountIt;
        accounts.erase(accountIt);
        accountMap.erase(accountId);
        saveAccounts();
        return true;
    }
    return false;
}

Account* Library::getAccountByUserId(const std::string& userId) const {
    for (Account* account : accounts) {
        if (account->getUserId() == userId) {
            return account;
        }
    }
    return nullptr;
}

// Library operations
void Library::payFine(const std::string& userId, double amount) {
    Account* account = getAccountByUserId(userId);
    if (account) {
        account->updateFine(-amount);
        if (account->getTotalFine() <= 0) {
            account->setFinePaid(true);
        }
        saveAccounts();
    }
}

void Library::updateFines() {
    calculateFines();
}

void Library::calculateFines() {
    for (Account* account : accounts) {
        double totalFine = 0.0;
        for (int bookId : account->getCurrentlyBorrowedBooks()) {
            totalFine += calculateFine(account->getUserId(), bookId);
        }
        account->updateFine(totalFine);
    }
    saveAccounts();
}

double Library::calculateFine(const std::string& userId, int bookId) const {
    User* user = getUser(userId);
    Book* book = getBook(bookId);
    Account* account = getAccountByUserId(userId);

    if (!user || !book || !account) {
        return 0.0;
    }

    // Get the borrow record for this book
    for (const BorrowRecord& record : account->getBorrowHistory()) {
        if (record.bookId == bookId && !record.returned) {
            // Calculate days overdue
            time_t now = time(0);
            struct tm due = {};
            std::stringstream ss(record.dueDate);
            ss >> std::get_time(&due, "%a %b %d %H:%M:%S %Y");
            
            if (ss.fail()) {
                return 0.0;
            }
            
            time_t dueTime = mktime(&due);
            double diff = difftime(now, dueTime);
            
            if (diff < 0) {
                return 0.0;
            }
            
            // Convert seconds to days (10 seconds = 1 day for testing)
            int daysOverdue = static_cast<int>(diff / 10);
            
            // Calculate fine based on user type, starting after 15 days
            if (user->getRole() == "Student" && daysOverdue > 15) {
                return (daysOverdue - 15) * 10.0; // 10 rupees per day for students, after 15 days
            } else if (user->getRole() == "Faculty") {
                return 0.0; // No fine for faculty
            }
        }
    }
    
    return 0.0;
}

void Library::displayAllBooks() const {
    std::cout << "Library Books:" << std::endl;
    for (const Book* book : books) {
        std::cout << "ID: " << book->getBookId()
                  << ", Title: " << book->getTitle()
                  << ", Author: " << book->getAuthor()
                  << ", Status: " << book->getStatus()
                  << ", Borrowed By: " << book->getBorrowedBy() << std::endl;
    }
}

void Library::displayUserDetails(const std::string& userId) const {
    User* user = getUser(userId);
    Account* account = getAccountByUserId(userId);

    if (!user || !account) {
        std::cout << "User not found." << std::endl;
        return;
    }

    std::cout << "User Details:" << std::endl;
    std::cout << "ID: " << user->getUserId()
              << ", Name: " << user->getName()
              << ", Role: " << user->getRole()
              << ", Email: " << user->getEmail()
              << ", Total Fine: " << account->getTotalFine()
              << ", Fine Paid: " << (account->isFinePaid() ? "Yes" : "No") << std::endl;
}

void Library::displayUserBorrowHistory(const std::string& userId) const {
    Account* account = getAccountByUserId(userId);
    if (!account) {
        std::cout << "User not found." << std::endl;
        return;
    }

    std::cout << "Borrow History for User " << userId << ":" << std::endl;
    for (const BorrowRecord& record : account->getBorrowHistory()) {
        std::cout << "Book ID: " << record.bookId
                  << ", Borrowed: " << record.borrowDate
                  << ", Due: " << record.dueDate
                  << ", Returned: " << (record.returned ? "Yes" : "No")
                  << ", Fine: " << record.fine << std::endl;
    }
}

bool Library::changePassword(const std::string& userId, const std::string& oldPassword, const std::string& newPassword) {
    User* user = authenticateUser(userId, oldPassword);
    if (user) {
        user->setPassword(newPassword);
        saveUsers();
        return true;
    }
    return false;
}

// Add this helper function to check if file exists and has content
bool Library::fileExistsAndHasContent(const std::string& filename) const {
    std::ifstream file(filename);
    return file.good() && file.peek() != std::ifstream::traits_type::eof();
}

bool Library::updateBook(int bookId, const std::string& title, const std::string& author,
                        const std::string& publisher, int year, const std::string& isbn) {
    Book* book = getBook(bookId);
    if (!book) {
        return false;
    }

    book->setTitle(title);
    book->setAuthor(author);
    book->setPublisher(publisher);
    book->setYear(year);
    book->setIsbn(isbn);
    
    saveBooks();
    return true;
}

bool Library::reserveBook(const std::string& userId, int bookId) {
    User* user = getUser(userId);
    Book* book = getBook(bookId);
    
    if (!user || !book) return false;
    
    // Can't reserve if book is available (should borrow instead)
    if (book->isAvailable()) {
        std::cout << "Book is available for borrowing. No need to reserve.\n";
        return false;
    }
    
    // Can't reserve if user has already borrowed or reserved the book
    if (book->getBorrowedBy() == userId || book->hasReservation(userId)) {
        std::cout << "You have already borrowed or reserved this book.\n";
        return false;
    }
    
    if (book->reserve(userId)) {
        std::cout << "Book reserved successfully. You will be notified when it becomes available.\n";
        saveBooks();
        return true;
    }
    return false;
}

bool Library::cancelReservation(const std::string& userId, int bookId) {
    Book* book = getBook(bookId);
    if (!book) return false;
    
    if (book->cancelReservation(userId)) {
        std::cout << "Reservation cancelled successfully.\n";
        saveBooks();
        return true;
    }
    std::cout << "No reservation found for this book.\n";
    return false;
}

void Library::checkAndUpdateReservations() {
    for (Book* book : books) {
        if (book->isReservationExpired()) {
            std::string userId = book->getNextReservation();
            std::cout << "Reservation expired for user " << userId << " for book: " << book->getTitle() << "\n";
            book->removeExpiredReservation();
            saveBooks();
        }
    }
}

std::vector<Book*> Library::getReservedBooks(const std::string& userId) const {
    std::vector<Book*> reservedBooks;
    for (Book* book : books) {
        if (book->hasReservation(userId)) {
            reservedBooks.push_back(book);
        }
    }
    return reservedBooks;
}

bool Library::isBookReservedForUser(const std::string& userId, int bookId) const {
    Book* book = getBook(bookId);
    return book && book->hasReservation(userId);
}

void Library::notifyUserAboutReservation(const std::string& userId, int bookId) const {
    Book* book = getBook(bookId);
    if (!book) return;
    
    if (book->getNextReservation() == userId) {
        std::cout << "\nNOTIFICATION: The book '" << book->getTitle() 
                  << "' (ID: " << book->getBookId() 
                  << ") is now available for you to borrow.\n"
                  << "You have 3 days to borrow the book before the reservation expires.\n";
    }
} 