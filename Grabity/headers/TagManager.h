/*!****************************************************************
\file: TagManager.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Declares the TagManager class, which manages tags used for
		identifying and categorizing game objects. The TagManager
		allows adding, removing, and validating tags to ensure
		consistent tagging within the engine. It uses a singleton
		pattern to maintain a single instance throughout the system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include <unordered_set>
#include <string>

class TagManager {
private:
	/**
	 * @brief A set containing all available tags in the system.
	 */
	std::unordered_set<std::string> availableTags;

public:

	//Preload existing tags
	/**
	 * @brief Preloads existing tags from a saved file or database.
	 * @return True if tags are successfully loaded, false otherwise.
	 */
	bool PreloadTags();

	//Adding new available tag
	/**
	 * @brief Adds a new tag to the available tags list.
	 * @param tagName The name of the tag to be added.
	 * @return True if the tag is added successfully, false if it already exists.
	 */
	bool AddTag(const std::string& tagName);

	//Removing existing tag
	/**
	 * @brief Removes an existing tag from the available tags list.
	 * @param tagName The name of the tag to be removed.
	 * @return True if the tag is removed successfully, false if it does not exist.
	 */
	bool RemoveTag(const std::string& tagName);

	//Check if tag exists in availableTag
	/**
	 * @brief Checks if a given tag exists in the available tags list.
	 * @param tagName The name of the tag to validate.
	 * @return True if the tag exists, false otherwise.
	 */
	bool IsTagValid(const std::string& tagName) const;

	//All tags getter
	/**
	 * @brief Retrieves all available tags in the system.
	 * @return A constant reference to the set of all available tags.
	 */
	const std::unordered_set<std::string>& GetAllTags() const;

	//SingleTon
	/**
	 * @brief Retrieves the singleton instance of the TagManager class.
	 * @return A reference to the singleton instance.
	 */
	static TagManager& GetInstance();
};