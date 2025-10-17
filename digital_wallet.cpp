#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
#else
    #include <dirent.h>
    #include <sys/stat.h>
#endif

using namespace std;

// Simple utility functions
class Utils {
public:
    static string getCurrentTimestamp() {
        time_t now = time(0);
        char* timeStr = ctime(&now);
        string result(timeStr);
        result.pop_back(); // Remove newline
        return result;
    }

    static string formatCurrency(double amount) {
        return "Rs." + to_string((int)amount) + "." +
               (((int)(amount * 100)) % 100 < 10 ? "0" : "") +
               to_string(((int)(amount * 100)) % 100);
    }

    static void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    static void pause() {
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    static int generateOTP() {
        return 1000 + (rand() % 9000); // 4-digit OTP
    }

    // Get list of files in current directory
    static vector<string> listFiles(string extension = "") {
        vector<string> files;

        #ifdef _WIN32
            WIN32_FIND_DATA findFileData;
            HANDLE hFind = FindFirstFile("*.*", &findFileData);

            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    string filename = findFileData.cFileName;
                    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        if (extension.empty() || filename.find(extension) != string::npos) {
                            files.push_back(filename);
                        }
                    }
                } while (FindNextFile(hFind, &findFileData) != 0);
                FindClose(hFind);
            }
        #else
            DIR* dir = opendir(".");
            if (dir) {
                struct dirent* entry;
                while ((entry = readdir(dir)) != nullptr) {
                    string filename = entry->d_name;
                    if (entry->d_type == DT_REG) { // Regular file
                        if (extension.empty() || filename.find(extension) != string::npos) {
                            files.push_back(filename);
                        }
                    }
                }
                closedir(dir);
            }
        #endif

        return files;
    }
};

// Transaction structure
struct Transaction {
    string id;
    string type;
    double amount;
    string description;
    string timestamp;
    double balanceAfter;

    Transaction(string t, double amt, string desc, double bal)
        : type(t), amount(amt), description(desc), balanceAfter(bal) {
        timestamp = Utils::getCurrentTimestamp();
        id = "TXN" + to_string(rand() % 10000);
    }
};

// Account class
class Account {
private:
    string accountId;
    string userId;
    double balance;
    vector<Transaction> transactions;

public:
    Account(string uid) : userId(uid), balance(0.0) {
        accountId = uid + "_ACC" + to_string(rand() % 1000);
    }

    string getAccountId() const { return accountId; }
    double getBalance() const { return balance; }
    string getUserId() const { return userId; }

    void setAccountId(string id) { accountId = id; }

    bool deposit(double amount, string description = "Deposit") {
        if (amount <= 0) return false;

        balance += amount;
        transactions.push_back(Transaction("DEPOSIT", amount, description, balance));
        return true;
    }

    bool withdraw(double amount, string description = "Withdrawal") {
        if (amount <= 0 || amount > balance) return false;

        balance -= amount;
        transactions.push_back(Transaction("WITHDRAWAL", amount, description, balance));
        return true;
    }

    bool transfer(double amount, string toAccount) {
        if (amount <= 0 || amount > balance) return false;

        balance -= amount;
        transactions.push_back(Transaction("TRANSFER_OUT", amount,
                                         "Transfer to " + toAccount, balance));
        return true;
    }

    bool receiveTransfer(double amount, string fromAccount) {
        if (amount <= 0) return false;

        balance += amount;
        transactions.push_back(Transaction("TRANSFER_IN", amount,
                                         "Transfer from " + fromAccount, balance));
        return true;
    }

    void showTransactions(int limit = 10) const {
        cout << "\n[Recent Transactions]\n";
        cout << string(70, '-') << "\n";
        cout << left << setw(12) << "Type" << setw(10) << "Amount"
             << setw(25) << "Description" << setw(15) << "Balance\n";
        cout << string(70, '-') << "\n";

        int count = 0;
        for (auto it = transactions.rbegin(); it != transactions.rend() && count < limit; ++it, ++count) {
            cout << left << setw(12) << it->type
                 << setw(10) << Utils::formatCurrency(it->amount)
                 << setw(25) << it->description.substr(0, 23)
                 << setw(15) << Utils::formatCurrency(it->balanceAfter) << "\n";
        }

        if (transactions.empty()) {
            cout << "No transactions found.\n";
        }
    }

    void saveToFile() const {
        string filename = accountId + ".txt";
        ofstream file(filename);
        if (file.is_open()) {
            file << accountId << "\n" << userId << "\n" << balance << "\n";
            file << transactions.size() << "\n";
            for (const auto& txn : transactions) {
                file << txn.id << "|" << txn.type << "|" << txn.amount << "|"
                     << txn.description << "|" << txn.timestamp << "|"
                     << txn.balanceAfter << "\n";
            }
            file.close();
        }
    }

