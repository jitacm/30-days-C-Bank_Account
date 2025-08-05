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

    FILE *fp = fopen("accounts.dat", "ab");
    if (!fp) {
        printf("Error opening accounts file.\n");
        return;
    }
    fwrite(&a, sizeof(struct Account), 1, fp);
    fclose(fp);

    printf("Account Created: %d, Name: %s, Balance: %.2f\n", a.acc_no, a.name, a.balance);
}

// TODO: Save deposit to file
void deposit() {
    int acc_no;
    float amount;
    int found = 0;

    printf("Enter account number: ");
    scanf("%d", &acc_no);
    printf("Enter amount to deposit: ");
    scanf("%f", &amount);

    FILE *fp = fopen("accounts.dat", "rb+");
    if (!fp) {
        printf("No accounts found.\n");
        return;
    }

    struct Account a;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no == acc_no) {
            found = 1;
            a.balance += amount;
            fseek(fp, -sizeof(struct Account), SEEK_CUR);
            fwrite(&a, sizeof(struct Account), 1, fp);
            printf("Deposit successful. New balance: %.2f\n", a.balance);
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Account not found.\n");
    }
}


void withdraw() {
    int acc_no;
    float amount;
    int found = 0;

    printf("Enter account number: ");
    scanf("%d", &acc_no);
    printf("Enter amount to withdraw: ");
    scanf("%f", &amount);

    FILE *fp = fopen("accounts.dat", "rb+");
    if (!fp) {
        printf("No accounts found.\n");
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
                printf("Withdraw successful. New balance: %.2f\n", a.balance);
            } else {
                printf("Insufficient balance.\n");
            }
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Account not found.\n");
    }
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