/*!****************************************************************
\file: FileManager.h
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\brief: Declaration of functions for FileManager

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include <fstream>
#include <string>

/*!****************************************************************
\class FileManager
\brief Manages file operations such as reading and writing in text
       or binary modes.
*******************************************************************!*/
class FileManager {
public:
    /*!****************************************************************
    \enum  FileManager::Mode
    \brief Specifies the mode of file operation.
    \values
        - Text: Operates on text files.
        - Binary: Operates on binary files.
    *******************************************************************!*/
    enum class Mode {
        Text,
        Binary
    };

    /*!****************************************************************
    \enum  FileManager::WriteMode
    \brief Specifies the write mode for file operations.
    \values
        - Overwrite: Overwrites the existing content of the file.
        - Append: Appends to the existing content of the file.
    *******************************************************************!*/
    enum class WriteMode {
        Overwrite,
        Append
    };

    FileManager() = delete;

    /*!****************************************************************
    \func  FileManager::FileManager
    \brief Constructor that initializes the FileManager with the specified
           file path, mode, and write mode.
    \param filePath The path to the file.
    \param mode The mode of file operation (default is Mode::Text).
    \param writeMode The write mode for file operations (default is WriteMode::Overwrite).
    *******************************************************************!*/
    FileManager(const std::string& filePath, Mode mode = Mode::Text, WriteMode writeMode = WriteMode::Overwrite);

    /*!****************************************************************
    \func  FileManager::~FileManager
    \brief Destructor that ensures the file stream is properly closed.
    *******************************************************************!*/
    ~FileManager();

    /*!****************************************************************
    \func  FileManager::FileManager
    \brief Move constructor for transferring ownership of a FileManager instance.
    \param other The FileManager instance to move from.
    *******************************************************************!*/
    FileManager(FileManager&& other) noexcept;

    /*!****************************************************************
    \func  FileManager::operator=
    \brief Move assignment operator for transferring ownership of a FileManager instance.
    \param other The FileManager instance to assign from.
    \return A reference to the assigned FileManager instance.
    *******************************************************************!*/
    FileManager& operator=(FileManager&& other) noexcept;


    /*!****************************************************************
    \func  FileManager::IsOpen
    \brief Checks whether the file stream is currently open.
    \return True if the file stream is open, false otherwise.
    *******************************************************************!*/
    bool IsOpen() const;

    /*!****************************************************************
    \func  FileManager::Write
    \brief Writes a string to the file.
    \param data The string data to write.
    *******************************************************************!*/
    void Write(const std::string& data);

    /*!****************************************************************
    \func  FileManager::Write
    \brief Writes binary data to the file.
    \param data A pointer to the binary data to write.
    \param size The size of the binary data in bytes.
    *******************************************************************!*/
    void Write(const char* data, std::size_t size);

    /*!****************************************************************
    \func  FileManager::Read
    \brief Reads the entire content of the file as a string.
    \return The content of the file as a string.
    *******************************************************************!*/
    std::string Read();

    /*!****************************************************************
    \func  FileManager::Read
    \brief Reads binary data from the file into a buffer.
    \param buffer A pointer to the buffer where the data will be stored.
    \param size The size of the buffer in bytes.
    *******************************************************************!*/
    void Read(char* buffer, std::size_t size);

private:
    std::fstream fileStream;
    std::string filePath;
    Mode mode;
    WriteMode writeMode;
};
