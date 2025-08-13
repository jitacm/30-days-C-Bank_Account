# 🏦 Bank Account Management System (C CLI) — Secure & Feature-Rich Version

A comprehensive, secure command-line tool in C for managing bank accounts, handling transactions, processing loans, and calculating interest. This enhanced version builds upon robust security foundations, including PIN hashing, account lockout, masked PIN input, admin-level authentication, and adds crucial features like loan management and daily interest crediting.

---

## 🚀 Installation

### Requirements

* **C Compiler:** GCC recommended (C99 compliant or later).
* **Standard Libraries:** `stdio.h`, `stdlib.h`, `string.h`, `time.h`, `ctype.h`, `termios.h` (for Unix-like systems), `unistd.h`.

### Build & Run

Ensure you are in the `jitacm-30-days-c-bank_account` directory.

```bash
# Compile the program (Linux/macOS)
# Note: The SHA-256 implementation is internal, so no external crypto library is strictly required.
gcc bank_system.c -o bank_system -lm

# Run the program (Linux/macOS)
./bank_system

# On Windows, after compiling:
bank_system.exe
```

---

## 📦 About the Project

This CLI program provides a robust platform for managing bank accounts, offering a wide range of functionalities:

* **Account Management:** Create new accounts with secure PINs and initial balances.
* **Secure Transactions:** Deposit, withdraw, and transfer funds efficiently and securely.
* **PIN Security:** Utilizes salted SHA-256 hashing for PINs, masked input, and account lockout mechanisms.
* **Transaction History:** Detailed logging of all account activities for audit purposes.
* **Loan Management:** Users can apply for loans, make repayments, and track their status. Administrators can review and approve/reject loan applications.
* **Interest Calculation:** Automatically calculates and credits daily interest on account balances.
* **Admin Controls:** Comprehensive tools for administrators, including account monitoring, unlocking, transaction review, loan management, and interest processing.

---

## 🔐 Security Features

### 1. PIN Hashing with SHA-256 and Salt

* PINs are never stored in plaintext.
* Each PIN is hashed using SHA-256 combined with a unique random salt per account.
* Protects against data breaches and offline attacks.

### 2. Masked PIN Input

* PINs are entered invisibly (masked with `*`) to prevent shoulder surfing.
* Works cross-platform (Windows and Unix-like systems).

### 3. Account Lockout

* After 3 failed PIN attempts, accounts are locked.
* Locked accounts cannot be accessed until unlocked by an administrator.
* Prevents brute-force PIN guessing attacks.

### 4. Admin-Level Authentication

* Critical administrative actions require admin PIN authentication.
* Admin PIN is securely stored hashed with salt.
* Admin PIN setup is prompted on the first run.

---

## 🎯 Menu Options & Actions

This application provides distinct menus for users and administrators, ensuring proper separation of duties and access control.

### User Menu Actions:

* **Create Account**  
  Enter account number, full name, set a 4-digit PIN (masked input), and initial balance.
* **Deposit**  
  Enter account number, PIN (masked), and deposit amount.
* **Withdraw**  
  Enter account number, PIN (masked), and withdrawal amount.
* **Transfer Money**  
  Securely transfer funds between accounts after PIN authentication.
* **View Transaction History**  
  View a detailed log of all transactions (deposits, withdrawals, transfers, loan repayments, interest credits) for the authenticated account.
* **Generate Account Statement**  
  View current balance and account holder details after authentication.
* **Apply for Loan**  
  Submit a loan application after PIN authentication.
* **Repay Loan**  
  Make payments towards an outstanding loan after PIN authentication.

### Admin Menu Actions:

* **View All Accounts**  
  Displays all accounts with numbers, names, and balances.
* **Search Account**  
  Enter account number to view details.
* **Update Account Holder Name**  
  Update the name associated with an account. Requires admin PIN authentication.
* **Delete Account**  
  Permanently remove an account from the system. Requires admin PIN authentication.
* **Unlock User Account**  
  Manually unlock accounts that have been locked due to failed PIN attempts.
* **Process Scheduled Interest** (Admin Triggered)  
  Manually initiates the daily interest calculation and crediting process for all eligible accounts.
* **View Interest Log**  
  Displays a detailed log of all interest calculation events.
