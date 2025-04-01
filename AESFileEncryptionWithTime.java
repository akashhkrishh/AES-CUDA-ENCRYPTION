package org.example;

import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.UUID;

public class AESFileEncryptionWithTime {

    public static String convert(String uuid) {
        String formattedUUID = uuid.replace("-", "");  // Remove dashes
        return formattedUUID;
    }

    // Method to encrypt the file
    public static void encryptFile(String inputFilePath, String outputFilePath, String secretKey) throws Exception {
        // Create AES key from the provided secret key
        SecretKeySpec keySpec = new SecretKeySpec(secretKey.getBytes(), "AES");

        // Create Cipher object for AES encryption
        Cipher cipher = Cipher.getInstance("AES");

        // Initialize cipher for encryption
        cipher.init(Cipher.ENCRYPT_MODE, keySpec);

        // Read input file
        byte[] fileBytes = Files.readAllBytes(Paths.get(inputFilePath));

        // Encrypt the file bytes
        byte[] encryptedBytes = cipher.doFinal(fileBytes);

        // Write encrypted bytes to output file
        try (FileOutputStream fos = new FileOutputStream(outputFilePath)) {
            fos.write(encryptedBytes);
        }
    }

    // Method to decrypt the file
    public static void decryptFile(String inputFilePath, String outputFilePath, String secretKey) throws Exception {
        // Create AES key from the provided secret key
        SecretKeySpec keySpec = new SecretKeySpec(secretKey.getBytes(), "AES");

        // Create Cipher object for AES decryption
        Cipher cipher = Cipher.getInstance("AES");

        // Initialize cipher for decryption
        cipher.init(Cipher.DECRYPT_MODE, keySpec);

        // Read encrypted input file
        byte[] encryptedFileBytes = Files.readAllBytes(Paths.get(inputFilePath));

        // Decrypt the file bytes
        byte[] decryptedBytes = cipher.doFinal(encryptedFileBytes);

        // Write decrypted bytes to output file
        try (FileOutputStream fos = new FileOutputStream(outputFilePath)) {
            fos.write(decryptedBytes);
        }
    }
    private static String executeCommand(String executable, String inputFile, String outputFile, String key) throws Exception {

        File executableFile = new File( System.getProperty("user.dir")+"/src/main/resources/", executable);

        if (!executableFile.exists()) {
            throw new IOException("Executable not found: " + executableFile.getAbsolutePath());
        }
        
        String command = String.format("\"%s\" \"%s\" \"%s\" \"%s\"", executableFile.getAbsolutePath(), inputFile, outputFile, convert(key));

        System.out.println("Executing command: " + command);

        ProcessBuilder processBuilder = new ProcessBuilder(command.split(" "));
        processBuilder.redirectErrorStream(true);
        Process process = processBuilder.start();

        StringBuilder output = new StringBuilder();
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
            String line;
            while ((line = reader.readLine()) != null) {
                output.append(line).append("\n");
            }
        }

        try (BufferedReader errorReader = new BufferedReader(new InputStreamReader(process.getErrorStream()))) {
            String errorLine;
            while ((errorLine = errorReader.readLine()) != null) {
                output.append("ERROR: ").append(errorLine).append("\n");
            }
        }


        int exitCode = process.waitFor();
        if (exitCode != 0) {
            throw new IOException("Command execution failed with exit code " + exitCode + "\nOutput: " + output.toString());
        }


        return output.toString();
    }

    public static void main(String[] args) {
        String rootPath = System.getProperty("user.dir")+"/src/main/resources/";
        System.out.println(rootPath);
        String secretKey = "1234567890123456";  // 16-byte secret key (128-bit AES key)
        String inputFile = rootPath+"input.txt";         // File to encrypt
        String encryptedFile = rootPath+"encrypted.bin"; // Encrypted output file
        String decryptedFile = rootPath+"decrypted.txt"; // Decrypted output file
        String key = UUID.randomUUID().toString();

        try {
            // Measure encryption time
            long startTime = System.currentTimeMillis();
            System.out.println("Encrypting file...");
            encryptFile(inputFile, encryptedFile, secretKey);
            long endTime = System.currentTimeMillis();
            System.out.println("Encryption completed in " + (endTime - startTime) + " milliseconds.");

            // Measure decryption time
            startTime = System.currentTimeMillis();
            System.out.println("Decrypting file...");
            decryptFile(encryptedFile, decryptedFile, secretKey);
            endTime = System.currentTimeMillis();
            System.out.println("Decryption completed in " + (endTime - startTime) + " milliseconds.");
            String encryptionDetails = executeCommand("aes_encryption.exe", inputFile,rootPath+"GPUinput.txt",key);
            String decryptionDetails = executeCommand("aes_decryption.exe", rootPath+"GPUinput.txt",rootPath+"GPUoutput.txt",key);
            System.out.println(encryptionDetails);
            System.out.println(decryptionDetails);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
