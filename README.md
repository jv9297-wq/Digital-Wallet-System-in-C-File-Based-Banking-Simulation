# 🏦 Digital Wallet System in C++: File-Based Banking Simulation

An object-oriented, console-based application that simulates a digital wallet or basic banking system. It demonstrates core C++ principles, including OOP design, file-based data persistence, and basic security implementation.

## ✨ Features

* **User Authentication:** Secure **Registration** and **Login** for distinct users.
* **Financial Operations:** Supports **Deposit**, **Withdrawal**, and **Balance Inquiry**.
* **Peer-to-Peer Transfer:** Funds transfer between users within the system.
* **Data Persistence:** User credentials, account balances, and transaction history are saved to and loaded from **local text files** (`.txt`).
* **Transaction Logging:** Detailed history of all account movements (deposits, transfers, withdrawals).
* **Basic Security:** Implements a simulated **4-digit OTP (One-Time Password)** for withdrawals and transfers.
* **Cross-Platform:** Includes utility functions designed for compatibility with both Windows and Unix-like environments.

## 🛠️ Technology Stack

* **Language:** C++
* **Design:** Object-Oriented Programming (OOP)
* **Data Handling:** `std::vector`, `std::map`, and `fstream` (File I/O)

## 🚀 Getting Started

These instructions will guide you through setting up, compiling, and running the **Digital Wallet System** on your local machine.

-----

### Prerequisites

You need a C++ compiler installed on your system. **GCC (GNU Compiler Collection)** or **Clang** is highly recommended.

  * **Linux/macOS:** You likely have GCC/Clang pre-installed or can install them via your package manager (`apt`, `brew`, etc.) or Xcode (macOS).
  * **Windows:** You can use **MinGW** (Minimalist GNU for Windows) or **Visual Studio with C++ tools**.

### 1\. Compilation

1.  **Save the Code:** Save the provided C++ code into a single file named `main.cpp`.
2.  **Open Terminal/Command Prompt:** Navigate to the directory where you saved `main.cpp`.

#### Compiling on Linux/macOS (GCC/Clang)

Use the following command to compile. The `-std=c++11` flag is crucial as the code uses modern C++ features.

```bash
g++ -std=c++11 main.cpp -o wallet
```

#### Compiling on Windows (MinGW)

If using MinGW, the command is the same:

```bash
g++ -std=c++11 main.cpp -o wallet.exe
```

-----

### 2\. Execution

After successful compilation, an executable file named `wallet` (or `wallet.exe` on Windows) will be created in your current directory.

#### Running on Linux/macOS

```bash
./wallet
```

#### Running on Windows

```bash
wallet.exe
```

### 3\. Data Storage 💾

Upon first run and after user registration, the system will create new files in the same directory where the executable is located:

  * **User File (`[username]_user.txt`):** Stores basic credentials and links to the account.
  * **Account File (`[accountId].txt`):** Stores the current balance and the complete **transaction history**.

**Note:** If you delete these files, the system will restart with no registered users.

-----

### Quick Start Workflow

1.  **Register:** Select **2** (Register) from the Main Menu. Create at least two users (e.g., `userA` and `userB`) to test transfers.
2.  **Login:** Log in as `userA`.
3.  **Deposit:** Select **2** (Deposit Money) and add some funds (e.g., Rs. 5000).
4.  **Transfer:** Log in as `userA`, select **4** (Transfer Money), choose `userB` as the recipient, and enter an amount. You'll need to enter the **simulated OTP** shown on the screen.
5.  **Verify:** Log out, log in as `userB`, and check the balance or view transactions to confirm the funds were received.
