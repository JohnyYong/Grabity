/*!****************************************************************
\file:  LuaConfig.h
\author: Goh Jun Jie, g.junjie, 2301293
\co-author:
\brief: Functions for Serialization/Deserialization using Lua.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include <string>

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include "ImGuiConsole.h"
#include <fstream>
#include <vector>
#include <variant>
#include <unordered_set>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

//==============================================DECLARATION====================================================

namespace LuaUtilities {
    //void LoadSceneFromLua(const std::string& loadFilePath);
}

class LuaManager {
public: 
    using LuaValue = std::variant<int, float, double, std::string, bool>;
    using LuaValueContainer = std::vector<LuaValue>;

public:
    LuaManager(const std::string& luaFilePath);


    //Similar Templates that works the same========================================================================
    /**
     * \brief Reads a value from the `Name` subtable of the specified Lua table.
     *
     * This function accesses the `Name` subtable of the given `tableName` and retrieves
     * the value associated with the specified `key`.
     *
     * \tparam T The type of the value to retrieve.
     * \param tableName The name of the base Lua table.
     * \param key The key within the `Name` subtable to retrieve the value.
     * \return The value of type `T` from the `Name` subtable.
     * \throws std::runtime_error If the table or key is not found, or the type is invalid.
     */
    template<typename T>
    T LuaRead(const std::string& tableName, const std::vector<std::string>& keys);

 /**
 * \brief Reads a value of type `T` from the "Parent" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Parent" section.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromName(const std::string& tableName, const std::string& key);    //name


    /**
 * \brief Reads a value of type `T` from the "Parent" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Parent" section.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromParent(const std::string& tableName, const std::string& key);  //parent


    /**
     * \brief Reads a value of type `T` from the "Window" Lua table.
     * \tparam T The type of the value to retrieve.
     * \param key The key to access the value in the "Window" table.
     * \return The retrieved value of type `T`.
     */
    template<typename T>
    T LuaReadFromWindow(const std::string& key);    //window


    /**
 * \brief Reads a value of type `T` from the "Sprite" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The base key for the value in the "Sprite" section.
 * \param index The index appended to the key for accessing specific values.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromSprite(const std::string& tableName, const std::string& key, int index);    //sprite


    /**
 * \brief Reads a value of type `T` from the "Transform" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Transform" section.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromTransform(const std::string& tableName, const std::string& key);  //transform


    /**
 * \brief Reads a value of type `T` from the "RigidBody" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "RigidBody" section.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromRigidBody(const std::string& tableName, const std::string& key); //rigidbody


    /**
 * \brief Reads a value of type `T` from the "Collider" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The base key for the value in the "Collider" section.
 * \param index The index appended to the key for accessing specific values.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromCollider(const std::string& tableName, const std::string& key, int index); //collider


    /**
 * \brief Reads a value of type `T` from the "AIState" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The base key for the value in the "AIState" section.
 * \param index The index appended to the key for accessing specific values.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromAIState(const std::string& tableName, const std::string& key, int index); //ai state machine



    /**
     * \brief Reads a value of type `T` from the "PlayerController" section of the specified Lua table.
     * \tparam T The type of the value to retrieve.
     * \param tableName The name of the Lua table.
     * \param key The key to access the value within the "PlayerController" section.
     * \return The retrieved value of type `T`.
     */
    template<typename T>
    T LuaReadFromPlayer(const std::string& tableName, const std::string& key);  //playerComponent



    /**
 * \brief Reads a value of type `T` from the "Text" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Text" section.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromText(const std::string& tableName, const std::string& key);    //text

    /**
* \brief Reads a value of type `T` from the "TextUI" section of the specified Lua table.
* \tparam T The type of the value to retrieve.
* \param tableName The name of the Lua table.
* \param key The key to access the value within the "TextUI" section.
* \return The retrieved value of type `T`.
*/
    template<typename T>
    T LuaReadFromTextUI(const std::string& tableName, const std::string& key);    //text


    /**
     * \brief Reads a value of type `T` from the "Health" section of the specified Lua table.
     * \tparam T The type of the value to retrieve.
     * \param tableName The name of the Lua table.
     * \param key The key to access the value within the "Health" section.
     * \return The retrieved value of type `T`.
     */
    template<typename T>
    T LuaReadFromHealth(const std::string& tableName, const std::string& key);  //health


    /**
 * \brief Reads a value of type `T` from the "Spawner" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Spawner" section.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromSpawner(const std::string& tableName, const std::string& key); //spawner

    /**
 * \brief Reads a value of type `T` from the "SpriteUI" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "SpriteUI" section.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromSpriteUI(const std::string& tableName, const std::string& key);    //spriteUI
    

    /**
 * \brief Reads a value of type `T` from the "Button" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Button" section.
 * \return The retrieved value of type `T`.
 */
    template<typename T>
    T LuaReadFromButton(const std::string& tableName, const std::string& key);  //button


    /**
 * \brief Reads a value of type `T` from the "UI" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "UI" section.
 * \return The retrieved value of type `T`.
 */
	template<typename T>
	T LuaReadFromUI(const std::string& tableName, const std::string& key);  //UI

    template<typename T>
    T LuaReadFromExplosion(const std::string& tableName, const std::string& key);

    template<typename T>
    T LuaReadFromSplitting(const std::string& tableName, const std::string& key);
    /**
 * \brief Reads audio clip IDs from the "AudioClips" section of the specified Lua table.
 * \param tableName The name of the Lua table.
 * \param audioClipIDs A reference to a vector to store the retrieved audio clip IDs.
 */
    void LuaReadFromAudioClips(const std::string& tableName, std::vector<int>& audioClipIDs);   //audio

    //Similar Templates that works the same========================================================================






 /**
 * \brief Appends values to a new nested table inside the main table if not found.
 *        If the nested table exists, it updates or adds key-value pairs within it.
 * \param tableName The name of the main table in the Lua file.
 * \param values A container of values to write to the Lua file.
 * \param keys A vector of keys corresponding to the values.
 * \param nestedTableName The name of the nested table to update or create.
 */
    void LuaWrite(const std::string& tableName, const LuaValueContainer& values, const std::vector<std::string>& keys, const std::string& nestedTableName); //

    /**
   * \brief Saves Lua content to a new scene file.
   * \param filename The name of the new scene file to be created.
   * \param luaContent The Lua script content to save into the file.
   */
    void SaveNewScene(const std::string& filename, const std::string& luaContent);

    /**
 * \brief Extracts object names and their corresponding IDs from the Lua globals table.
 * \return An unordered map where keys are object IDs, and values are object names.
 */
    std::unordered_map<int, std::string> extractNames();

    /**
 * \brief Checks if a sub-table exists within a parent Lua table.
 * \param parentTable The name of the parent Lua table.
 * \param subTable The name of the sub-table to check for.
 * \return True if the sub-table exists, false otherwise.
 */
    bool TableExists(const std::string& parentTable, const std::string& subTable);

    /**
 * \brief Loads a Lua script from a file into the Lua state.
 * \param luaFilePath The file path of the Lua script to load.
 */
    void LuaLoadFile(const std::string& luaFilePath);

    /**
 * \brief Counts the number of custom tables in the Lua globals.
 * \return The number of custom tables in the Lua globals.
 */
    int countTables();

    /**
 * \brief Clears the content of the current Lua file and resets it to an empty configuration.
 */
    void ClearLuaFile();

    /**
  * \brief Extracts object names, IDs, and parent IDs from the Lua globals table.
  * \return An unordered map where keys are object IDs, and values are pairs containing
  *         the object name and its parent ID.
  */
    std::unordered_map<int, std::pair<std::string, int>>  extractNamesWithParentIDs();
