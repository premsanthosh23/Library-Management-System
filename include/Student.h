#ifndef STUDENT_H
#define STUDENT_H

#include "User.h"

class Student : public User {
public:
    Student(const std::string& name, const std::string& email, const std::string& id)
        : User(name, email, id) {
        role = "Student";
        maxBorrowLimit = 3;
        maxBorrowPeriod = 15;  // 15 days
    }

    bool canBorrowBook() const override { return true; }
};

#endif 