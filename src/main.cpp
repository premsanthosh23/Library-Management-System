#include "Library.h"
#include <iostream>
#include <limits>

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void displayLibrarianMenu() {
    std::cout << "\nLibrarian Menu:\n";
    std::cout << "1. Add User\n";
    std::cout << "2. Remove User\n";
    std::cout << "3. Add Book\n";
    std::cout << "4. Remove Book\n";
    std::cout << "5. Update Book\n";
    std::cout << "6. Display All Books\n";
    std::cout << "7. Display User Details\n";
    std::cout << "8. Change Password\n";
    std::cout << "9. Logout\n";
    std::cout << "Enter your choice: ";
}

void displayUserMenu(const std::string& role) {
    std::cout << "\nUser Menu:\n";
    std::cout << "1. View Available Books\n";
    std::cout << "2. Borrow Book\n";
    std::cout << "3. Return Book\n";
    std::cout << "4. View My Borrowed Books\n";
    std::cout << "5. Reserve Book\n";
    std::cout << "6. Cancel Reservation\n";
    std::cout << "7. View My Reserved Books\n";
    if (role != "Faculty") {
        std::cout << "8. View My Fines\n";
        std::cout << "9. Pay Fine\n";
        std::cout << "10. Change Password\n";
        std::cout << "11. Logout\n";
    } else {
        std::cout << "8. Change Password\n";
        std::cout << "9. Logout\n";
    }
    std::cout << "Enter your choice: ";
}

