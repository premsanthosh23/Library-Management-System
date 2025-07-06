#ifndef LIBRARIAN_H
#define LIBRARIAN_H

#include "User.h"

class Librarian : public User {
public:
    Librarian(const std::string& name, const std::string& email, const std::string& id)
        : User(name, email, id) {
        role = "Librarian";
        maxBorrowLimit = 0;  // Librarians don't borrow books
        maxBorrowPeriod = 0;
    }

    bool canBorrowBook() const override { return false; }
};

#endif 