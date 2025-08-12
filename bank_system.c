#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

#define RED    "\x1b[31m"
#define GREEN  "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE   "\x1b[34m"
#define RESET  "\x1b[0m"

// ------- STRUCT DEFINITION -------
struct Account {
    int acc_no;
    char name[100];
    float balance;
    int pin;
};

// ------- UTILITY FUNCTIONS -------
int accountExists(int acc_no) {
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) return 0;
    struct Account a;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no == acc_no) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int authenticate(int acc_no, int pin) {
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) return 0;
    struct Account a;
    int result = 0;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no == acc_no && a.pin == pin) {
            result = 1;
            break;
        }
    }
    fclose(fp);
    return result;
}

// ------- CORE FEATURES -------
void createAccount() {
    struct Account a;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &a.acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    if (accountExists(a.acc_no)) {
        printf(RED "Account number already exists. Choose another.\n" RESET);
        return;
    }
    printf(GREEN "Enter account holder name: " RESET);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF); // Flush newline before fgets
    if (!fgets(a.name, sizeof(a.name), stdin)) {
        printf(RED "Invalid name input.\n" RESET);
        return;
    }
    size_t len = strlen(a.name);
    if (len > 0 && a.name[len - 1] == '\n')
        a.name[len - 1] = '\0';
    if (strlen(a.name) == 0) {
        printf(RED "Name cannot be empty.\n" RESET);
        return;
    }
    printf(GREEN "Set a 4-digit PIN: " RESET);
    if (scanf("%d", &a.pin) != 1 || a.pin < 1000 || a.pin > 9999) {
        printf(RED "Invalid PIN. Must be a 4-digit number.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter initial balance: " RESET);
    if (scanf("%f", &a.balance) != 1) {
        printf(RED "Invalid balance input.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    if (a.balance < 0.0f) {
        printf(RED "Initial balance cannot be negative.\n" RESET);
        return;
    }

    FILE *fp = fopen("accounts.dat", "ab");
    if (!fp) {
        printf(RED "Error opening accounts file.\n" RESET);
        return;
    }
    fwrite(&a, sizeof(struct Account), 1, fp);
    fclose(fp);

    printf(GREEN "Account Created: %d, Name: %s, Balance: %.2f\n" RESET, a.acc_no, a.name, a.balance);
}

void deposit() {
    int acc_no, pin;
    float amount;
    int found = 0;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter your PIN: " RESET);
    if (scanf("%d", &pin) != 1) {
        printf(RED "Invalid PIN input.\n" RESET);
        return;
    }
    if (!authenticate(acc_no, pin)) {
        printf(RED "Authentication failed. Wrong account or PIN.\n" RESET);
        return;
    }
    printf(GREEN "Enter amount to deposit: " RESET);
    if (scanf("%f", &amount) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    if (amount <= 0.0f) {
        printf(RED "Amount must be positive.\n" RESET);
        return;
    }

    FILE *fp = fopen("accounts.dat", "rb+");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }

    struct Account a;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no == acc_no) {
            found = 1;
            a.balance += amount;
            fseek(fp, -sizeof(struct Account), SEEK_CUR);
            fwrite(&a, sizeof(struct Account), 1, fp);
            printf(GREEN "Deposit successful. New balance: %.2f\n" RESET, a.balance);
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf(RED "Account not found.\n" RESET);
    }
}

void withdraw() {
    int acc_no, pin;
    float amount;
    int found = 0;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter your PIN: " RESET);
    if (scanf("%d", &pin) != 1) {
        printf(RED "Invalid PIN input.\n" RESET);
        return;
    }
    if (!authenticate(acc_no, pin)) {
        printf(RED "Authentication failed. Wrong account or PIN.\n" RESET);
        return;
    }
    printf(GREEN "Enter amount to withdraw: " RESET);
    if (scanf("%f", &amount) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    if (amount <= 0.0f) {
        printf(RED "Amount must be positive.\n" RESET);
        return;
    }

    FILE *fp = fopen("accounts.dat", "rb+");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }

    struct Account a;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no == acc_no) {
            found = 1;
            if (a.balance >= amount) {
                a.balance -= amount;
                fseek(fp, -sizeof(struct Account), SEEK_CUR);
                fwrite(&a, sizeof(struct Account), 1, fp);
                printf(GREEN "Withdraw successful. New balance: %.2f\n" RESET, a.balance);
            } else {
                printf(RED "Insufficient balance.\n" RESET);
            }
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf(RED "Account not found.\n" RESET);
    }
}

