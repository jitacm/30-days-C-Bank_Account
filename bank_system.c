#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
#include <conio.h>
#define CLEAR "cls"
#else
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#define RED    "\x1b[31m"
#define GREEN  "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE   "\x1b[34m"
#define RESET  "\x1b[0m"

#define SALT_SIZE 16
#define HASH_SIZE 32

#define ADMIN_FILE "admin.dat"
#define ADMIN_MAGIC "ADM1v1"
#define ADMIN_MAGIC_LEN 6

// --- SHA-256 Implementation (public domain) ---
typedef unsigned char BYTE;         // 8-bit byte
typedef unsigned int  WORD;         // 32-bit word

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

typedef struct {
    BYTE data[64];
    WORD datalen;
    unsigned long long bitlen;
    WORD state[8];
} SHA256_CTX;

void sha256_transform(SHA256_CTX *ctx, const BYTE data[])
{
    WORD a,b,c,d,e,f,g,h,i,j,t1,t2,m[64];

    static const WORD k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
        0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
        0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
        0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
        0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
        0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    for (i=0,j=0; i < 16; ++i, j += 4)
        m[i] = (data[j] << 24) | (data[j+1] << 16) | (data[j+2] << 8) | (data[j+3]);
    for ( ; i < 64; ++i)
        m[i] = SIG1(m[i-2]) + m[i-7] + SIG0(m[i-15]) + m[i-16];

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (i=0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void sha256_init(SHA256_CTX *ctx)
{
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len)
{
    WORD i;

    for (i=0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == 64) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

void sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
    WORD i;

    i = ctx->datalen;

    // Pad whatever data is left in the buffer.
    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        while (i < 56)
            ctx->data[i++] = 0x00;
    }
    else {
        ctx->data[i++] = 0x80;
        while (i < 64)
            ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data);
        memset(ctx->data, 0, 56);
    }

    // Append to the padding the total message's length in bits and transform.
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = ctx->bitlen;
    ctx->data[62] = ctx->bitlen >> 8;
    ctx->data[61] = ctx->bitlen >> 16;
    ctx->data[60] = ctx->bitlen >> 24;
    ctx->data[59] = ctx->bitlen >> 32;
    ctx->data[58] = ctx->bitlen >> 40;
    ctx->data[57] = ctx->bitlen >> 48;
    ctx->data[56] = ctx->bitlen >> 56;
    sha256_transform(ctx, ctx->data);

    // Since this implementation uses little endian byte ordering and SHA uses big endian,
    // reverse all the bytes when copying the final state to the output hash.
    for (i=0; i < 4; ++i) {
        hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}

void sha256(const BYTE *data, size_t len, BYTE *out_hash)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, out_hash);
}
// --- End SHA-256 Implementation ---

struct Account
{
    int acc_no;
    char name[100];
    float balance;
    unsigned char pin_hash[HASH_SIZE];
    unsigned char salt[SALT_SIZE];
    int failed_attempts;
    int locked;
};

typedef struct {
    char magic[8];
    unsigned char salt[SALT_SIZE];
    unsigned char pin_hash[HASH_SIZE];
} AdminRecord;

// ------- UTILITY FUNCTIONS -------
void generateSalt(unsigned char *salt, size_t length);
void hashPin(const char *pin, const unsigned char *salt, size_t salt_len, unsigned char *out_hash);
int accountExists(int acc_no);
int authenticate(int acc_no, const char *pin_input);
int authenticateAdmin(const char *pin_input);
void createAccount();
void deposit();
void withdraw();
void transferMoney();
void viewAccounts();
void searchAccount(int show_pin); // Modified function signature
void updateAccountName();
void deleteAccount();
void printHex(const unsigned char *data, size_t len);
void flush_stdin(void);
void getMaskedInput(char *buffer, size_t size);

