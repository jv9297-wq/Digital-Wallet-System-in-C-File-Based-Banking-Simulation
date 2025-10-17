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

These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

You need a C++ compiler installed on your system (e.g., GCC/G++).

```bash
# Check your compiler version
g++ --version