// ******** TRANSFER MONEY FEATURE (Add here, after withdraw) ********
void transferMoney() {
    FILE *fp;
    struct Account sender, receiver;
    int senderAcc, receiverAcc, senderPin;
    float amount;
    int foundSender = 0, foundReceiver = 0;
    long senderPos, receiverPos;

    printf("\n" GREEN "--- Money Transfer ---\n" RESET);
    printf("Enter Sender Account Number: ");
    scanf("%d", &senderAcc);
    printf("Enter Sender PIN: ");
    scanf("%d", &senderPin);

    fp = fopen("accounts.dat", "rb+");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }

    // Find sender
    while (fread(&sender, sizeof(struct Account), 1, fp) == 1) {
        if (sender.acc_no == senderAcc && sender.pin == senderPin) {
            foundSender = 1;
            senderPos = ftell(fp) - sizeof(struct Account);
            break;
        }
    }
    if (!foundSender) {
        printf(RED "Sender account not found or invalid PIN.\n" RESET);
        fclose(fp);
        return;
    }

    printf("Enter Receiver Account Number: ");
    scanf("%d", &receiverAcc);

    rewind(fp); // Start search again for receiver

    // Find receiver
    while (fread(&receiver, sizeof(struct Account), 1, fp) == 1) {
        if (receiver.acc_no == receiverAcc) {
            foundReceiver = 1;
            receiverPos = ftell(fp) - sizeof(struct Account);
            break;
        }
    }
    if (!foundReceiver) {
        printf(RED "Receiver account not found.\n" RESET);
        fclose(fp);
        return;
    }

    printf("Enter amount to transfer: ");
    scanf("%f", &amount);

    if (amount <= 0) {
        printf(RED "Invalid amount.\n" RESET);
        fclose(fp);
        return;
    }

    if (sender.balance < amount) {
        printf(RED "Insufficient balance in sender's account.\n" RESET);
        fclose(fp);
        return;
    }

    sender.balance -= amount;
    fseek(fp, senderPos, SEEK_SET);
    fwrite(&sender, sizeof(struct Account), 1, fp);

    receiver.balance += amount;
    fseek(fp, receiverPos, SEEK_SET);
    fwrite(&receiver, sizeof(struct Account), 1, fp);

   printf(GREEN "Rs. %.2f successfully transferred from %s to %s\n" RESET, amount, sender.name, receiver.name);


    fclose(fp);
}
// ******** END OF TRANSFER FEATURE ********

// ------- REMAINING FEATURES -------
void viewAccounts() {
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }
    struct Account a;
    printf(BLUE "\n+-------------+---------------------------+------------------------------+\n" RESET);
    printf(BLUE "| Account No  | Name                      | Balance                      |\n" RESET);
    printf(BLUE "+-------------+---------------------------+------------------------------+\n" RESET);
    while (fread(&a, sizeof(struct Account), 1, fp))
        printf("| %-11d | %-25s | %-28.2f |\n", a.acc_no, a.name, a.balance);
    printf(BLUE "+-------------+---------------------------+------------------------------+\n" RESET);
    fclose(fp);
}

void searchAccount() {
    int acc_no, pin;
    printf(GREEN "Enter account number to search: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter PIN for authentication: " RESET);
    if (scanf("%d", &pin) != 1) {
        printf(RED "Invalid PIN input.\n" RESET);
        return;
    }
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }
    struct Account a;
    int found = 0;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no == acc_no && a.pin == pin) {
            found = 1;
            printf(BLUE "\nAccount Details:\n" RESET);
            printf(YELLOW "Account No: %d\nName: %s\nBalance: %.2f\n" RESET, a.acc_no, a.name, a.balance);
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf(RED "Account not found or PIN incorrect.\n" RESET);
    }
}