// Admin credential helpers
int loadAdminCredentials(unsigned char *salt, unsigned char *hash);
int saveAdminCredentials(const unsigned char *salt, const unsigned char *hash);
int setAdminPinInteractive(void);
int adminInitIfNeeded(void);

// New Feature: User-Friendly Account Statement
void generateAccountStatement();

// New Menu functions
void userMenu();
void adminMenu();


void generateSalt(unsigned char *salt, size_t length) {
    for (size_t i = 0; i < length; i++) {
        salt[i] = (unsigned char)(rand() % 256);
    }
}

void hashPin(const char *pin, const unsigned char *salt, size_t salt_len, unsigned char *out_hash) {
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, salt, salt_len);
    sha256_update(&ctx, (const BYTE *)pin, strlen(pin));
    sha256_final(&ctx, out_hash);
}

int accountExists(int acc_no)
{
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

int authenticate(int acc_no, const char *pin_input) {
    FILE *fp = fopen("accounts.dat", "rb+");
    if (!fp) {
        return 0;
    }

    struct Account a;
    int found = 0;
    long pos = 0;

    while (fread(&a, sizeof(struct Account), 1, fp) == 1) {
        if (a.acc_no == acc_no) {
            found = 1;
            pos = ftell(fp) - sizeof(struct Account);
            break;
        }
    }

    if (!found) {
        fclose(fp);
        return 0;
    }

    if (a.locked) {
        fclose(fp);
        return 0;
    }

    unsigned char input_hash[HASH_SIZE];
    hashPin(pin_input, a.salt, SALT_SIZE, input_hash);

    if (memcmp(input_hash, a.pin_hash, HASH_SIZE) == 0) {
        a.failed_attempts = 0;
        fseek(fp, pos, SEEK_SET);
        fwrite(&a, sizeof(struct Account), 1, fp);
        fflush(fp);
        fclose(fp);
        return 1;
    } else {
        a.failed_attempts++;
        if (a.failed_attempts >= 3) {
            a.locked = 1;
            printf(RED "\nToo many failed attempts. Account has been locked.\n" RESET);
        }
        fseek(fp, pos, SEEK_SET);
        fwrite(&a, sizeof(struct Account), 1, fp);
        fflush(fp);
        fclose(fp);
        return 0;
    }
}

// --- Admin credentials (secure storage) ---
int loadAdminCredentials(unsigned char *salt, unsigned char *hash) {
    FILE *fp = fopen(ADMIN_FILE, "rb");
    if (!fp) return 0;
    AdminRecord rec;
    size_t r = fread(&rec, sizeof(AdminRecord), 1, fp);
    fclose(fp);
    if (r != 1) return 0;
    if (memcmp(rec.magic, ADMIN_MAGIC, ADMIN_MAGIC_LEN) != 0) return 0;
    memcpy(salt, rec.salt, SALT_SIZE);
    memcpy(hash, rec.pin_hash, HASH_SIZE);
    return 1;
}

int saveAdminCredentials(const unsigned char *salt, const unsigned char *hash) {
    FILE *fp = fopen(ADMIN_FILE, "wb");
    if (!fp) return 0;
    AdminRecord rec;
    memset(&rec, 0, sizeof(rec));
    memcpy(rec.magic, ADMIN_MAGIC, ADMIN_MAGIC_LEN);
    memcpy(rec.salt, salt, SALT_SIZE);
    memcpy(rec.pin_hash, hash, HASH_SIZE);
    size_t w = fwrite(&rec, sizeof(AdminRecord), 1, fp);
    fclose(fp);
#ifndef _WIN32
    // Restrict permissions on Unix-like systems: owner read/write only
    chmod(ADMIN_FILE, S_IRUSR | S_IWUSR);
#endif
    return w == 1;
}

int setAdminPinInteractive(void) {
    char pin1[64], pin2[64];

    while (1) {
        printf(YELLOW "\nNo admin PIN configured.\n" RESET);
        printf(GREEN "Set admin PIN (4-12 digits): " RESET);
        getMaskedInput(pin1, sizeof(pin1));
        size_t n = strlen(pin1);
        if (n < 4 || n > 12 || strspn(pin1, "0123456789") != n) {
            printf(RED "Invalid PIN. Must be 4-12 digits.\n" RESET);
            continue;
        }

        printf(GREEN "Confirm admin PIN: " RESET);
        getMaskedInput(pin2, sizeof(pin2));
        if (strcmp(pin1, pin2) != 0) {
            printf(RED "PINs do not match. Try again.\n" RESET);
            continue;
        }

        unsigned char salt[SALT_SIZE];
        unsigned char hash[HASH_SIZE];
        generateSalt(salt, SALT_SIZE);
        hashPin(pin1, salt, SALT_SIZE, hash);

        memset(pin1, 0, sizeof(pin1));
        memset(pin2, 0, sizeof(pin2));

        if (!saveAdminCredentials(salt, hash)) {
            printf(RED "Failed to save admin PIN. Please try again.\n" RESET);
            return 0;
        }

        printf(GREEN "Admin PIN set successfully.\n" RESET);
        return 1;
    }
}

int adminInitIfNeeded(void) {
    unsigned char salt[SALT_SIZE], hash[HASH_SIZE];
    if (loadAdminCredentials(salt, hash)) return 1;
    return setAdminPinInteractive();
}

int authenticateAdmin(const char *pin_input) {
    unsigned char salt[SALT_SIZE], stored_hash[HASH_SIZE], input_hash[HASH_SIZE];
    if (!loadAdminCredentials(salt, stored_hash)) {
        return 0;
    }
    hashPin(pin_input, salt, SALT_SIZE, input_hash);
    return memcmp(input_hash, stored_hash, HASH_SIZE) == 0;
}


void createAccount()
{
    struct Account a;
    char pin_str[32];
    int ch;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &a.acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    if (accountExists(a.acc_no)) {
        printf(RED "Account number already exists. Choose another.\n" RESET);
        return;
    }
    printf(GREEN "Enter account holder name: " RESET);

    while ((ch = getchar()) != '\n' && ch != EOF);
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
    getMaskedInput(pin_str, sizeof(pin_str));
    if (strlen(pin_str) != 4 || strspn(pin_str, "0123456789") != 4)
    {
        printf(RED "Invalid PIN. Must be exactly 4 digits.\n" RESET);
        return;
    }

    generateSalt(a.salt, SALT_SIZE);
    hashPin(pin_str, a.salt, SALT_SIZE, a.pin_hash);

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

    a.failed_attempts = 0;
    a.locked = 0;

    FILE *fp = fopen("accounts.dat", "ab");
    if (!fp) {
        printf(RED "Error opening accounts file.\n" RESET);
        return;
    }
    fwrite(&a, sizeof(struct Account), 1, fp);
    fclose(fp);

    printf(GREEN "Account Created: %d, Name: %s, Balance: %.2f\n" RESET, a.acc_no, a.name, a.balance);
}

void deposit()
{
    int acc_no;
    char pin_str[32];
    float amount;
    int found = 0;
    int ch;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter your PIN: " RESET);

    while ((ch = getchar()) != '\n' && ch != EOF);
    getMaskedInput(pin_str, sizeof(pin_str));

    if (!authenticate(acc_no, pin_str))
    {
        printf(RED "Authentication failed. Wrong account or PIN.\n" RESET);
        return;
    }
    printf(GREEN "Enter amount to deposit: " RESET);
    if (scanf("%f", &amount) != 1) {
        printf(RED "Invalid input format.\n" RESET);
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

void withdraw()
{
    int acc_no;
    char pin_str[32];
    float amount;
    int found = 0;
    int ch;

    printf(GREEN "Enter account number: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter your PIN: " RESET);

    while ((ch = getchar()) != '\n' && ch != EOF);
    getMaskedInput(pin_str, sizeof(pin_str));

    if (!authenticate(acc_no, pin_str))
    {
        printf(RED "Authentication failed. Wrong account or PIN.\n" RESET);
        return;
    }
    printf(GREEN "Enter amount to withdraw: " RESET);
    if (scanf("%f", &amount) != 1) {
        printf(RED "Invalid input format.\n" RESET);
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

void transferMoney() {
    FILE *fp;
    struct Account sender, receiver;
    int senderAcc, receiverAcc;
    char senderPin_str[32];
    float amount;
    int foundSender = 0, foundReceiver = 0;
    long senderPos, receiverPos;

    printf("\n" GREEN "--- Money Transfer ---\n" RESET);
    printf("Enter Sender Account Number: ");
    if (scanf("%d", &senderAcc) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf("Enter Sender PIN: ");
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF); // Flush newline before masked input
    getMaskedInput(senderPin_str, sizeof(senderPin_str));

    // Authenticate sender
    if (!authenticate(senderAcc, senderPin_str)) {
        printf(RED "Authentication failed. Sender account not found or invalid PIN.\n" RESET);
        return;
    }

    fp = fopen("accounts.dat", "rb+");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }

    // Find sender to get their details
    rewind(fp);
    while (fread(&sender, sizeof(struct Account), 1, fp) == 1) {
        if (sender.acc_no == senderAcc) {
            foundSender = 1;
            senderPos = ftell(fp) - sizeof(struct Account);
            break;
        }
    }

    printf("Enter Receiver Account Number: ");
    if (scanf("%d", &receiverAcc) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        fclose(fp);
        return;
    }
    if (senderAcc == receiverAcc) {
        printf(RED "Cannot transfer money to the same account.\n" RESET);
        fclose(fp);
        return;
    }

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
    if (scanf("%f", &amount) != 1) {
        printf(RED "Invalid input format.\n" RESET);
        fclose(fp);
        return;
    }

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

void viewAccounts()
{
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

// searchAccount now does not require a PIN since it's an admin function
void searchAccount(int show_pin)
{
    int acc_no;
    int ch;
    
    printf(GREEN "Enter account number to search: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }
    struct Account a;
    int found = 0;
    while (fread(&a, sizeof(struct Account), 1, fp))
    {
        if (a.acc_no == acc_no)
        {
            found = 1;
            printf(BLUE "\nAccount Details:\n" RESET);
            printf(YELLOW "Account No: %d\n" RESET, a.acc_no);
            printf(YELLOW "Name: %s\n" RESET, a.name);
            printf(YELLOW "Balance: %.2f\n" RESET, a.balance);
            if (show_pin) {
                 printf(YELLOW "Pin Hash: " RESET);
                 printHex(a.pin_hash, HASH_SIZE);
            }
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf(RED "Account not found.\n" RESET);
    }
}

// updateAccountName now does not require an internal admin check
void updateAccountName()
{
    int acc_no;
    char newName[100];
    int found = 0;
    int ch;

    printf(GREEN "Enter account number to update: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    printf(GREEN "Enter new account holder name: " RESET);
    while ((ch = getchar()) != '\n' && ch != EOF);
    if (!fgets(newName, sizeof(newName), stdin))
    {
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
    while (fread(&a, sizeof(struct Account), 1, fp))
    {
        if (a.acc_no == acc_no)
        {
            found = 1;
            strcpy(a.name, newName);
            fseek(fp, -sizeof(struct Account), SEEK_CUR);
            fwrite(&a, sizeof(struct Account), 1, fp);
            printf(GREEN "Account holder's name updated successfully.\n" RESET);
            printf(YELLOW "Account %d name changed to: %s\n" RESET, acc_no, newName);
            break;
        }
    }
    fclose(fp);
    
    if (!found) {
        printf(RED "Account not found.\n" RESET);
    }
}

// deleteAccount now does not require an internal admin check
void deleteAccount()
{
    int acc_no;
    int found = 0;
    int ch;

    printf(GREEN "Enter account number to delete: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printf(RED "No accounts found.\n" RESET);
        return;
    }
    
    FILE *temp = fopen("temp.dat", "wb");
    if (!temp)
    {
        fclose(fp);
        printf(RED "Error creating temporary file.\n" RESET);
        return;
    }
    
    struct Account a;
    while (fread(&a, sizeof(struct Account), 1, fp))
    {
        if (a.acc_no == acc_no)
        {
            found = 1;
            // Skip writing this account => it will be deleted
            printf(YELLOW "Deleting account: %d, Name: %s, Balance: %.2f\n" RESET, 
                       a.acc_no, a.name, a.balance);
            continue;
        }
        else
        {
            fwrite(&a, sizeof(struct Account), 1, temp);
        }
    }
    fclose(fp);
    fclose(temp);

    if (found)
    {
        remove("accounts.dat");
        rename("temp.dat", "accounts.dat");
        printf(GREEN "Account %d deleted successfully.\n" RESET, acc_no);
    }
    else
    {
        remove("temp.dat");
        printf(RED "Account not found.\n" RESET);
    }
}

void printHex(const unsigned char *data, size_t len)
{
    for (size_t i = 0; i < len; ++i)
        printf("%02x", data[i]);
    printf("\n");
}

void flush_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Cross-platform masked PIN input:
 * - On Windows uses _getch()
 * - On Unix uses termios to disable echo
 * Accepts backspace. Stops on Enter. Writes null-terminated string.
 */
void getMaskedInput(char *buffer, size_t size)
{
    if (size == 0) return;
    size_t idx = 0;
#ifdef _WIN32
    int ch;
    while ((ch = _getch()) != '\r' && ch != EOF) {
        if (ch == '\b' || ch == 127) { // backspace
            if (idx > 0) {
                idx--;
                printf("\b \b");
            }
        } else if (ch >= ' ' && ch <= '~' && idx + 1 < size) {
            buffer[idx++] = (char)ch;
            printf("*");
        }
    }
    buffer[idx] = '\0';
    printf("\n");
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        if (ch == 127 || ch == '\b') { // backspace
            if (idx > 0) {
                idx--;
                printf("\b \b");
            }
        } else if (ch >= ' ' && ch <= '~' && idx + 1 < size) {
            buffer[idx++] = (char)ch;
            printf("*");
        }
    }
    buffer[idx] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\n");
#endif
}

void generateAccountStatement() {
    int acc_no;
    char pin_str[32];
    int found = 0;
    int ch;
    
    printf(GREEN "Enter account number for statement: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    
    printf(GREEN "Enter PIN for authentication: " RESET);
    while ((ch = getchar()) != '\n' && ch != EOF);
    getMaskedInput(pin_str, sizeof(pin_str));
    
    if (!authenticate(acc_no, pin_str)) {
        printf(RED "Authentication failed. Account not found or PIN incorrect.\n" RESET);
        return;
    }
    
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printf(YELLOW "No accounts found.\n" RESET);
        return;
    }
    
    struct Account a;
    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (a.acc_no == acc_no) {
            found = 1;
            printf(BLUE "\n============================================\n" RESET);
            printf(BLUE "         BANK ACCOUNT STATEMENT             \n" RESET);
            printf(BLUE "============================================\n" RESET);
            printf(YELLOW "Account Number:  %d\n" RESET, a.acc_no);
            printf(YELLOW "Account Holder:  %s\n" RESET, a.name);
            printf(YELLOW "Current Balance: %.2f\n" RESET, a.balance);
            printf(BLUE "============================================\n" RESET);
            break;
        }
    }
    fclose(fp);
    
    if (!found) {
        printf(RED "Account not found or PIN incorrect.\n" RESET);
    }
}

// ------- NEW MENU FUNCTIONS -------
void userMenu() {
    int choice;
    do {
        system(CLEAR);
        printf(BLUE "\n=== User Menu ===\n" RESET);
        printf(YELLOW "1. Create Account\n" RESET);
        printf(YELLOW "2. Deposit\n" RESET);
        printf(YELLOW "3. Withdraw\n" RESET);
        printf(YELLOW "4. Transfer Money\n" RESET);
        printf(YELLOW "5. Generate Account Statement\n" RESET); 
        printf(YELLOW "6. Exit to Main Menu\n" RESET);
        printf(GREEN "Enter your choice: " RESET);

        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input!\n" RESET);
            flush_stdin();
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
                transferMoney();
                break;
            case 5:
                generateAccountStatement();
                break;
            case 6:
                printf(GREEN "Exiting user menu...\n" RESET);
                break;
            default:
                printf(RED "Invalid choice!\n" RESET);
        }
        if (choice != 6) {
            printf(YELLOW "\nPress Enter to continue..." RESET);
            flush_stdin();
            getchar();
        }
    } while (choice != 6);
}

void adminMenu() {
    int choice;
    do {
        system(CLEAR);
        printf(BLUE "\n=== Admin Menu ===\n" RESET);
        printf(YELLOW "1. View All Accounts\n" RESET);
        printf(YELLOW "2. Search Account by Number\n" RESET);
        printf(YELLOW "3. Update Account Holder Name\n" RESET);
        printf(YELLOW "4. Delete Account\n" RESET);
        printf(YELLOW "5. Exit to Main Menu\n" RESET);
        printf(GREEN "Enter your choice: " RESET);

        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input!\n" RESET);
            flush_stdin();
            continue;
        }

        switch (choice) {
            case 1:
                viewAccounts();
                break;
            case 2:
                searchAccount(0); // Admin search, no PIN needed. '0' for show_pin
                break;
            case 3:
                updateAccountName();
                break;
            case 4:
                deleteAccount();
                break;
            case 5:
                printf(GREEN "Exiting admin menu...\n" RESET);
                break;
            default:
                printf(RED "Invalid choice!\n" RESET);
        }
        if (choice != 5) {
            printf(YELLOW "\nPress Enter to continue..." RESET);
            flush_stdin();
            getchar();
        }
    } while (choice != 5);
}

// ------- MAIN MENU -------
int main()
{
    srand((unsigned int)time(NULL));

    if (!adminInitIfNeeded()) {
        printf(RED "Failed to initialize admin credentials. Exiting.\n" RESET);
        return 1;
    }

    int choice;
    char admin_pin[32];
    
    do {
        system(CLEAR);
        printf(BLUE "\n=== Bank Account Management ===\n" RESET);
        printf(YELLOW "1. User Login\n" RESET);
        printf(YELLOW "2. Admin Login\n" RESET);
        printf(YELLOW "3. Exit\n" RESET);
        printf(GREEN "Enter your choice: " RESET);

        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input!\n" RESET);
            flush_stdin();
            continue;
        }

        switch (choice) {
            case 1:
                userMenu();
                break;
            case 2:
                printf(GREEN "Enter Admin PIN: " RESET);
                flush_stdin();
                getMaskedInput(admin_pin, sizeof(admin_pin));
                
                if (authenticateAdmin(admin_pin)) {
                    printf(GREEN "Admin login successful.\n" RESET);
                    adminMenu();
                } else {
                    printf(RED "Incorrect passcode. Access denied.\n" RESET);
                }
                break;
            case 3:
                printf(GREEN "Exiting program...\n" RESET);
                break;
            default:
                printf(RED "Invalid choice!\n" RESET);
        }
        if (choice != 3) {
            printf(YELLOW "\nPress Enter to continue..." RESET);
            flush_stdin();
            getchar();
        }
    } while (choice != 3);

    return 0;
}