private:
    sol::state lua;
    std::string currentLuaFilePath;
    std::vector<std::string> fileContent; 

    /**
     * \brief Determines the Lua type for a value and appends it to the entry string.
     * \param entry The string to which the type information will be appended.
     * \param value The Lua value whose type is to be determined.
     */
    void FindType(std::string& entry, const LuaValue& value);

    /**
     * \brief Reads the content of a Lua file into a vector of strings.
     * \param luaFilePath The file path of the Lua script to read.
     * \return A vector of strings containing the file's content line by line.
     */
    std::vector<std::string> readFile(const std::string& luaFilePath);

    /**
     * \brief Writes content from a vector of strings into a Lua file.
     * \param luaFilePath The file path to write the content to.
     * \param content A vector of strings containing the content to be written.
     * \return True if the write operation was successful, false otherwise.
     */
    bool writeFile(const std::string& luaFilePath, const std::vector<std::string>& content);
};

/**
 * \brief Removes leading and trailing whitespace from a string.
 * \param str The string to trim.
 * \return The trimmed string with whitespace removed from both ends.
 */
std::string trimWhitespace(const std::string& str);


//==============================================DEFINITIONS====================================================



//Similar Templates that works the same========================================================================
/**
 * \brief Reads a value from the `Name` subtable of the specified Lua table.
 *
 * This function accesses the `Name` subtable of the given `tableName` and retrieves
 * the value associated with the specified `key`.
 *
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the base Lua table.
 * \param key The key within the `Name` subtable to retrieve the value.
 * \return The value of type `T` from the `Name` subtable.
 * \throws std::runtime_error If the table or key is not found, or the type is invalid.
 */

