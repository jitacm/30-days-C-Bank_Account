#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"

struct Account {
    int acc_no;
    char name[100];
    float balance;
};

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

// Improved: Prompts for each field are clear and step-by-step
void createAccount() {
    struct Account a;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &a.acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    if (accountExists(a.acc_no)) {
        printf(RED "Account number already exists. Choose another.\n" RESET);
        return;
    }

    printf(GREEN "Enter account holder name: " RESET);
    if (scanf("%s", a.name) != 1) {
        printf(RED "Invalid name input.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    printf(GREEN "Enter initial balance: " RESET);
    if (scanf("%f", &a.balance) != 1) {
        printf(RED "Invalid balance input.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
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
    int acc_no;
    float amount;
    int found = 0;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter amount to deposit: " RESET);
    if (scanf("%f", &amount) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
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
    int acc_no;
    float amount;
    int found = 0;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter amount to withdraw: " RESET);
    if (scanf("%f", &amount) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
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

void viewAccounts() {
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }
    struct Account a;
    printf(BLUE "\n--- List of Accounts ---\n" RESET);
    printf(YELLOW "%-12s %-20s %-10s\n" RESET, "Account No", "Name", "Balance");
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        printf("%-12d %-20s %-10.2f\n", a.acc_no, a.name, a.balance);
    }
    fclose(fp);
}

int main() {
    int choice;
    do {
        system(CLEAR);
        printf(BLUE "\n=== Bank Account Management ===\n" RESET);
        printf(YELLOW "1. Create Account\n" RESET);
        printf(YELLOW "2. Deposit\n" RESET);
        printf(YELLOW "3. Withdraw\n" RESET);
        printf(YELLOW "4. View Accounts\n" RESET);
        printf(YELLOW "5. Exit\n" RESET);
        printf(GREEN "Enter your choice: " RESET);
        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input!\n" RESET);
            int ch; while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }

        switch(choice) {
            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: viewAccounts(); break;
            case 5: printf(GREEN "Exiting...\n" RESET); break;
            default: printf(RED "Invalid choice!\n" RESET);
        }
        if (choice != 5) {
            printf(YELLOW "\nPress Enter to continue..." RESET);
            int ch; while ((ch = getchar()) != '\n' && ch != EOF);
            getchar();
        }
    } while(choice != 5);

    return 0;
}
