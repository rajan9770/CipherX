#ifndef AES_CRYPTO_H
#define AES_CRYPTO_H

#include <string>

bool encryptFile(
    const std::string &inputFileName,
    const std::string &outputFileName,
    const std::string &password);

bool decryptFile(
    const std::string &inputFileName,
    const std::string &outputFileName,
    const std::string &password);

#endif