template<typename T>
T LuaManager::LuaReadFromName(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "Name", key });
}


/**
 * \brief Reads a value of type `T` from the "Parent" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Parent" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromWindow(const std::string& key) {
    return LuaRead<T>("Window", { key });
}


/**
 * \brief Reads a value of type `T` from the "Window" Lua table.
 * \tparam T The type of the value to retrieve.
 * \param key The key to access the value in the "Window" table.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromParent(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "Parent", key });
}


/**
 * \brief Reads a value of type `T` from the "Sprite" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The base key for the value in the "Sprite" section.
 * \param index The index appended to the key for accessing specific values.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromSprite(const std::string& tableName, const std::string& key, int index) {
    // Construct the full key for the collider
    std::string fullKey = key + "_" + std::to_string(index);

    return LuaRead<T>(tableName, { "Sprite", {fullKey} });
}


/**
 * \brief Reads a value of type `T` from the "Transform" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Transform" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromTransform(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "Transform", key });
}


/**
 * \brief Reads a value of type `T` from the "RigidBody" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "RigidBody" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromCollider(const std::string& tableName, const std::string& key, int index) {

    std::string fullKey = key + "_" + std::to_string(index);

    // Read the value using the existing LuaRead function
    return LuaRead<T>(tableName, { "Collider", {fullKey} });
}


/**
 * \brief Reads a value of type `T` from the "Collider" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The base key for the value in the "Collider" section.
 * \param index The index appended to the key for accessing specific values.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromRigidBody(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "RigidBody", key });
}


/**
 * \brief Reads a value of type `T` from the "AIState" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The base key for the value in the "AIState" section.
 * \param index The index appended to the key for accessing specific values.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromAIState(const std::string& tableName, const std::string& key, int index) {

    std::string fullKey = key + "_" + std::to_string(index);
    return LuaRead<T>(tableName, { "AIState", {fullKey} });
}


/**
 * \brief Reads a value of type `T` from the "PlayerController" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "PlayerController" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromPlayer(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "PlayerController", key });
}



/**
 * \brief Reads a value of type `T` from the "Text" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Text" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromText(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "Text", key });
}


/**
 * \brief Reads a value of type `T` from the "TextUI" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "TextUI" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromTextUI(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "TextUI", key });
}

/**
 * \brief Reads a value of type `T` from the "Health" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Health" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromSpriteUI(const std::string& tableName, const std::string& key){
    return LuaRead<T>(tableName, { "SpriteUI", key });
}


/**
 * \brief Reads a value of type `T` from the "Spawner" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Spawner" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromHealth(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "Health", key });
}



/**
 * \brief Reads a value of type `T` from the "SpriteUI" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "SpriteUI" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromSpawner(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "Spawner", key });
}


/**
 * \brief Reads a value of type `T` from the "Button" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "Button" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromButton(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "ButtonComponent", key });
}


/**
 * \brief Reads a value of type `T` from the "UI" section of the specified Lua table.
 * \tparam T The type of the value to retrieve.
 * \param tableName The name of the Lua table.
 * \param key The key to access the value within the "UI" section.
 * \return The retrieved value of type `T`.
 */
template<typename T>
T LuaManager::LuaReadFromUI(const std::string& tableName, const std::string& key) {
	return LuaRead<T>(tableName, { "UIComponent", key });
}


