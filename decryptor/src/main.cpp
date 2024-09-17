#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <vector>

namespace fs = std::filesystem;

int totalFiles = 0;

void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}

void decryptFile(const fs::path &inputFilePath, const fs::path &outputFilePath, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        handleErrors();

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key.data(), iv.data()))
    {
        handleErrors();
    }

    std::ifstream inFile(inputFilePath, std::ios::binary);
    std::ofstream outFile(outputFilePath, std::ios::binary);

    if (!inFile.is_open() || !outFile.is_open())
    {
        std::cerr << "Errore nell'apertura dei file!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    unsigned char buffer[1024];
    unsigned char* decryptedText = new unsigned char[1024 + EVP_CIPHER_block_size(EVP_aes_128_cbc())];

    int len = 0;
    int decryptedText_len = 0;

    while (inFile.read(reinterpret_cast<char *>(buffer), sizeof(buffer)) || inFile.gcount() > 0)
    {
        if (1 != EVP_DecryptUpdate(ctx, decryptedText, &len, buffer, inFile.gcount()))
        {
            handleErrors();
        }
        outFile.write(reinterpret_cast<char *>(decryptedText), len);
    }

    if (1 != EVP_DecryptFinal_ex(ctx, decryptedText, &decryptedText_len))
    {
        handleErrors();
    }
    outFile.write(reinterpret_cast<char *>(decryptedText), decryptedText_len);

    inFile.close();
    outFile.close();
    EVP_CIPHER_CTX_free(ctx);
}

std::string removeEncExtension(const std::string &fileName)
{
    std::string encExtension = ".enc";
    size_t pos = fileName.rfind(encExtension);
    if (pos != std::string::npos && pos == fileName.length() - encExtension.length())
    {
        return fileName.substr(0, pos); // Restituisce il fileName senza ".enc"
    }

    return fileName;
}

void processFilesInDirectory(const fs::path &directoryPath, bool maintainEncryptedFiles, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
    for (const auto &entry : fs::directory_iterator(directoryPath))
    {
        if (entry.is_directory())
        {
            processFilesInDirectory(entry.path(), maintainEncryptedFiles, key, iv);
        }
        else if (entry.is_regular_file() && entry.path().filename().string().find(".enc") != std::string::npos)
        {
            fs::path encryptedFilePath = entry.path();

            decryptFile(encryptedFilePath, removeEncExtension(encryptedFilePath.string()), key, iv);
            if(!maintainEncryptedFiles) fs::remove(encryptedFilePath);
            totalFiles++;
        }
    }
}

std::vector<unsigned char> base64Decode(const std::string& base64Data) {
    BIO* bio = BIO_new_mem_buf(base64Data.c_str(), -1);
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // No newline

    std::vector<unsigned char> decodedData(base64Data.length());
    int decodedLength = BIO_read(bio, decodedData.data(), base64Data.length());

    BIO_free_all(bio);

    decodedData.resize(decodedLength);
    return decodedData;
}

int main()
{
    std::string directoryPath;
    std::string base64Key;
    std::string base64Iv;
    std::string maintainEncryptedFiles;

    std::cout << "Path to decipher: ";
    std::getline(std::cin, directoryPath);

    std::cout << "Key in base 64: ";
    std::getline(std::cin, base64Key);

    std::cout << "IV in base 64: ";
    std::getline(std::cin, base64Iv);

    std::cout << "Maintaining encrypted files? (y/n): ";
    std::getline(std::cin, maintainEncryptedFiles);
    bool flag = maintainEncryptedFiles == "y" ? true : false;

    std::vector<unsigned char> key = base64Decode(base64Key);
    std::vector<unsigned char> iv = base64Decode(base64Iv);

    processFilesInDirectory(directoryPath, flag, key, iv);
    std::cout << "Total files decrypted: " << totalFiles << std::endl;

    return 0;
}