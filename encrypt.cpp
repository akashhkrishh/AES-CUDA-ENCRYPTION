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

// Function to write encrypted data to a file
void writeFile(const std::string &filename, const std::vector<uint8_t> &data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        exit(1);
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// Padding data to be a multiple of 16 bytes
void padData(std::vector<uint8_t> &data) {
    size_t paddingSize = 16 - (data.size() % 16);
    if (paddingSize != 16) {
        data.resize(data.size() + paddingSize, 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: encrypt <input_file> <output_file> <aes_key>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    std::string hexKey = argv[3];  // AES key passed as a command-line argument

    // Convert hexKey string into a vector of uint8_t
    std::vector<uint8_t> aesKey;
    if (!hexStringToBytes(hexKey, aesKey) || aesKey.size() != 16) {
        std::cerr << "Invalid AES key. The key must be 16 bytes (32 hex characters)." << std::endl;
        return 1;
    }

    // Read input file
    std::vector<uint8_t> dataToEncrypt = readFile(inputFile);

    if (dataToEncrypt.empty()) {
        std::cerr << "Input file is empty or could not be read!" << std::endl;
        return 1;
    }

    std::cout << "Read " << dataToEncrypt.size() << " bytes from " << inputFile << std::endl;

    padData(dataToEncrypt);  // Pad data to a multiple of 16 bytes

    size_t dataSize = dataToEncrypt.size();
    std::vector<uint8_t> encryptedData(dataSize);
    std::memcpy(encryptedData.data(), dataToEncrypt.data(), dataSize);

    // Initialize GPU for encryption
    std::cout << "Initializing GPU..." << std::endl;
    initGPU();

    std::cout << "Encrypting data..." << std::endl;
    aesEncrypt(encryptedData.data(), aesKey.data(), dataSize);

    // Write encrypted data to the output file
    writeFile(outputFile, encryptedData);
    std::cout << "Encrypted data written to " << outputFile << std::endl;

    freeGPU();  // Free GPU resources

    return 0;
}