template<typename T>
T LuaManager::LuaReadFromExplosion(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "Explosion", key });
}

template<typename T>
T LuaManager::LuaReadFromSplitting(const std::string& tableName, const std::string& key) {
    return LuaRead<T>(tableName, { "Splitting", key });
}
//Similar Templates that works the same========================================================================



/**
 * \brief Reads a value of type `T` from a Lua table using a table name and nested keys.
 *
 * This function starts with the base Lua table specified by `tableName` and traverses
 * the nested structure using the provided `keys` vector. It returns the value of type `T`
 * located at the specified path. If the table, key, or type is invalid, an exception
 * is thrown, and a default value of type `T` is returned.
 *
 * \tparam T The type of the value to retrieve from the Lua table.
 * \param tableName The name of the base table in the Lua script.
 * \param keys A vector of keys specifying the path to the desired value in nested tables.
 * \return The value of type `T` found at the specified location in the Lua table.
 * \throws std::runtime_error If the table or key is not found, or the value is of an incompatible type.
 *
 * \note If an error occurs, the function logs the error and returns a default-constructed value of type `T`.
 *
 * Example usage:
 * \code{.cpp}
 * LuaManager luaManager;
 * std::vector<std::string> keys = {"level1", "level2", "myValue"};
 * int value = luaManager.LuaRead<int>("myTable", keys);
 * \endcode
 */
template<typename T>
T LuaManager::LuaRead(const std::string& tableName, const std::vector<std::string>& keys) {
    try {
        // Start with the base table
        sol::table currentTable = lua[tableName];

        // Log the base table for debugging
        if (!currentTable) {
            throw std::runtime_error("Table '" + tableName + "' not found.");
        }

        // Traverse the nested tables using the provided keys
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];

            // Check if the current table contains the next key
            sol::optional<sol::table> nextTable = currentTable[key];

            // If this is the last key, break the loop
            if (i == keys.size() - 1) {
                sol::optional<T> result = currentTable[key];
                if (result) {
                    return result.value(); // Safe to access
                }
                else {
                    throw std::runtime_error("Key '" + key + "' not found or invalid type in table '" + tableName + "'");
                }
            }

            // If we have not reached the last key, we need to go deeper
            if (!nextTable) {
                throw std::runtime_error("Key '" + key + "' not found in table '" + tableName + "'");
            }

            currentTable = *nextTable; // Move to the next level
        }

    }
    catch (const sol::error& e) {
		ImGuiConsole::Cout("Lua error: %s", e.what());
    }
    catch (const std::bad_optional_access& e) {
		ImGuiConsole::Cout("Bad optional access: %s", e.what());
    }
    catch (const std::runtime_error& e) {
		ImGuiConsole::Cout("Runtime error: %s", e.what());
    }

    return T(); // Returns default if an error is found
}




/**
 * \brief Appends values to a new nested table inside the main table if not found.
 *        If the nested table exists, it updates or adds key-value pairs within it.
 * \param tableName The name of the main table in the Lua file.
 * \param values A container of values to write to the Lua file.
 * \param keys A vector of keys corresponding to the values.
 * \param nestedTableName The name of the nested table to update or create.
 */
