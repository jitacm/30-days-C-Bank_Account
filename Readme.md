# 🏦 Bank Account Management System (C CLI)

A simple, secure CLI tool in C for creating, searching, updating, deleting, and managing bank accounts using file storage. Supports authentication and offers robust account operations right from your terminal.

## 🚀 Installation

**Requirements:**
- C Compiler (like GCC)
- Works on Windows, Linux, and macOS
- Recommended: GCC version 5+ (any C99-compliant compiler should work)

**Install & Run:**
1. Clone this repository:
    ```bash
    git clone https://github.com/your-username/your-repo.git
    cd your-repo
    ```

2. Build:
    ```bash
    gcc bank_system.c -o bank_system
    ```

3. Run:
    ```bash
    ./bank_system    # For Linux/macOS
    bank_system.exe  # For Windows
    ```

## 📦 About the Project

This is a command-line program that lets you:
- Open, manage, and delete bank accounts.
- Deposit and withdraw funds securely.
- Search for accounts with authentication.
- Update account holder names.
- Review all stored account information simply.

**How It Works:**
- Account data is stored in a binary file (`accounts.dat`).
- PIN authentication is used for important actions.
- The interface is textual (no GUI), but colorful and user-friendly.

## 🛠️ Features

- Create new accounts (set account number, full name, PIN, starting balance)
- Deposit to an account (PIN required)
- Withdraw from an account (PIN required)
- Search for an account (number & PIN)
- Update account holder’s name (PIN required)
- Delete an account (PIN required)
- View all accounts easily
- Input validation and helpful error feedback
- Cross-platform—works in any terminal

## 🎯 Menu Options & Actions

When you run the program, you’ll see:

1. Create Account  
   Enter an account number, full name, choose a 4-digit PIN, and enter the starting balance.

2. Deposit  
   Enter an account number, correct PIN, and deposit amount.

3. Withdraw  
   Enter an account number, PIN, and withdrawal amount.

4. View Accounts  
   Shows all account numbers, account holder names, and balances.

5. Search Account  
   Enter an account number and PIN to see the details of one account.

6. Update Account Holder Name  
   Enter an account number, PIN, and the new name to update your info.

7. Delete Account  
   Enter an account number and PIN to securely remove an account.

8. Exit  
   Close the program.

- Enter the menu option number (ex: `1`) and follow prompts.
- PIN codes (4-digit) are required for sensitive actions.
- Full names (including spaces) are supported.

## 💻 Common Commands

```bash
# Compile the program (Linux/macOS)
gcc bank_system.c -o bank_system

# Run the program (Linux/macOS)
./bank_system

# On Windows, after compiling:
bank_system.exe
```

## 🧩 Technologies Used

- C Language (C99)
- File I/O (binary storage)
- Terminal color feedback (ANSI codes)
- Runs directly in your command line shell

## 🤝 Contributing

- Fork the repo, make your changes, and open a pull request!
- Please keep code clean and well-documented to help new users.

## ❓ FAQ

- **Q: What if I forget my PIN?**  
  A: For your security, PINs cannot be recovered.
- **Q: Will this work on any operating system?**  
  A: Yes! The code is tested on Windows, Linux, and Mac terminals and auto-detects the best way to clear your screen and show color feedback.