* **Exit**  
  Return to the main menu or close the program.

---

## ⚙️ How to Use Admin Features

* On the first run, you will be prompted to set an admin PIN. Keep this PIN secure.
* Select "Admin Login" from the main menu and enter the correct Admin PIN to access the Admin Menu.
* The Admin Menu provides access to all administrative functions, including managing accounts, unlocking user accounts, processing scheduled interest calculations, and viewing logs.
* Sensitive operations like updating account names, deleting accounts, and processing interest require admin PIN authentication.
* Admin PINs are entered using masked input for enhanced security.

---

## 📈 Interest Calculation & Scheduled Updates

This system now incorporates a daily interest calculation feature for user account balances, alongside loan management capabilities.

### How Interest Calculation Works:
* **Daily Calculation:** Interest is computed daily based on the account's current balance and a predefined annual interest rate (`ANNUAL_INTEREST_RATE`, set at 5%).
* **Eligibility:** Interest is credited only to accounts with a positive balance.
* **Tracking:** The system records the `last_interest_date` for each account to ensure calculations are performed approximately once every 24 hours.
* **Admin Triggered:** The interest calculation process (`Process Scheduled Interest`) must be manually initiated by an administrator via the Admin Menu. This provides administrative control and visibility over when interest is applied.
* **Logging:** All interest crediting events are logged in a dedicated `interest_log.dat` file, accessible via the Admin Menu (`View Interest Log`), detailing the account, date, amount credited, and new balance.

### Key Components:
* `ANNUAL_INTEREST_RATE`: Configurable constant for the interest rate.
* `Account` Structure: Updated to store `last_interest_date`.
* `TransactionType`: New type `INTEREST_CREDIT` for logging.
* `processScheduledInterest()`: Admin function to perform calculations.
* `viewInterestLog()`: Admin function to display logs.

**Note:** This implementation focuses on the calculation and admin-triggered application of interest. True background/automatic scheduling (e.g., running daily without admin intervention) is not included in this version but could be a future enhancement.

---

## 💻 Common Commands

```bash
# Compile the program (Linux/macOS)
gcc bank_system.c -o bank_system -lm

# Run the program (Linux/macOS)
./bank_system

# On Windows, after compiling:
bank_system.exe
```

---

## 🧩 Technologies Used

* **C Language:** C99 Standard for core implementation.
* **File I/O:** Binary file storage for persistent data (accounts, transactions, loans, admin credentials, interest logs).
* **Hashing:** Internal SHA-256 implementation for secure PIN and credential storage.
* **Terminal Interaction:**
    * ANSI escape codes for colored output.
    * Platform-specific handling (`termios.h` on Unix-like systems, `conio.h` on Windows) for masked input.
* **Time & Date:** `time.h` library used for timestamps, transaction logging, and interest calculation scheduling.

---

## 🗑️ About `tempCodeRunnerFile.c`

This file appears to be a temporary or backup file generated by some IDEs or code runners. It duplicates the main program code and is not required for the project to function.

**Recommendation:**  
You should delete `tempCodeRunnerFile.c` from the repository to keep the project clean and avoid confusion.

---

## 🤝 Contributing

Fork the repo, make your changes, and open a pull request!  
Please keep code clean and well-documented to help new users.

---

## ❓ FAQ

**Q: What if I forget my User PIN?**  
A: For your security, PINs cannot be recovered. If your account is locked due to failed attempts, contact an administrator to unlock it.

**Q: What if I forget my Admin PIN?**  
A: If the Admin PIN is lost, the `admin.dat` file will need to be deleted to reset the admin credentials (the system will prompt for a new PIN on the next run). This is a security measure to prevent unauthorized access if admin credentials are compromised.

**Q: Will this work on any operating system?**  
A: Yes! The core logic is cross-platform. Input masking uses platform-specific headers (`termios.h` for Unix-like systems, `conio.h` for Windows). The build instructions provide guidance for common environments.

**Q: How is interest calculated?**  
A: Interest is calculated daily based on the account's balance and a fixed annual rate (5%). This process is triggered manually by an administrator via the Admin Menu (`Process Scheduled Interest`).

---

Thank you for using the Bank Account Management System!