void updateAccountName() {
    int acc_no, pin, found = 0;
    char newName[100];
    printf(GREEN "Enter account number to update: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter PIN for authentication: " RESET);
    if (scanf("%d", &pin) != 1) {
        printf(RED "Invalid PIN input.\n" RESET);
        return;
    }
    printf(GREEN "Enter new account holder name: " RESET);
    int ch; while ((ch = getchar()) != '\n' && ch != EOF);

    if (!fgets(newName, sizeof(newName), stdin)) {
        printf(RED "Invalid name input.\n" RESET);
        return;
    }
    size_t len = strlen(newName);
    if (len > 0 && newName[len - 1] == '\n')
        newName[len - 1] = '\0';
    if (strlen(newName) == 0) {
        printf(RED "Name cannot be empty.\n" RESET);
        return;
    }
    FILE *fp = fopen("accounts.dat", "rb+");
    if (!fp) {
        printf(RED "No accounts found.\n" RESET);
        return;
    }
    struct Account a;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no == acc_no && a.pin == pin) {
            found = 1;
            strcpy(a.name, newName);
            fseek(fp, -sizeof(struct Account), SEEK_CUR);
            fwrite(&a, sizeof(struct Account), 1, fp);
            printf(GREEN "Account holder's name updated successfully.\n" RESET);
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf(RED "Account not found or authentication failed.\n" RESET);
    }
}

void deleteAccount() {
    int acc_no, pin, found = 0;
    printf(GREEN "Enter account number to delete: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter PIN for authentication: " RESET);
    if (scanf("%d", &pin) != 1) {
        printf(RED "Invalid PIN input.\n" RESET);
        return;
    }
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printf(RED "No accounts found.\n" RESET);
        return;
    }
    FILE *temp = fopen("temp.dat", "wb");
    struct Account a;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no != acc_no) {
            fwrite(&a, sizeof(struct Account), 1, temp);
        }
        else if (a.pin == pin) {
            found = 1;
        } else {
            fwrite(&a, sizeof(struct Account), 1, temp); // Wrong PIN, do not delete
        }
    }
    fclose(fp);
    fclose(temp);

    remove("accounts.dat");
    rename("temp.dat", "accounts.dat");
    if (found) {
        printf(GREEN "Account %d deleted successfully.\n" RESET, acc_no);
    } else {
        printf(RED "Account not found or PIN incorrect.\n" RESET);
    }
}

// ------- MAIN MENU -------
int main() {
    int choice;
    do {
        system(CLEAR);
        printf(BLUE "\n=== Bank Account Management ===\n" RESET);
        printf(YELLOW "1. Create Account\n" RESET);
        printf(YELLOW "2. Deposit\n" RESET);
        printf(YELLOW "3. Withdraw\n" RESET);
        printf(YELLOW "4. Transfer Money\n" RESET);   // <-- NEW transfer option added here
        printf(YELLOW "5. View Accounts\n" RESET);
        printf(YELLOW "6. Search Account\n" RESET);
        printf(YELLOW "7. Update Account Holder Name\n" RESET);
        printf(YELLOW "8. Delete Account\n" RESET);
        printf(YELLOW "9. Exit\n" RESET);
        printf(GREEN "Enter your choice: " RESET);

        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input!\n" RESET);
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                deposit();
                break;
            case 3:
                withdraw();
                break;
            case 4:
                transferMoney(); // <-- Calls the new feature
                break;
            case 5:
                viewAccounts();
                break;
            case 6:
                searchAccount();
                break;
            case 7:
                updateAccountName();
                break;
            case 8:
                deleteAccount();
                break;
            case 9:
                printf(GREEN "Exiting...\n" RESET);
                break;
            default:
                printf(RED "Invalid choice!\n" RESET);
        }
        if (choice != 9) {
            printf(YELLOW "\nPress Enter to continue..." RESET);
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            getchar();
        }
    } while (choice != 9);

    return 0;
}
