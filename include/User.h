    #ifndef USER_H
#define USER_H

#include <string>

class User {
protected:
    std::string userId;
    std::string name;
    std::string email;
    std::string password;
    std::string role;
    int maxBorrowLimit;
    int maxBorrowPeriod;  // in days

public:
    User(const std::string& name, const std::string& email, const std::string& id)
        : name(name), email(email), userId(id), maxBorrowLimit(0), maxBorrowPeriod(0) {}
    
    virtual ~User() = default;

    // Getters
    std::string getUserId() const { return userId; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getPassword() const { return password; }
    std::string getRole() const { return role; }
    int getMaxBorrowLimit() const { return maxBorrowLimit; }
    int getMaxBorrowPeriod() const { return maxBorrowPeriod; }

    // Setters
    void setPassword(const std::string& pwd) { password = pwd; }
    void setRole(const std::string& r) { role = r; }

    // Virtual functions
    virtual bool canBorrowBook() const { return true; }
};

#endif 