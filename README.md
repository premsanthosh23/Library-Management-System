# Library Management System

A comprehensive C++ library management system that handles book borrowing, user management, and fine calculations.

## Features

- **User Management**
  - Multiple user types: Students, Faculty, and Librarians
  - Secure login system
  - User profile management

- **Book Management**
  - Add, remove, and update books
  - Track book availability
  - Borrowing history

- **Borrowing Rules**
  - Students: Can borrow up to 3 books for 15 days with fine system
  - Faculty: Can borrow up to 5 books for 30 days (must return current books before borrowing new ones)
  - Fine calculation for overdue books (Students only)

- **Fine System**
  - Automatic fine calculation for overdue books
  - Fine payment tracking
  - Students: Rs. 10 per day for overdue books
  - Faculty: No fines, but must return current books before borrowing new ones

## System Requirements

- C++ compiler with C++11 support
- Standard C++ libraries

## File Structure

```
.
├── include/            # Header files
│   ├── Account.h      # Account management
│   ├── Book.h         # Book class definition
│   ├── Faculty.h      # Faculty user type
│   ├── Library.h      # Main library system
│   ├── Librarian.h    # Librarian user type
│   ├── Student.h      # Student user type
│   └── User.h         # Base user class
├── src/               # Source files
│   ├── Library.cpp    # Library implementation
│   └── main.cpp       # Main program
└── data/              # Data storage
    ├── users.txt      # User records
    ├── books.txt      # Book records
    └── accounts.txt   # Account records
```

## Usage

1. **Compilation**
   ```bash
   g++ -std=c++11 src/main.cpp src/Library.cpp -I include -o lms
   ```

2. **Running the Program**
   ```bash
   ./lms
   ```
3. For Calculation of fines:
   considered 10 seconds == 1 day;
## User Types and Permissions

### Student
- Can borrow up to 3 books
- 15-day borrowing period
- Incurs fines for late returns
- Must clear fines to borrow new books

### Faculty
- Can borrow up to 5 books
- 30-day borrowing period
- No fines for late returns
- Must return all current books before borrowing new ones

### Librarian
- Cannot borrow books
- Can add/remove/update books
- Can add/remove users
- Can view all user details and history
- Cannot remove users with borrowed books or pending fines
- Cannot remove books that are currently borrowed

## Data Persistence
- All data is automatically saved to files in the data directory
- Data is loaded when the program starts
- Automatic backup of data files 