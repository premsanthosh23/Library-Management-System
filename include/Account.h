#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>
#include <set>

struct BorrowRecord {
    int bookId;
    std::string borrowDate;
    std::string dueDate;
    bool returned;
    double fine;
};

class Account {
private:
    std::string userId;
    double totalFine;
    bool finePaid;
    std::set<int> currentlyBorrowedBooks;
    std::vector<BorrowRecord> borrowHistory;

public:
    Account(const std::string& id) : userId(id), totalFine(0.0), finePaid(true) {}

    // Getters
    std::string getUserId() const { return userId; }
    double getTotalFine() const { return totalFine; }
    bool isFinePaid() const { return finePaid; }
    const std::set<int>& getCurrentlyBorrowedBooks() const { return currentlyBorrowedBooks; }
    const std::vector<BorrowRecord>& getBorrowHistory() const { return borrowHistory; }

    // Setters
    void setFinePaid(bool paid) { finePaid = paid; }
    void updateFine(double amount) { 
        totalFine += amount;
        if (totalFine <= 0) {
            totalFine = 0;
            finePaid = true;
        }
    }

    // Book management
    void addBorrowedBook(int bookId, const std::string& borrowDate, const std::string& dueDate) {
        currentlyBorrowedBooks.insert(bookId);
        BorrowRecord record{bookId, borrowDate, dueDate, false, 0.0};
        borrowHistory.push_back(record);
    }

    void removeBorrowedBook(int bookId) {
        currentlyBorrowedBooks.erase(bookId);
    }

    void addToHistory(const BorrowRecord& record) {
        borrowHistory.push_back(record);
    }

    bool canBorrowMore() const {
        return finePaid;  // Can only borrow if fines are paid
    }
};

#endif 