inline void LuaManager::LuaWrite(const std::string& tableName, const LuaValueContainer& values, const std::vector<std::string>& keys, const std::string& nestedTableName) {
    if (fileContent.empty()) {
		ImGuiConsole::Cout("No Lua file content loaded.");
        return;
    }

    bool insideTable = false;
    bool insideNestedTable = false;
    auto mainTableIt = fileContent.end(); // Position of the main table
    auto nestedTableIt = fileContent.end(); // Position of the nested table

    size_t braceCounter = 0; // Counter for curly braces

    // First pass: Look for the main table
    for (auto it = fileContent.begin(); it != fileContent.end(); ++it) {
        std::string& line = *it;

        // Trim whitespace from the line
        std::string trimmedLine = trimWhitespace(line);

        // Check for the main table
        if (trimmedLine.find(tableName + " = {") != std::string::npos) {
            insideTable = true;
            mainTableIt = it; // Remember the position of the main table
            braceCounter++; // Increment for the opening brace
            //ImGuiConsole::Cout("Found main table: " << tableName); // Debugging line
        }

        // If we are inside the main table, check for the nested table
        if (insideTable) {
            // Check for existing nested table
            std::string searchString = nestedTableName + " = {";
            //ImGuiConsole::Cout("Checking for nested table: " << searchString); // Debugging line
            if (trimmedLine == searchString) { // Check for exact match
                insideNestedTable = true;
                nestedTableIt = it; // Remember the position of the nested table
                //ImGuiConsole::Cout("Found nested table: " << nestedTableName); // Debugging line
            }

            // Update brace counter
            braceCounter += std::count(trimmedLine.begin(), trimmedLine.end(), '{'); // Count opening braces
            braceCounter -= std::count(trimmedLine.begin(), trimmedLine.end(), '}'); // Count closing braces

            // If the brace counter goes back to zero, we've found the end of the main table
            if (braceCounter == 0) {
                insideTable = false; // End of the main table
                //ImGuiConsole::Cout("End of main table detected for: " << tableName); // Debugging line
                break; // Exit loop after processing the main table
            }
        }
    }

    // If the main table was found
    if (mainTableIt != fileContent.end()) {
        // If the nested table was found, update existing entries
        if (insideNestedTable) {
            for (size_t index = 0; index < values.size(); ++index) {
                const LuaValue& value = values[index];
                const std::string& keyName = keys[index];

                // Prepare the updated entry
                std::string updatedEntry = "        " + keyName + " = ";
                FindType(updatedEntry, value);
                updatedEntry += ","; // Add a comma for formatting

                // Check if the key already exists in the nested table
                bool keyFound = false;

                for (auto entryIt = nestedTableIt + 1; entryIt != fileContent.end(); ++entryIt) {
                    // If we find the end of the nested table, we should stop
                    if (entryIt->find("}") != std::string::npos) {
                        break; // End of the nested table
                    }

                    // Check for existing keys to update
                    if (entryIt->find(keyName + " = ") != std::string::npos) {
                        *entryIt = updatedEntry; // Update existing entry
                        keyFound = true;
                        //ImGuiConsole::Cout("Updated existing entry in nested table: " << updatedEntry); // Debugging line
                        break;
                    }
                }

                // If the key was not found, insert the new entry
                if (!keyFound) {
                    // Insert just after the nested table declaration
                    fileContent.insert(nestedTableIt + 1, updatedEntry);
                    //ImGuiConsole::Cout("Inserted entry in existing nested table: " << updatedEntry); // Debugging line
                }
            }
        }
        else {
            // If the nested table was not found, create it inside the main table
            std::string newEntry = "    " + nestedTableName + " = {\n";
            for (size_t index = 0; index < values.size(); ++index) {
                const LuaValue& value = values[index];
                const std::string& keyName = keys[index];

                // Prepare the updated entry
                std::string entry = "        " + keyName + " = ";
                FindType(entry, value);
                entry += ",\n"; // Add a newline for formatting
                newEntry += entry; // Add the entry to the new table
            }
            newEntry += "    },\n"; // Close the nested table

            // Insert the nested table after the main table declaration
            fileContent.insert(mainTableIt + 1, newEntry); // Insert one line below the main table
            //ImGuiConsole::Cout("Created new nested table: " << nestedTableName); // Debugging line
        }
    }
    else {
        // If the main table was not found, create a new one
        std::string newTable = tableName + " = {\n" + "    " + nestedTableName + " = {\n"; // Added indentation here
        for (size_t index = 0; index < values.size(); ++index) {
            const LuaValue& value = values[index];
            const std::string& keyName = keys[index];

            // Prepare the updated entry with consistent indentation
            std::string newEntry = "        " + keyName + " = ";
            FindType(newEntry, value);
            newEntry += ",\n"; // Add a newline for formatting
            newTable += newEntry; // Add the entry to the new table
        }
        newTable += "    },\n"; // Close the nested table
        newTable += "}\n"; // Close the main table
        fileContent.push_back(newTable); // Add new table at the end of fileContent
        //ImGuiConsole::Cout("Created new main table with nested table: " << tableName); // Debugging line
    }

    // Save changes to the file
    writeFile(currentLuaFilePath, fileContent);  // Save to the specified Lua file
}