    bool loadFromFile(string filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        string line;
        getline(file, accountId);
        getline(file, userId);
        file >> balance;

        int txnCount;
        file >> txnCount;
        file.ignore(); // Skip newline

        transactions.clear();
        for (int i = 0; i < txnCount; i++) {
            getline(file, line);
            // Parse transaction line
            size_t pos = 0;
            vector<string> parts;
            while ((pos = line.find('|')) != string::npos) {
                parts.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            parts.push_back(line);

            if (parts.size() >= 6) {
                Transaction txn(parts[1], stod(parts[2]), parts[3], stod(parts[5]));
                txn.id = parts[0];
                txn.timestamp = parts[4];
                transactions.push_back(txn);
            }
        }
        file.close();
        return true;
    }
};

// User class
class User {
private:
    string userId;
    string username;
    string password;
    string fullName;
    string phone;
    Account* account;

public:
    User(string uname, string pass, string name, string ph)
        : username(uname), password(pass), fullName(name), phone(ph) {
        userId = "USER" + to_string(rand() % 10000);
        account = new Account(userId);
    }

    ~User() {
        delete account;
    }

    // Getters
    string getUserId() const { return userId; }
    string getUsername() const { return username; }
    string getFullName() const { return fullName; }
    string getPhone() const { return phone; }
    Account* getAccount() const { return account; }

    bool verifyPassword(string pass) const {
        return password == pass;
    }

    void saveToFile() const {
        string filename = username + "_user.txt";
        ofstream file(filename);
        if (file.is_open()) {
            file << userId << "\n" << username << "\n" << password << "\n"
                 << fullName << "\n" << phone << "\n"
                 << account->getAccountId() << "\n";
            file.close();
        }
        account->saveToFile();
    }

    bool loadFromFile(string filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        string accountIdToLoad;
        getline(file, userId);
        getline(file, username);
        getline(file, password);
        getline(file, fullName);
        getline(file, phone);
        getline(file, accountIdToLoad);
        file.close();

        // Set the account ID before loading
        account->setAccountId(accountIdToLoad);

        // Load account data
        string accountFile = accountIdToLoad + ".txt";
        return account->loadFromFile(accountFile);
    }
};

// Main Wallet System
class WalletSystem {
private:
    vector<User*> users;
    User* currentUser;
    map<string, User*> userMap; // For quick lookup

public:
    WalletSystem() : currentUser(nullptr) {
        srand(time(0));
        loadUsersFromFiles();
    }

    ~WalletSystem() {
        saveAllUsers();
        for (User* user : users) {
            delete user;
        }
    }

    void run() {
        int choice;
        do {
            showMainMenu();
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1: userLogin(); break;
                case 2: userRegistration(); break;
                case 3: showAbout(); break;
                case 4: cout << "\nThank you for using Digital Wallet!\n"; break;
                default: cout << "\n[X] Invalid choice!\n";
            }

            if (choice != 4) Utils::pause();

        } while (choice != 4);
    }

private:
    void showMainMenu() {
        Utils::clearScreen();
        cout << "\n+===================================+\n";
        cout << "|       DIGITAL WALLET SYSTEM       |\n";
        cout << "|            Version 1.0            |\n";
        cout << "+===================================+\n\n";

        cout << "+-----------------------------------+\n";
        cout << "|           MAIN MENU               |\n";
        cout << "+-----------------------------------+\n";
        cout << "|  1. Login                         |\n";
        cout << "|  2. Register                      |\n";
        cout << "|  3. About                         |\n";
        cout << "|  4. Exit                          |\n";
        cout << "+-----------------------------------+\n\n";

        // Show number of loaded users (helpful for debugging)
        if (!users.empty()) {
            cout << "[Info] " << users.size() << " user(s) loaded from files.\n\n";
        }
    }

    void userLogin() {
        Utils::clearScreen();
        cout << "\n[USER LOGIN]\n";
        cout << "=============\n\n";

        string username, password;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        if (userMap.find(username) == userMap.end()) {
            cout << "\n[X] User not found!\n";
            return;
        }

        User* user = userMap[username];
        if (!user->verifyPassword(password)) {
            cout << "\n[X] Invalid password!\n";
            return;
        }

        currentUser = user;
        cout << "\n[SUCCESS] Login successful! Welcome, " << user->getFullName() << "!\n";
        Utils::pause();

        userDashboard();
    }

