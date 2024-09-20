# A simple ransomware

## This repository is for educational purposes only.

### How it works

This is a small example of ransomware written in C++ that uses the AES-128 algorithm to encrypt all files in a specific folder. It communicates with a C2 server to retrieve the folder to encrypt, the encryption key, and the IV.

Create a new directory called `TestRW` under your user folder (e.g., `C:\Users\name on Windows`) and place some test files inside it. To test the ransomware, you must run the C2 remote server locally and update the `C2APIURL` in `main.cpp` to point to the correct local endpoint.

For more details on setting up the C2 server, please refer to the specific README included.

Build the ransomware using CMake and run the executable (called `a-simple-ransomware`).

**Warning:** The plaintext will be deleted, and only the encrypted files will remain.

### Decrypting

Build the decryptor using CMake and run the executable. It will prompt you for the path to decrypt, the encryption key, and the IV.