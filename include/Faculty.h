#ifndef FACULTY_H
#define FACULTY_H

#include "User.h"
#include "Account.h"

class Faculty : public User {
private:
    Account* account;

public:
    Faculty(const std::string& name, const std::string& email, const std::string& id)
        : User(name, email, id), account(nullptr) {
        role = "Faculty";
        maxBorrowLimit = 5;
        maxBorrowPeriod = 30;  // 30 days
    }

    void setAccount(Account* acc) { account = acc; }

    bool canBorrowBook() const override { 
        // Faculty can only borrow if they have no currently borrowed books
        return account ? account->getCurrentlyBorrowedBooks().empty() : false;
    }
};

#endif 