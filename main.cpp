#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <iomanip>
#include "aes.h"  // Include the header file with AES GPU functions

const uint8_t aesKey[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x97, 0x75, 0x46, 0x61, 0x1f, 0x3d
};

// Function to read a file into a vector of bytes
std::vector<uint8_t> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
}

// Function to write encrypted data to a file
void writeFile(const std::string &filename, const std::vector<uint8_t> &data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        exit(1);
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// Function to write decrypted text to a file
void writeDecryptedTextToFile(const std::string &filename, const std::string &decryptedText) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing decrypted text: " << filename << std::endl;
        exit(1);
    }
    file << decryptedText;
}

// Padding data to be a multiple of 16 bytes
void padData(std::vector<uint8_t> &data) {
    size_t paddingSize = 16 - (data.size() % 16);
    if (paddingSize != 16) {
        data.resize(data.size() + paddingSize, 0);
    }
}

// Removing padding (zero padding)
void removePadding(std::vector<uint8_t> &data) {
    size_t i = data.size();
    while (i > 0 && data[i - 1] == 0) {
        --i;
    }
    data.resize(i);
}

// Function to convert decrypted data to a human-readable string (ASCII)
std::string decryptDataToString(const std::vector<uint8_t>& data) {
    // Convert the byte data to a string using ASCII encoding
    std::string result(data.begin(), data.end());
    return result;
}

int main() {
    std::string inputFile = "input.txt";
    std::vector<uint8_t> dataToEncrypt = readFile(inputFile);

    if (dataToEncrypt.empty()) {
        std::cerr << "Input file is empty or could not be read!" << std::endl;
        return 1;
    }

    std::cout << "Read " << dataToEncrypt.size() << " bytes from " << inputFile << std::endl;

    padData(dataToEncrypt);  // Pad data to a multiple of 16 bytes

    size_t dataSize = dataToEncrypt.size();

    std::vector<uint8_t> encryptedData(dataSize);
    std::vector<uint8_t> decryptedData(dataSize);

    std::memcpy(encryptedData.data(), dataToEncrypt.data(), dataSize);

    std::cout << "Initializing GPU..." << std::endl;
    initGPU();

    std::cout << "Encrypting data..." << std::endl;
    aesEncrypt(encryptedData.data(), aesKey, dataSize);
    writeFile("encrypted_output.txt", encryptedData);

    std::cout << "Decrypting data..." << std::endl;
    std::memcpy(decryptedData.data(), encryptedData.data(), dataSize);
    aesDecrypt(decryptedData.data(), aesKey, dataSize);

    removePadding(decryptedData);  // Remove padding after decryption

    // Convert the decrypted data to a readable string (assuming it's ASCII encoded)
    std::string decryptedText = decryptDataToString(decryptedData);

    // Write the decrypted text to a file
    writeDecryptedTextToFile("decryptionresult.txt", decryptedText);

    std::cout << "Decrypted data written to decryptionresult.txt" << std::endl;

    freeGPU();  // Free GPU resources
    return 0;
}
