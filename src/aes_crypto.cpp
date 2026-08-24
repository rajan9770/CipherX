#include "aes_crypto.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

using namespace std;

const int SALT_SIZE = 16;
const int IV_SIZE = 16;
const int KEY_SIZE = 32;
const int ITERATIONS = 100000;

void printOpenSSLError()
{
    ERR_print_errors_fp(stderr);
}

bool deriveKey(
    const string &password,
    const unsigned char *salt,
    unsigned char *key)
{
    return PKCS5_PBKDF2_HMAC(
        password.c_str(),
        static_cast<int>(password.length()),
        salt,
        SALT_SIZE,
        ITERATIONS,
        EVP_sha256(),
        KEY_SIZE,
        key);
}

bool encryptFile(
    const string &inputFileName,
    const string &outputFileName,
    const string &password)
{
    ifstream inputFile(inputFileName, ios::binary);

    if (!inputFile)
    {
        return false;
    }

    vector<unsigned char> plaintext(
        (istreambuf_iterator<char>(inputFile)),
        istreambuf_iterator<char>());

    inputFile.close();

    unsigned char salt[SALT_SIZE];
    unsigned char iv[IV_SIZE];
    unsigned char key[KEY_SIZE];

    if (RAND_bytes(salt, SALT_SIZE) != 1)
    {
        return false;
    }

    if (RAND_bytes(iv, IV_SIZE) != 1)
    {
        return false;
    }

    if (!deriveKey(password, salt, key))
    {
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (!ctx)
    {
        return false;
    }

    if (EVP_EncryptInit_ex(
            ctx,
            EVP_aes_256_cbc(),
            nullptr,
            key,
            iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    vector<unsigned char> ciphertext(
        plaintext.size() + EVP_MAX_BLOCK_LENGTH);

    int len = 0;
    int ciphertextLength = 0;

    if (EVP_EncryptUpdate(
            ctx,
            ciphertext.data(),
            &len,
            plaintext.data(),
            static_cast<int>(plaintext.size())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    ciphertextLength = len;

    if (EVP_EncryptFinal_ex(
            ctx,
            ciphertext.data() + ciphertextLength,
            &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    ciphertextLength += len;

    EVP_CIPHER_CTX_free(ctx);

    // File format:
    // [16-byte salt][16-byte IV][encrypted data]

    ofstream outputFile(outputFileName, ios::binary);

    if (!outputFile)
    {
        return false;
    }

    outputFile.write(
        reinterpret_cast<char *>(salt),
        SALT_SIZE);

    outputFile.write(
        reinterpret_cast<char *>(iv),
        IV_SIZE);

    outputFile.write(
        reinterpret_cast<char *>(ciphertext.data()),
        ciphertextLength);

    outputFile.close();

    return true;
}

bool decryptFile(
    const string &inputFileName,
    const string &outputFileName,
    const string &password)
{
    ifstream inputFile(inputFileName, ios::binary);

    if (!inputFile)
    {
        return false;
    }

    unsigned char salt[SALT_SIZE];
    unsigned char iv[IV_SIZE];

    inputFile.read(
        reinterpret_cast<char *>(salt),
        SALT_SIZE);

    if (inputFile.gcount() != SALT_SIZE)
    {
        inputFile.close();
        return false;
    }

    inputFile.read(
        reinterpret_cast<char *>(iv),
        IV_SIZE);

    if (inputFile.gcount() != IV_SIZE)
    {
        inputFile.close();
        return false;
    }

    vector<unsigned char> ciphertext(
        (istreambuf_iterator<char>(inputFile)),
        istreambuf_iterator<char>());

    inputFile.close();

    if (ciphertext.empty())
    {
        return false;
    }

    unsigned char key[KEY_SIZE];

    if (!deriveKey(password, salt, key))
    {
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (!ctx)
    {
        return false;
    }

    if (EVP_DecryptInit_ex(
            ctx,
            EVP_aes_256_cbc(),
            nullptr,
            key,
            iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    vector<unsigned char> plaintext(
        ciphertext.size() + EVP_MAX_BLOCK_LENGTH);

    int len = 0;
    int plaintextLength = 0;

    if (EVP_DecryptUpdate(
            ctx,
            plaintext.data(),
            &len,
            ciphertext.data(),
            static_cast<int>(ciphertext.size())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    plaintextLength = len;

    if (EVP_DecryptFinal_ex(
            ctx,
            plaintext.data() + plaintextLength,
            &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    plaintextLength += len;

    EVP_CIPHER_CTX_free(ctx);

    ofstream outputFile(outputFileName, ios::binary);

    if (!outputFile)
    {
        return false;
    }

    outputFile.write(
        reinterpret_cast<char *>(plaintext.data()),
        plaintextLength);

    outputFile.close();

    return true;
}