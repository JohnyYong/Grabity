/*!****************************************************************
\file: TagManager.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Defines the TagManager class, which manages tags used for
        identifying and categorizing game objects. The TagManager
        allows adding, removing, and validating tags to ensure
        consistent tagging within the engine. It uses a singleton
        pattern to maintain a single instance throughout the system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "TagManager.h"
#include <iostream>
#include "fstream"
#include "ImGuiConsole.h"

#include "GameObjectFactory.h"

/**
 * @brief Retrieves the singleton instance of the TagManager class.
 * @return A reference to the singleton instance.
 */
TagManager& TagManager::GetInstance()
{
	static TagManager instance;
	return instance;
}
/**
 * @brief Preloads existing tags from a file into the system.
 * @return True if tags are successfully loaded, false otherwise.
 */
bool TagManager::PreloadTags()
{
	std::ifstream file("Assets/Lua/tags.txt");
	
	if (!file.is_open())
	{
		ImGuiConsole::Cout("Failed to open file");
		return false;
	}

	std::string tag;
	while (std::getline(file, tag))
	{
		if (!tag.empty() && !IsTagValid(tag))
		{
			AddTag(tag);
		}
	}

	file.close();
	return true;
}
/**
 * @brief Adds a new tag to the available tags list and saves it to a file.
 * @param tagName The name of the tag to be added.
 * @return True if the tag is added successfully, false if it already exists.
 */
bool TagManager::AddTag(const std::string& tagName) {
    if (availableTags.count(tagName)) {
		ImGuiConsole::Cout("Tag %s already exists! Failed to add tag!", tagName.c_str());
        return false;
    }

    // Add the tag to the availableTags set
    availableTags.insert(tagName);

#pragma region SavingIntoFile
    //Check if the tag already exists in the file
    const std::string tagFilePath = "Assets/Lua/tags.txt"; // Path to your tag file
    std::ifstream inputFile(tagFilePath);
    if (!inputFile.is_open()) {
		ImGuiConsole::Cout("Failed to open tag file for reading: %s", tagFilePath.c_str());
        return false;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        if (line == tagName) {
			ImGuiConsole::Cout("Tag %s already exists in the file. Skipping append.", tagName.c_str());
            return true;
        }
    }
    inputFile.close();

    //Append the new tag to the file
    std::ofstream outputFile(tagFilePath, std::ios::app);
    if (!outputFile.is_open()) {
		ImGuiConsole::Cout("Failed to open tag file for writing: %s", tagFilePath.c_str());
        return false;
    }

    outputFile << tagName << '\n';
    outputFile.close();
#pragma endregion


	ImGuiConsole::Cout("Tag %s added successfully and saved to file. Current no. of tags: %d", tagName.c_str(), availableTags.size());
    return true;
}

/**
 * @brief Removes an existing tag from the available tags list and updates the file.
 *        Reassigns affected game objects to the "Untagged" tag.
 * @param tagName The name of the tag to be removed.
 * @return True if the tag is removed successfully, false if it does not exist.
 */
bool TagManager::RemoveTag(const std::string& tagName)
{
	if (!availableTags.erase(tagName))
	{
		ImGuiConsole::Cout("Tag does not exists! Failed to remove tag!");
		return false;
	}

    //Force update
    for (auto& pair : GameObjectFactory::GetInstance().GetAllGameObjects()) {
        if (pair.second->GetTag() == tagName) {
            pair.second->SetTag("Untagged");
        }
    }

#pragma region SavingIntoFile
    const std::string tagFilePath = "Assets/Lua/tags.txt";
    std::ifstream inputFile(tagFilePath);

    if (!inputFile.is_open())
    {
        std::cerr << "Failed to open tag file for reading: " << tagFilePath << std::endl;
        return false;
    }

    std::vector<std::string> remainingTags;
    std::string line;
    while (std::getline(inputFile, line))
    {
        if (line != tagName)
        {
            remainingTags.push_back(line);
        }
    }
    inputFile.close();

    std::ofstream outputFile(tagFilePath, std::ios::trunc);
    if (!outputFile.is_open())
    {
        std::cerr << "Failed to open tag file for writing: " << tagFilePath << std::endl;
        return false;
    }

    for (const auto& savedTag : remainingTags)
    {
        outputFile << savedTag << '\n';
    }
    outputFile.close();
#pragma endregion

	return true;
}
/**
 * @brief Checks if a given tag exists in the available tags list.
 * @param tagName The name of the tag to validate.
 * @return True if the tag exists, false otherwise.
 */
bool TagManager::IsTagValid(const std::string& tagName) const
{
	return availableTags.count(tagName) > 0;
}
/**
 * @brief Retrieves all available tags in the system.
 * @return A constant reference to the set of all available tags.
 */
const std::unordered_set<std::string>& TagManager::GetAllTags() const
{
	return availableTags;
}

