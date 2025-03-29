#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "aes.h"

// Helper function to convert hex string to byte array
bool hexStringToBytes(const std::string &hex, std::vector<uint8_t> &out) {
    // Check if the hex string has an even length
    if (hex.length() % 2 != 0) {
        std::cerr << "Invalid hex string length. It must be even." << std::endl;
        return false;
    }

    // Convert the hex string into bytes
    for (size_t i = 0; i < hex.length(); i += 2) {
        uint8_t byte = 0;
        std::istringstream(hex.substr(i, 2)) >> std::hex >> byte;
        out.push_back(byte);
    }
    return true;
}

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

// Function to write decrypted text to a file
void writeDecryptedTextToFile(const std::string &filename, const std::string &decryptedText) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing decrypted text: " << filename << std::endl;
        exit(1);
    }
    file << decryptedText;
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

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: decrypt <input_file> <output_file> <aes_key>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    std::string hexKey = argv[3];

    // Convert hexKey string into a vector of uint8_t
    std::vector<uint8_t> aesKey;
    if (!hexStringToBytes(hexKey, aesKey) || aesKey.size() != 16) {
        std::cerr << "Invalid AES key. The key must be 16 bytes (32 hex characters)." << std::endl;
        return 1;
    }

    // Read encrypted file
    std::vector<uint8_t> encryptedData = readFile(inputFile);

    if (encryptedData.empty()) {
        std::cerr << "Encrypted file is empty or could not be read!" << std::endl;
        return 1;
    }

    std::cout << "Read " << encryptedData.size() << " bytes from " << inputFile << std::endl;

    size_t dataSize = encryptedData.size();
    std::vector<uint8_t> decryptedData(dataSize);
    std::memcpy(decryptedData.data(), encryptedData.data(), dataSize);

    // Initialize GPU for decryption
    std::cout << "Initializing GPU..." << std::endl;
    initGPU();

    std::cout << "Decrypting data..." << std::endl;
    aesDecrypt(decryptedData.data(), aesKey.data(), dataSize);

    // Remove padding after decryption
    removePadding(decryptedData);

    // Convert the decrypted data to a readable string
    std::string decryptedText = decryptDataToString(decryptedData);

    // Write the decrypted text to the output file
    writeDecryptedTextToFile(outputFile, decryptedText);
    std::cout << "Decrypted data written to " << outputFile << std::endl;

    freeGPU();  // Free GPU resources

    return 0;
}
