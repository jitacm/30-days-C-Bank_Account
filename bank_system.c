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
#endif

 
#define RED    "\x1b[31m"
#define GREEN  "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE   "\x1b[34m"
#define RESET  "\x1b[0m"

// ------- STRUCT DEFINITION -------
struct Account {

#define RED    "\x1b[31m"
#define GREEN  "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE   "\x1b[34m"
#define RESET  "\x1b[0m"

#define SALT_SIZE 16
#define HASH_SIZE 32

// --- SHA-256 Implementation (public domain) ---
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;              // 32-bit word

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


// ------- UTILITY FUNCTIONS -------
int accountExists(int acc_no) {

void generateSalt(unsigned char *salt, size_t length);
void hashPin(const char *pin, const unsigned char *salt, size_t salt_len, unsigned char *out_hash);
int accountExists(int acc_no);
int authenticate(int acc_no, const char *pin_input);
void createAccount();
void deposit();
void withdraw();
void viewAccounts();
void searchAccount();
void updateAccountName();
void deleteAccount();
void printHex(const unsigned char *data, size_t len);
void flush_stdin(void);
void getMaskedInput(char *buffer, size_t size);

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

    // Read the first record
    while (fread(&a, sizeof(struct Account), 1, fp) == 1) {
        if (a.acc_no == acc_no) {
            found = 1;
            pos = ftell(fp) - sizeof(struct Account); // Position of the beginning of this record
            break;
        }
    }

    if (!found) {
        fclose(fp);
        return 0;
    }

    // Check if account is locked
    if (a.locked) {
        fclose(fp);
        return 0;
    }

    // Compute hash for the provided PIN
    unsigned char input_hash[HASH_SIZE];
    hashPin(pin_input, a.salt, SALT_SIZE, input_hash);

    if (memcmp(input_hash, a.pin_hash, HASH_SIZE) == 0) {
        // Correct PIN: reset failed attempts
        a.failed_attempts = 0;
        fseek(fp, pos, SEEK_SET);
        fwrite(&a, sizeof(struct Account), 1, fp);
        fflush(fp);
        fclose(fp);
        return 1;
    } else {
        // Incorrect PIN
        a.failed_attempts++;
        if (a.failed_attempts >= 3) {
            a.locked = 1;
        }
        fseek(fp, pos, SEEK_SET);
        fwrite(&a, sizeof(struct Account), 1, fp);
        fflush(fp);
        fclose(fp);
        return 0;
    }
}


// ------- CORE FEATURES -------


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

    // Get PIN masked input
    printf(GREEN "Set a 4-digit PIN: " RESET);

    getMaskedInput(pin_str, sizeof(pin_str));
    if (strlen(pin_str) != 4 || strspn(pin_str, "0123456789") != 4)
    {
        printf(RED "Invalid PIN. Must be exactly 4 digits.\n" RESET);

        return;
    }

    // Generate salt and hash PIN
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

    while ((ch = getchar()) != '\n' && ch != EOF); // flush newline
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


// ******** END OF TRANSFER FEATURE ********

// ------- REMAINING FEATURES -------


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


void searchAccount()
{
    int acc_no;
    char pin_str[32];
    int ch;


    printf(GREEN "Enter account number to search: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);

        int ch; 


        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    printf(GREEN "Enter PIN for authentication: " RESET);

    while ((ch = getchar()) != '\n' && ch != EOF);
    getMaskedInput(pin_str, sizeof(pin_str));

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
            unsigned char input_hash[HASH_SIZE];
            hashPin(pin_str, a.salt, SALT_SIZE, input_hash);
            if (memcmp(input_hash, a.pin_hash, HASH_SIZE) == 0)
            {
                found = 1;
                printf(BLUE "\nAccount Details:\n" RESET);
                printf(YELLOW "Account No: %d\nName: %s\nBalance: %.2f\n" RESET, a.acc_no, a.name, a.balance);
                break;
            }
            else
            {
                printf(RED "PIN incorrect.\n" RESET);
                break;
            }

        }
    }
    fclose(fp);
    if (!found) {
        printf(RED "Account not found or PIN incorrect.\n" RESET);
    }
}


void updateAccountName()
{
    int acc_no;
    char pin_str[32];

    char newName[100];
    int found = 0;
    int ch;

    printf(GREEN "Enter account number to update: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);

        int ch; 


        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    printf(GREEN "Enter PIN for authentication: " RESET);

    while ((ch = getchar()) != '\n' && ch != EOF);
    getMaskedInput(pin_str, sizeof(pin_str));

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
            unsigned char input_hash[HASH_SIZE];
            hashPin(pin_str, a.salt, SALT_SIZE, input_hash);
            if (memcmp(input_hash, a.pin_hash, HASH_SIZE) == 0)
            {
                found = 1;
                strcpy(a.name, newName);
                fseek(fp, -sizeof(struct Account), SEEK_CUR);
                fwrite(&a, sizeof(struct Account), 1, fp);
                printf(GREEN "Account holder's name updated successfully.\n" RESET);
                break;
            }
            else
            {
                printf(RED "Authentication failed. Wrong PIN.\n" RESET);
                break;
            }

        }
    }
    fclose(fp);
    if (!found) {
        printf(RED "Account not found or authentication failed.\n" RESET);
    }
}


void deleteAccount()
{
    int acc_no;
    char pin_str[32];
    int found = 0;
    int ch;


    printf(GREEN "Enter account number to delete: " RESET);
    if (scanf("%d", &acc_no) != 1) {
        printf(RED "Invalid account number input.\n" RESET);

        int ch; 
      while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    printf(GREEN "Enter PIN for authentication: " RESET);
    while ((ch = getchar()) != '\n' && ch != EOF);
    getMaskedInput(pin_str, sizeof(pin_str));


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
            unsigned char input_hash[HASH_SIZE];
            hashPin(pin_str, a.salt, SALT_SIZE, input_hash);
            if (memcmp(input_hash, a.pin_hash, HASH_SIZE) == 0)
            {
                found = 1;
                /* skip writing this account => it will be deleted */
                continue;
            }
            else
            {
                /* PIN incorrect: write the record back and stop deletion */
                fwrite(&a, sizeof(struct Account), 1, temp);
            }
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
        /* replace file */
        remove("accounts.dat");
        rename("temp.dat", "accounts.dat");
        printf(GREEN "Account %d deleted successfully.\n" RESET, acc_no);
    }
    else
    {
        remove("temp.dat");
        printf(RED "Account not found or PIN incorrect. No deletion performed.\n" RESET);
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


int main()
{
    srand((unsigned int)time(NULL));

    while (1)
    {
        int choice;
        printf("\n" BLUE "=== CLI Banking System (Instance 2 — Features 1+2) ===\n" RESET);
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. View All Accounts\n");
        printf("5. Search Account\n");
        printf("6. Update Account Holder Name\n");
        printf("7. Delete Account\n");
        printf("8. Exit\n");
        printf("Choose an option: ");
        if (scanf("%d", &choice) != 1)
        {
            printf(RED "Invalid input.\n" RESET);
            flush_stdin();
            continue;
        }

        switch (choice)
        {

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

                viewAccounts();
                break;
            case 5:
                searchAccount();
                break;
            case 6:
                updateAccountName();
                break;
            case 7:
                deleteAccount();
                break;
            case 8:
                printf(YELLOW "Exiting.\n" RESET);
                exit(0);
            default:
                printf(RED "Invalid choice.\n" RESET);
        }
    }


    return 0;
}