    void userRegistration() {
        Utils::clearScreen();
        cout << "\n[USER REGISTRATION]\n";
        cout << "====================\n\n";

        string username, password, fullName, phone;

        cout << "Username: ";
        cin >> username;

        if (userMap.find(username) != userMap.end()) {
            cout << "\n[X] Username already exists!\n";
            return;
        }

        cout << "Password: ";
        cin >> password;

        cin.ignore();
        cout << "Full Name: ";
        getline(cin, fullName);

        cout << "Phone: ";
        cin >> phone;

        User* newUser = new User(username, password, fullName, phone);
        users.push_back(newUser);
        userMap[username] = newUser;

        cout << "\n[SUCCESS] Registration successful!\n";
        cout << "Account ID: " << newUser->getAccount()->getAccountId() << "\n";

        newUser->saveToFile();
    }

    void userDashboard() {
        int choice;
        do {
            showUserDashboard();
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1: checkBalance(); break;
                case 2: depositMoney(); break;
                case 3: withdrawMoney(); break;
                case 4: transferMoney(); break;
                case 5: viewTransactions(); break;
                case 6:
                    cout << "\nLogging out...\n";
                    currentUser = nullptr;
                    break;
                default: cout << "\n[X] Invalid choice!\n";
            }

            if (choice != 6) Utils::pause();

        } while (choice != 6);
    }

    void showUserDashboard() {
        Utils::clearScreen();
        cout << "\n+======================================+\n";
        cout << "|          USER DASHBOARD              |\n";
        cout << "|  Welcome, " << left << setw(20) << currentUser->getFullName() << "     |\n";
        cout << "+======================================+\n\n";

        cout << "Balance: " << Utils::formatCurrency(currentUser->getAccount()->getBalance()) << "\n\n";

        cout << "+------------------------------------+\n";
        cout << "|            SERVICES                |\n";
        cout << "+------------------------------------+\n";
        cout << "|  1. Check Balance                  |\n";
        cout << "|  2. Deposit Money                  |\n";
        cout << "|  3. Withdraw Money                 |\n";
        cout << "|  4. Transfer Money                 |\n";
        cout << "|  5. View Transactions              |\n";
        cout << "|  6. Logout                         |\n";
        cout << "+------------------------------------+\n\n";
    }

    void checkBalance() {
        Utils::clearScreen();
        cout << "\n[ACCOUNT BALANCE]\n";
        cout << "=================\n\n";

        Account* acc = currentUser->getAccount();
        cout << "Account ID: " << acc->getAccountId() << "\n";
        cout << "Account Holder: " << currentUser->getFullName() << "\n";
        cout << "Current Balance: " << Utils::formatCurrency(acc->getBalance()) << "\n";
        cout << "Phone: " << currentUser->getPhone() << "\n";
    }

    void depositMoney() {
        Utils::clearScreen();
        cout << "\n[DEPOSIT MONEY]\n";
        cout << "===============\n\n";

        double amount;
        cout << "Enter amount to deposit: Rs.";
        cin >> amount;

        if (amount <= 0) {
            cout << "\n[X] Invalid amount!\n";
            return;
        }

        Account* acc = currentUser->getAccount();
        if (acc->deposit(amount)) {
            cout << "\n[SUCCESS] Deposit successful!\n";
            cout << "Amount: " << Utils::formatCurrency(amount) << "\n";
            cout << "New Balance: " << Utils::formatCurrency(acc->getBalance()) << "\n";

            currentUser->saveToFile();
        } else {
            cout << "\n[X] Deposit failed!\n";
        }
    }

    void withdrawMoney() {
        Utils::clearScreen();
        cout << "\n[WITHDRAW MONEY]\n";
        cout << "================\n\n";

        Account* acc = currentUser->getAccount();
        cout << "Current Balance: " << Utils::formatCurrency(acc->getBalance()) << "\n\n";

        double amount;
        cout << "Enter amount to withdraw: Rs.";
        cin >> amount;

        if (amount <= 0) {
            cout << "\n[X] Invalid amount!\n";
            return;
        }

        // Simple OTP verification
        int otp = Utils::generateOTP();
        cout << "\n[OTP] OTP sent to " << currentUser->getPhone() << ": " << otp << "\n";
        cout << "Enter OTP: ";
        int enteredOTP;
        cin >> enteredOTP;

        if (enteredOTP != otp) {
            cout << "\n[X] Invalid OTP!\n";
            return;
        }

        if (acc->withdraw(amount)) {
            cout << "\n[SUCCESS] Withdrawal successful!\n";
            cout << "Amount: " << Utils::formatCurrency(amount) << "\n";
            cout << "New Balance: " << Utils::formatCurrency(acc->getBalance()) << "\n";

            currentUser->saveToFile();
        } else {
            cout << "\n[X] Insufficient balance!\n";
        }
    }

    void transferMoney() {
        Utils::clearScreen();
        cout << "\n[TRANSFER MONEY]\n";
        cout << "================\n\n";

        Account* senderAcc = currentUser->getAccount();
        cout << "Your Balance: " << Utils::formatCurrency(senderAcc->getBalance()) << "\n\n";

        // Show available users
        cout << "Available Recipients:\n";
        vector<User*> recipients;
        for (User* user : users) {
            if (user != currentUser) {
                recipients.push_back(user);
                cout << recipients.size() << ". " << user->getFullName()
                     << " (" << user->getUsername() << ")\n";
            }
        }

        if (recipients.empty()) {
            cout << "\n[X] No recipients available!\n";
            return;
        }

        cout << "\nSelect recipient: ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice > recipients.size()) {
            cout << "\n[X] Invalid selection!\n";
            return;
        }

        User* recipient = recipients[choice - 1];
        Account* recipientAcc = recipient->getAccount();

        double amount;
        cout << "Enter amount to transfer: Rs.";
        cin >> amount;

        if (amount <= 0) {
            cout << "\n[X] Invalid amount!\n";
            return;
        }

        // OTP verification
        int otp = Utils::generateOTP();
        cout << "\n[OTP] OTP sent to " << currentUser->getPhone() << ": " << otp << "\n";
        cout << "Enter OTP: ";
        int enteredOTP;
        cin >> enteredOTP;

        if (enteredOTP != otp) {
            cout << "\n[X] Invalid OTP!\n";
            return;
        }

        // Perform transfer
        if (senderAcc->transfer(amount, recipientAcc->getAccountId()) &&
            recipientAcc->receiveTransfer(amount, senderAcc->getAccountId())) {

            cout << "\n[SUCCESS] Transfer successful!\n";
            cout << "Amount: " << Utils::formatCurrency(amount) << "\n";
            cout << "To: " << recipient->getFullName() << "\n";
            cout << "Your Balance: " << Utils::formatCurrency(senderAcc->getBalance()) << "\n";

            // Save both users
            currentUser->saveToFile();
            recipient->saveToFile();

            cout << "\n[SMS] SMS sent to " << recipient->getPhone()
                 << ": You received " << Utils::formatCurrency(amount)
                 << " from " << currentUser->getFullName() << "\n";

        } else {
            cout << "\n[X] Transfer failed! Check balance.\n";
        }
    }

    void viewTransactions() {
        Utils::clearScreen();
        cout << "\n[TRANSACTION HISTORY]\n";
        cout << "=====================\n\n";

        currentUser->getAccount()->showTransactions(15);
    }

    void showAbout() {
        Utils::clearScreen();
        cout << "\n+=======================================+\n";
        cout << "|         ABOUT THE SYSTEM              |\n";
        cout << "+=======================================+\n\n";

        cout << "Digital Wallet System v1.0\n\n";
        cout << "Features:\n";
        cout << "* User Registration & Login\n";
        cout << "* Account Balance Management\n";
        cout << "* Money Deposit & Withdrawal\n";
        cout << "* Fund Transfers\n";
        cout << "* Transaction History\n";
        cout << "* Basic OTP Security\n";
        cout << "* File-based Data Storage\n\n";

        cout << "Technology: C++ with Object-Oriented Design\n";
        cout << "Security: Basic OTP verification\n";
        cout << "Storage: Local file system\n\n";

        cout << "(c) 2024 Digital Wallet System\n";
    }

    void saveAllUsers() {
        for (User* user : users) {
            user->saveToFile();
        }
    }

    void loadUsersFromFiles() {
        // Find all user files in the current directory
        vector<string> files = Utils::listFiles("_user.txt");

        cout << "[System] Scanning for existing user files...\n";

        for (const string& filename : files) {
            // Extract username from filename (remove "_user.txt")
            string username = filename.substr(0, filename.find("_user.txt"));

            // Create a temporary user to load data
            User* tempUser = new User("temp", "temp", "temp", "temp");

            if (tempUser->loadFromFile(filename)) {
                users.push_back(tempUser);
                userMap[tempUser->getUsername()] = tempUser;
                cout << "[System] Loaded user: " << tempUser->getUsername()
                     << " (Balance: " << Utils::formatCurrency(tempUser->getAccount()->getBalance()) << ")\n";
            } else {
                delete tempUser;
                cout << "[Warning] Failed to load: " << filename << "\n";
            }
        }

        if (users.empty()) {
            cout << "[System] No existing users found. Starting fresh.\n";
        } else {
            cout << "[System] Successfully loaded " << users.size() << " user(s).\n";
        }

        cout << "\n";
    }
};

// Main function
int main() {
    cout << "Starting Digital Wallet System...\n\n";

    try {
        WalletSystem wallet;
        wallet.run();
    } catch (const exception& e) {
        cout << "\n[X] System Error: " << e.what() << "\n";
    }

    return 0;
}