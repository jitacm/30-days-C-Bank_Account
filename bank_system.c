#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Account {
    int acc_no;
    char name[100];
    float balance;
};

// TODO: Save accounts to file
void createAccount() {
    struct Account a;
    printf("Enter account number, name, and initial balance: ");
    scanf("%d %s %f", &a.acc_no, a.name, &a.balance);

    // Currently just prints the account info
    printf("Account Created: %d, Name: %s, Balance: %.2f\n", a.acc_no, a.name, a.balance);
}

// TODO: Withdraw amount from account
void withdraw() {
    printf("Withdraw function is not implemented yet.\n");
}

// TODO: Save deposit to file
void deposit() {
    printf("Deposit function is not implemented yet.\n");
}

int main() {
    int choice;
    do {
        printf("\n=== Bank Account Management ===\n");
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while(choice != 4);
    return 0;
}