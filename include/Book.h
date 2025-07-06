#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <queue>
#include <ctime>

struct Reservation {
    std::string userId;
    time_t reservationDate;
    time_t notificationDate;  // When the book became available
    bool notified;           // Whether user has been notified
    
    Reservation(const std::string& id) : 
        userId(id), 
        reservationDate(time(nullptr)),
        notificationDate(0),
        notified(false) {}
};

class Book {
private:
    int bookId;
    std::string title;
    std::string author;
    std::string publisher;
    int year;
    std::string isbn;
    std::string status;  // "Available", "Borrowed", or "Reserved"
    std::string borrowedBy;  // UserID of the borrower
    std::queue<Reservation> reservationQueue;  // Queue of reservations

public:
    Book(int id, const std::string& title, const std::string& author,
         const std::string& publisher, int year, const std::string& isbn)
        : bookId(id), title(title), author(author), publisher(publisher),
          year(year), isbn(isbn), status("Available"), borrowedBy("") {}

    // Getters
    int getBookId() const { return bookId; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getPublisher() const { return publisher; }
    int getYear() const { return year; }
    std::string getIsbn() const { return isbn; }
    std::string getStatus() const { return status; }
    std::string getBorrowedBy() const { return borrowedBy; }

    // Reservation methods
    bool reserve(const std::string& userId) {
        if (status == "Available" || hasReservation(userId)) {
            return false;
        }
        reservationQueue.push(Reservation(userId));
        return true;
    }

    bool cancelReservation(const std::string& userId) {
        if (reservationQueue.empty()) return false;
        
        // Since we can't easily remove from a queue, we'll create a new queue
        std::queue<Reservation> newQueue;
        bool found = false;
        
        while (!reservationQueue.empty()) {
            Reservation res = reservationQueue.front();
            reservationQueue.pop();
            
            if (res.userId != userId) {
                newQueue.push(res);
            } else {
                found = true;
            }
        }
        
        reservationQueue = newQueue;
        return found;
    }

    bool hasReservation(const std::string& userId) const {
        std::queue<Reservation> temp = reservationQueue;
        while (!temp.empty()) {
            if (temp.front().userId == userId) return true;
            temp.pop();
        }
        return false;
    }

    std::string getNextReservation() const {
        if (reservationQueue.empty()) return "";
        return reservationQueue.front().userId;
    }

    void notifyNextInQueue() {
        if (!reservationQueue.empty()) {
            Reservation& res = reservationQueue.front();
            res.notificationDate = time(nullptr);
            res.notified = true;
        }
    }

    bool isReservationExpired() const {
        if (reservationQueue.empty() || !reservationQueue.front().notified) {
            return false;
        }
        
        time_t now = time(nullptr);
        return (now - reservationQueue.front().notificationDate) > (3 * 10); // 3 days (10 seconds = 1 day for testing)
    }

    void removeExpiredReservation() {
        if (!reservationQueue.empty() && isReservationExpired()) {
            reservationQueue.pop();
        }
    }

    // Setters
    void setTitle(const std::string& t) { title = t; }
    void setAuthor(const std::string& a) { author = a; }
    void setPublisher(const std::string& p) { publisher = p; }
    void setYear(int y) { year = y; }
    void setIsbn(const std::string& i) { isbn = i; }
    void setStatus(const std::string& s) { status = s; }
    void setBorrowedBy(const std::string& userId) { borrowedBy = userId; }

    // Other methods
    bool isAvailable() const { return status == "Available"; }
    void markAsBorrowed(const std::string& userId) {
        status = "Borrowed";
        borrowedBy = userId;
    }
    void markAsReturned() {
        status = "Available";
        borrowedBy = "";
        if (!reservationQueue.empty()) {
            notifyNextInQueue();
        }
    }
};

#endif 