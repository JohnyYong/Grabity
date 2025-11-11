/*!****************************************************************
\file: FileManager.cpp
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\brief: This file contains the implementation of the FileManager class
which is a wrapper for file I/O operations. It provides functions to
read and write data to a file.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "Filemanager.h"

// Constructor that initializes the FileManager with the specified file path, mode, and write mode.
FileManager::FileManager(const std::string& filePath, Mode mode, WriteMode writeMode)
    : filePath(filePath), mode(mode), writeMode(writeMode) {
    std::ios_base::openmode openMode = std::ios::in | std::ios::out;
    if (mode == Mode::Binary) {
        openMode |= std::ios::binary;
    }
    if (writeMode == WriteMode::Append) {
        openMode |= std::ios::app;
    }
    else if (writeMode == WriteMode::Overwrite) {
        openMode |= std::ios::trunc;
    }
    fileStream.open(filePath, openMode);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
}

// Destructor that ensures the file stream is properly closed.
FileManager::~FileManager() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

// Move constructor for transferring ownership of a FileManager instance.
FileManager::FileManager(FileManager&& other) noexcept
    : fileStream(std::move(other.fileStream)), filePath(std::move(other.filePath)), mode(other.mode), writeMode(other.writeMode) {
    other.fileStream = std::fstream();
}

// Move assignment operator for transferring ownership of a FileManager instance.
FileManager& FileManager::operator=(FileManager&& other) noexcept {
    if (this != &other) {
        if (fileStream.is_open()) {
            fileStream.close();
        }
        fileStream = std::move(other.fileStream);
        filePath = std::move(other.filePath);
        mode = other.mode;
        writeMode = other.writeMode;
        other.fileStream = std::fstream();
    }
    return *this;
}

//  Checks whether the file stream is currently open.
bool FileManager::IsOpen() const {
    return fileStream.is_open();
}

//  Writes a string to the file.
void FileManager::Write(const std::string& data) {
    if (!fileStream.is_open()) {
        throw std::runtime_error("File is not open: " + filePath);
    }
    if (mode == Mode::Binary) {
        throw std::runtime_error("Cannot write std::string in binary mode");
    }
    fileStream << data;
}

// Writes binary data to the file.
void FileManager::Write(const char* data, std::size_t size) {
    if (!fileStream.is_open()) {
        throw std::runtime_error("File is not open: " + filePath);
    }
    fileStream.write(data, size);
}

// Reads the entire content of the file as a string.
std::string FileManager::Read() {
    if (!fileStream.is_open()) {
        throw std::runtime_error("File is not open: " + filePath);
    }
    if (mode == Mode::Binary) {
        throw std::runtime_error("Cannot read std::string in binary mode");
    }
    std::string content((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    return content;
}

// Reads binary data from the file into a buffer.
void FileManager::Read(char* buffer, std::size_t size) {
    if (!fileStream.is_open()) {
        throw std::runtime_error("File is not open: " + filePath);
    }
    fileStream.read(buffer, size);
}
