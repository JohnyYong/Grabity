/*!****************************************************************
\file: Systemlogging.h
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\brief: Declaration and implementation of functions for SystemLogging
class. This class is used to log the time taken by each system to
execute. It also calculates the percentage of time taken by each
system relative to the total time taken by all systems.
The average is calculated by taking the average of the
current time taken and the previous time taken. 

To use this class,create an instance of SystemLog with the 
name of the system as the parameter. The destructor of the
will automatically calculate the time taken by the system
and store it in the SystemLogManager class. Alternatively,
you can manually call the destructor of the SystemLog class
by calling the Reset() function. The SystemLogManager class
can be accessed by calling the GetInstance() function.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifdef _LOGGING
#pragma once
#ifndef SYSTEMLOGGING_H
#define SYSTEMLOGGING_H

#include <chrono>
#include <string>
#include <map>
#include <memory>

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include "FileManager.h"

class SystemLogManager {
public:

    /*!****************************************************************
    \func  SystemLogManager
    \brief Constructor for the SystemLogManager class.
    *******************************************************************!*/
    SystemLogManager() = default;

    /*!****************************************************************
    \func  ~SystemLogManager
    \brief Destructor for the SystemLogManager class. Automatically
           writes the average execution times of all systems to a
           log file upon destruction.
    *******************************************************************!*/
    ~SystemLogManager()
    {
		WriteLogAveragesToFile();
    }

    /*!****************************************************************
    \func  GetInstance
    \brief Retrieves the singleton instance of the SystemLogManager
           class.
    \return A reference to the SystemLogManager instance.
    *******************************************************************!*/
    static SystemLogManager& GetInstance() {
        if (!instance) {
            instance = std::make_unique<SystemLogManager>();
        }
        return *instance;
    }

    /*!****************************************************************
    \func  SetAverage
    \brief Sets the average execution time for a system. If a system
           already has an average, the new value is averaged with the
           existing one.
    \param systemName The name of the system.
    \param log The time taken by the system in milliseconds.
    *******************************************************************!*/
    void SetAverage(const std::string& systemName, double log) {
        if (LogAverageMap.find(systemName) == LogAverageMap.end()) {
            LogAverageMap[systemName] = log;
        }
        else {
            LogAverageMap[systemName] = (LogAverageMap[systemName] + log) / 2;
        }
    }

    /*!****************************************************************
    \func  WriteLogAveragesToFile
    \brief Writes the average execution times of all systems to a log
           file, along with the percentage of time relative to the
           total execution time.
    *******************************************************************!*/
    void WriteLogAveragesToFile() {
        FileManager file("SystemAverageTimes.log");
		double total = 0;
        for (const auto& log : LogAverageMap)
        {
			total += log.second;
        }
        for (const auto& log : LogAverageMap) {
			file.Write(log.first + ": " + std::to_string(log.second) + "ms" + " (" + std::to_string((log.second / total) * 100) + "%)\n");
        }
    }

    /*!****************************************************************
    \func  GetLogAverages
    \brief Retrieves a map of all system names and their average
           execution times.
    \return A map containing system names as keys and their average
            execution times in milliseconds as values.
    *******************************************************************!*/
    std::map<std::string, double> GetLogAverages() const {
        return LogAverageMap;
    }

private:
    std::map<std::string, double> LogAverageMap;
    static std::unique_ptr<SystemLogManager> instance;
};

std::unique_ptr<SystemLogManager> SystemLogManager::instance = nullptr;

class SystemLog {
public:

    /*!****************************************************************
    \func  SystemLog
    \brief Constructor for the SystemLog class. Starts timing for
           the specified system.
    \param systemName The name of the system to log.
    *******************************************************************!*/
    SystemLog(const std::string& systemName)
        : systemName(systemName), start(std::chrono::high_resolution_clock::now()) {
    }

    /*!****************************************************************
    \func  ~SystemLog
    \brief Destructor for the SystemLog class. Calculates the time
           taken by the system and updates the average in the
           SystemLogManager.
    *******************************************************************!*/
    ~SystemLog() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        SystemLogManager::GetInstance().SetAverage(systemName, duration.count());
    }

private:
    std::string systemName;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

#endif // SYSTEMLOGGING_H
#endif // _LOGGING