void handleLibrarianMenu(Library& lib, User* user) {
    int choice;
    std::string input;

    while (true) {
        displayLibrarianMenu();
        std::cin >> choice;
        clearInputBuffer();

        switch (choice) {
            case 1: { // Add User
                std::string name, email, password, type, id;
                std::cout << "Enter user details:\n";
                std::cout << "ID (format: S001 for Student, F001 for Faculty): ";
                std::getline(std::cin, id);
                std::cout << "Name: ";
                std::getline(std::cin, name);
                std::cout << "Email: ";
                std::getline(std::cin, email);
                std::cout << "Password: ";
                std::getline(std::cin, password);
                std::cout << "Type (Student/Faculty/Librarian): ";
                std::getline(std::cin, type);

                User* newUser = lib.addUser(name, email, password, type, id);
                if (newUser) {
                    lib.createAccount(newUser);
                    std::cout << "User added successfully!\n";
                } else {
                    std::cout << "Failed to add user.\n";
                }
                break;
            }
            case 2: { // Remove User
                std::string userId;
                std::cout << "Enter user ID to remove: ";
                std::getline(std::cin, userId);
                
                if (userId == user->getUserId()) {
                    std::cout << "Error: Cannot remove your own account!\n";
                } else {
                    // Display user details before removal
                    std::cout << "\nUser details before removal:\n";
                    lib.displayUserDetails(userId);
                    lib.displayUserBorrowHistory(userId);
                    
                    std::cout << "\nAre you sure you want to remove this user? (y/n): ";
                    char confirm;
                    std::cin >> confirm;
                    clearInputBuffer();
                    
                    if (confirm == 'y' || confirm == 'Y') {
                        if (lib.removeUser(userId)) {
                            std::cout << "User removed successfully!\n";
                        }
                    } else {
                        std::cout << "User removal cancelled.\n";
                    }
                }
                break;
            }
            case 3: { // Add Book
                std::string title, author, publisher, isbn;
                int year;
                
                std::cout << "Enter book details:\n";
                std::cout << "Title: ";
                std::getline(std::cin, title);
                std::cout << "Author: ";
                std::getline(std::cin, author);
                std::cout << "Publisher: ";
                std::getline(std::cin, publisher);
                std::cout << "Year: ";
                std::cin >> year;
                clearInputBuffer();
                std::cout << "ISBN: ";
                std::getline(std::cin, isbn);

                Book* newBook = lib.addBook(title, author, publisher, year, isbn);
                if (newBook) {
                    std::cout << "Book added successfully with ID: " << newBook->getBookId() << "\n";
                } else {
                    std::cout << "Failed to add book.\n";
                }
                break;
            }
            case 4: { // Remove Book
                int bookId;
                std::cout << "Enter book ID to remove: ";
                std::cin >> bookId;
                clearInputBuffer();

                // Display book details before removal
                Book* book = lib.getBook(bookId);
                if (book) {
                    std::cout << "\nBook details before removal:\n";
                    std::cout << "ID: " << book->getBookId() << "\n"
                             << "Title: " << book->getTitle() << "\n"
                             << "Author: " << book->getAuthor() << "\n"
                             << "Status: " << book->getStatus() << "\n";
                    
                    if (!book->isAvailable()) {
                        std::cout << "Borrowed by: " << book->getBorrowedBy() << "\n";
                    }
                    
                    std::cout << "\nAre you sure you want to remove this book? (y/n): ";
                    char confirm;
                    std::cin >> confirm;
                    clearInputBuffer();
                    
                    if (confirm == 'y' || confirm == 'Y') {
                        if (lib.removeBook(bookId)) {
                            std::cout << "Book removed successfully!\n";
                        }
                    } else {
                        std::cout << "Book removal cancelled.\n";
                    }
                } else {
                    std::cout << "Book not found.\n";
                }
                break;
            }
            case 5: { // Update Book
                int bookId;
                std::string title, author, publisher, isbn;
                int year;

                std::cout << "Enter book ID to update: ";
                std::cin >> bookId;
                clearInputBuffer();

                Book* book = lib.getBook(bookId);
                if (!book) {
                    std::cout << "Book not found.\n";
                    break;
                }

                std::cout << "Enter new details (press enter to keep current value):\n";
                std::cout << "Current Title: " << book->getTitle() << "\nNew Title: ";
                std::getline(std::cin, title);
                if (title.empty()) title = book->getTitle();

                std::cout << "Current Author: " << book->getAuthor() << "\nNew Author: ";
                std::getline(std::cin, author);
                if (author.empty()) author = book->getAuthor();

                std::cout << "Current Publisher: " << book->getPublisher() << "\nNew Publisher: ";
                std::getline(std::cin, publisher);
                if (publisher.empty()) publisher = book->getPublisher();

                std::cout << "Current Year: " << book->getYear() << "\nNew Year: ";
                std::string yearStr;
                std::getline(std::cin, yearStr);
                year = yearStr.empty() ? book->getYear() : std::stoi(yearStr);

                std::cout << "Current ISBN: " << book->getIsbn() << "\nNew ISBN: ";
                std::getline(std::cin, isbn);
                if (isbn.empty()) isbn = book->getIsbn();

                if (lib.updateBook(bookId, title, author, publisher, year, isbn)) {
                    std::cout << "Book updated successfully!\n";
                } else {
                    std::cout << "Failed to update book.\n";
                }
                break;
            }
            case 6: // Display All Books
                lib.displayAllBooks();
                break;
            case 7: { // Display User Details
                std::string userId;
                std::cout << "Enter user ID: ";
                std::getline(std::cin, userId);
                lib.displayUserDetails(userId);
                lib.displayUserBorrowHistory(userId);
                break;
            }
            case 8: { // Change Password
                std::string oldPass, newPass;
                std::cout << "Enter old password: ";
                std::getline(std::cin, oldPass);
                std::cout << "Enter new password: ";
                std::getline(std::cin, newPass);

                if (lib.changePassword(user->getUserId(), oldPass, newPass)) {
                    std::cout << "Password changed successfully!\n";
                } else {
                    std::cout << "Failed to change password.\n";
                }
                break;
            }
            case 9: // Logout
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void handleUserMenu(Library& lib, User* user) {
    int choice;
    bool isFaculty = (user->getRole() == "Faculty");
    
    while (true) {
        // Check for any available reservations
        std::vector<Book*> reservedBooks = lib.getReservedBooks(user->getUserId());
        for (Book* book : reservedBooks) {
            if (book->isAvailable()) {
                lib.notifyUserAboutReservation(user->getUserId(), book->getBookId());
            }
        }
        
        displayUserMenu(user->getRole());
        std::cin >> choice;
        clearInputBuffer();

        switch (choice) {
            case 1: // View Available Books
                lib.displayAllBooks();
                break;
            case 2: { // Borrow Book
                int bookId;
                std::cout << "Enter book ID to borrow: ";
                std::cin >> bookId;
                clearInputBuffer();

                if (lib.borrowBook(user->getUserId(), bookId)) {
                    std::cout << "Book borrowed successfully!\n";
                } else {
                    if (user->getRole() == "Faculty") {
                        std::cout << "Failed to borrow book. Please return your current books first.\n";
                    } else {
                        std::cout << "Failed to borrow book.\n";
                    }
                }
                break;
            }
            case 3: { // Return Book
                int bookId;
                std::cout << "Enter book ID to return: ";
                std::cin >> bookId;
                clearInputBuffer();

                if (lib.returnBook(user->getUserId(), bookId)) {
                    std::cout << "Book returned successfully!\n";
                } else {
                    std::cout << "Failed to return book.\n";
                }
                break;
            }
            case 4: // View My Borrowed Books
                lib.displayUserBorrowHistory(user->getUserId());
                break;
            case 5: { // Reserve Book
                int bookId;
                std::cout << "Enter book ID to reserve: ";
                std::cin >> bookId;
                clearInputBuffer();

                if (lib.reserveBook(user->getUserId(), bookId)) {
                    std::cout << "Book reserved successfully!\n";
                }
                break;
            }
            case 6: { // Cancel Reservation
                int bookId;
                std::cout << "Enter book ID to cancel reservation: ";
                std::cin >> bookId;
                clearInputBuffer();

                if (lib.cancelReservation(user->getUserId(), bookId)) {
                    std::cout << "Reservation cancelled successfully!\n";
                }
                break;
            }
            case 7: { // View My Reserved Books
                std::vector<Book*> reservedBooks = lib.getReservedBooks(user->getUserId());
                if (reservedBooks.empty()) {
                    std::cout << "You have no reserved books.\n";
                } else {
                    std::cout << "Your Reserved Books:\n";
                    for (const Book* book : reservedBooks) {
                        std::cout << "ID: " << book->getBookId()
                                << ", Title: " << book->getTitle()
                                << ", Status: " << book->getStatus()
                                << ", Currently held by: " << book->getBorrowedBy() << std::endl;
                    }
                }
                break;
            }
            case 8: {
                if (!isFaculty) { // View My Fines
                    Account* account = lib.getAccount(user->getUserId());
                    if (account) {
                        std::cout << "Current fine: Rs. " << account->getTotalFine() << std::endl;
                    }
                } else { // Change Password for Faculty
                    std::string oldPass, newPass;
                    std::cout << "Enter old password: ";
                    std::getline(std::cin, oldPass);
                    std::cout << "Enter new password: ";
                    std::getline(std::cin, newPass);

                    if (lib.changePassword(user->getUserId(), oldPass, newPass)) {
                        std::cout << "Password changed successfully!\n";
                    } else {
                        std::cout << "Failed to change password.\n";
                    }
                }
                break;
            }
            case 9: {
                if (!isFaculty) { // Pay Fine
                    Account* account = lib.getAccount(user->getUserId());
                    if (account && account->getTotalFine() > 0) {
                        double amount;
                        std::cout << "Current fine: Rs. " << account->getTotalFine() << std::endl;
                        std::cout << "Enter amount to pay: ";
                        std::cin >> amount;
                        clearInputBuffer();

                        lib.payFine(user->getUserId(), amount);
                        std::cout << "Payment processed. Remaining fine: Rs. " 
                                 << account->getTotalFine() << std::endl;
                    } else {
                        std::cout << "No fines to pay.\n";
                    }
                } else { // Logout for Faculty
                    return;
                }
                break;
            }
            case 10: {
                if (!isFaculty) { // Change Password for non-Faculty
                    std::string oldPass, newPass;
                    std::cout << "Enter old password: ";
                    std::getline(std::cin, oldPass);
                    std::cout << "Enter new password: ";
                    std::getline(std::cin, newPass);

                    if (lib.changePassword(user->getUserId(), oldPass, newPass)) {
                        std::cout << "Password changed successfully!\n";
                    } else {
                        std::cout << "Failed to change password.\n";
                    }
                }
                break;
            }
            case 11: { // Logout for non-Faculty
                if (!isFaculty) {
                    return;
                }
                break;
            }
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
        
        // Check and update reservations after each operation
        lib.checkAndUpdateReservations();
    }
}

int main() {
    Library lib;
    std::string userId, password;

    while (true) {
        std::cout << "\nLibrary Management System\n";
        std::cout << "1. Login\n";
        std::cout << "2. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        clearInputBuffer();

        if (choice == 2) break;

        if (choice == 1) {
            std::cout << "Enter User ID: ";
            std::getline(std::cin, userId);
            std::cout << "Enter Password: ";
            std::getline(std::cin, password);

            User* user = lib.authenticateUser(userId, password);
            if (user) {
                std::cout << "Welcome, " << user->getName() << "!\n";
                if (user->getRole() == "Librarian") {
                    handleLibrarianMenu(lib, user);
                } else {
                    handleUserMenu(lib, user);
                }
            } else {
                std::cout << "Invalid credentials. Please try again.\n";
            }
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
} 