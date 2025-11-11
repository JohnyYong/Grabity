/*!****************************************************************
\file:  LuaConfig.cpp
\author: Goh Jun Jie, g.junjie, 2301293
\co-author: 
\brief: Functions for Serialization/Deserialization using Lua.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "LuaConfig.h"
#include "GameObjectFactory.h"

//namespace LuaUtilities {

 /**
 * \brief Loads a scene from a Lua file, creating and linking game objects based on the data in the file.
 *
 * This function clears all existing game objects, reads data from the specified Lua file,
 * creates game objects, and establishes parent-child relationships based on the file's content.
 *
 * \param loadFilePath The file path to the Lua file containing the scene data.
 *
 * The function performs the following steps:
 * - Clears all existing game objects in the `GameObjectFactory`.
 * - Loads game object data (ID, name, and parent ID) from the Lua file.
 * - Sorts the game objects by ID for consistent creation order.
 * - Creates new game objects using `GameObjectFactory::CreateFromLua`.
 * - Links child objects to their respective parent objects based on the parent IDs.
 *
 * \note A parent ID of `-1` indicates that the game object does not have a parent.
 *
 * \throws std::runtime_error If an error occurs while loading or parsing the Lua file.
 *
 * \warning This function assumes that the Lua file is correctly formatted and contains
 * all necessary data for game object creation.
 */
//    void LoadSceneFromLua(const std::string& loadFilePath) {
//        // Clear existing game objects
//        GameObjectFactory& factory = GameObjectFactory::GetInstance();
//        for (const auto& it : factory.GetAllGameObjects()) {
//            GameObject* object = it.second;
//            factory.Despawn(object);
//        }
//
//        factory.Clear();
//        LuaManager luaManager(loadFilePath);
//
//        // Extract id-name map along with parent IDs
//        std::unordered_map<int, std::pair<std::string, int>> objectData = luaManager.extractNamesWithParentIDs();
//
//        // Collect and sort IDs
//        std::vector<int> sortedIds;
//        for (const auto& pair : objectData) {
//            sortedIds.push_back(pair.first);
//        }
//        std::sort(sortedIds.begin(), sortedIds.end());
//
//        // Create GameObjects from Lua data
//        std::unordered_map<int, GameObject*> createdObjects;
//        for (const int objectId : sortedIds) {
//            const auto& [objectName, parentID] = objectData[objectId];
//            //ImGuiConsole::Cout("Creating GameObject with ID: " << objectId
//            //    << ", Name: " << objectName
//            //    << ", Parent ID: " << parentID);
//
//            GameObject* newObject = factory.CreateFromLua(loadFilePath, objectName);
//            createdObjects[objectId] = newObject;
//        }
//
//        // Link child objects to their parents
//        for (const int objectId : sortedIds) {
//            const auto& [objectName, parentID] = objectData[objectId];
//
//            if (parentID != -1) { // -1 indicates no parent
//                GameObject* child = createdObjects[objectId];
//                GameObject* parent = createdObjects[parentID];
//                if (child && parent) {
//                    child->isDeserializing = true;
//                    child->SetParent(parent); // Use SetParent to establish hierarchy
//                    child->isDeserializing = false;
//
//                    //ImGuiConsole::Cout("Linked child " << child->GetName()
//                    //    << " to parent " << parent->GetName());
//                }
//                else {
//#ifdef _LOGGING
//                    ImGuiConsole::Cout("Error: Could not find parent or child for ID: " << objectId);
//#endif // _LOGGING
//                }
//            }
//        }
//    }
//
//}









/**
 * \brief Constructs a LuaManager and initializes the Lua state with a specified file.
 * \param luaFilePath The path to the Lua file to be managed.
 */
LuaManager::LuaManager(const std::string& luaFilePath) {
    lua.open_libraries(sol::lib::base);
    LuaLoadFile(luaFilePath);
    currentLuaFilePath = luaFilePath;

    // Load the file content into memory once during initialization
    fileContent = readFile(luaFilePath);
    if (fileContent.empty()) {
#ifdef _LOGGING
        ImGuiConsole::Cout("Failed to read Lua file or file is empty.");
#endif // _LOGGING
    }
}



/**
 * \brief Loads a Lua script from a file into the Lua state.
 * \param luaFilePath The path to the Lua file to load.
 */
void LuaManager::LuaLoadFile(const std::string& luaFilePath) {
    try {
        lua.safe_script_file(luaFilePath); // Load file
    }
    catch (const sol::error& e) {
#ifdef _LOGGING
		ImGuiConsole::Cout("Lua error: %s", e.what());
#else
        (void)e;
#endif // _LOGGING
    }
}


/**
 * \brief Reads audio clip IDs from a Lua table and stores them in a vector.
 * \param tableName The name of the Lua table containing audio clip information.
 * \param audioClipIDs A reference to a vector to store the retrieved audio clip IDs.
 */
void LuaManager::LuaReadFromAudioClips(const std::string& tableName, std::vector<int>& audioClipIDs) {
    lua_getglobal(lua, tableName.c_str());  // Push the main table onto the stack
    if (lua_istable(lua, -1)) {
        lua_getfield(lua, -1, "Audio");  // Push the Audio table onto the stack
        if (lua_istable(lua, -1)) {
            int index = 0;
            while (true) {
                // Generate the key for the audio clip
                std::string audioClipKey = "audioClip" + std::to_string(index);

                // Get the field with the generated key
                lua_getfield(lua, -1, audioClipKey.c_str());
                if (lua_isnil(lua, -1)) {
                    lua_pop(lua, 1);    // Remove the nil value from the stack
                    break;              // Stop if the field does not exist
                }

                int audioID = static_cast<int>(luaL_checknumber(lua, -1));  // Convert to the desired type
                audioClipIDs.push_back(audioID);  

                lua_pop(lua, 1); 

                ++index;  
            }
        }
        lua_pop(lua, 1);  // Pop the 'Audio' table
    }
    lua_pop(lua, 1);  // Remove the main table from the stack
}

/**
 * \brief Removes leading and trailing whitespace from a string.
 * \param str The input string to trim.
 * \return A trimmed version of the input string.
 */
std::string trimWhitespace(const std::string& str) {
    auto start = str.find_first_not_of(" \t");
    auto end = str.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}



/**
 * \brief Determines the Lua type for a value and appends it to the entry string.
 * \param entry The Lua key-value entry being created.
 * \param value The value whose type needs to be identified.
 */
void LuaManager::FindType(std::string& entry, const LuaValue& value) {
    if (std::holds_alternative<int>(value)) {
        entry += std::to_string(std::get<int>(value));
    }
    else if (std::holds_alternative<float>(value)) {
        std::stringstream stream;
        stream.precision(2);
        stream << std::fixed << std::get<float>(value); 
        entry += stream.str();
    }
    else if (std::holds_alternative<double>(value)) {  // Added handling for double
        std::stringstream stream;
        stream.precision(2);
        stream << std::fixed << std::get<double>(value);  
        entry += stream.str();
    }
    else if (std::holds_alternative<bool>(value)) {
        entry += std::get<bool>(value) ? "true" : "false";
    }
    else if (std::holds_alternative<std::string>(value)) {
        entry += "\"" + std::get<std::string>(value) + "\""; // Ensure strings are quoted
    }
    else {
        entry += "nil"; // Optional: handle unknown types
    }
}

/**
 * \brief Reads the content of a Lua file into a vector of strings.
 * \param luaFilePath The path to the Lua file.
 * \return A vector containing lines of the file as strings.
 */
std::vector<std::string> LuaManager::readFile(const std::string& luaFilePath) {
    std::ifstream inFile(luaFilePath);
    std::vector<std::string> content;
    if (!inFile.is_open()) {
#ifdef _LOGGING
		ImGuiConsole::Cout("Failed to open Lua file.");
#endif // _LOGGING
        return content;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        content.push_back(line);
    }

    inFile.close();
    return content;
}

/**
 * \brief Writes content from a vector of strings into a Lua file.
 * \param luaFilePath The path to the Lua file to write to.
 * \param content The content to write into the file.
 * \return True if the file was successfully written, false otherwise.
 */
bool LuaManager::writeFile(const std::string& luaFilePath, const std::vector<std::string>& content) {
    std::ofstream outFile(luaFilePath);
    if (!outFile.is_open()) {
#ifdef _LOGGING
        ImGuiConsole::Cout("Failed to open Lua file for writing.");
#endif // _LOGGING
        return false;
    }

    for (const std::string& line : content) {
        outFile << line << "\n";  // Ensure each line is written with a newline character
    }

    outFile.close();
    return true;
}

/**
 * \brief Saves Lua content to a new scene file.
 * \param filename The name of the file to create or overwrite.
 * \param luaContent The Lua script content to write to the file.
 */
void LuaManager::SaveNewScene(const std::string& filename, const std::string& luaContent) {
    // Open a file stream to create or overwrite the file
    std::ofstream luaFile(filename);

    // Check if the file stream is valid
    if (!luaFile.is_open()) {
		ImGuiConsole::Cout("Error: Could not create or open the file %s", filename.c_str());
        return;
    }

    // Write Lua content to the file
    luaFile << luaContent;

    // Close the file stream
    luaFile.close();

    //ImGuiConsole::Cout("Data saved to " << filename);
}


/**
 * \brief Extracts object names and IDs from the Lua globals table.
 * \return A map where keys are object IDs and values are their names.
 */
std::unordered_map<int, std::string> LuaManager::extractNames() {
    sol::table globals = lua.globals();
    std::unordered_map<int, std::string> idNameMap;

    // List of names to ignore (standard Lua globals)
    std::unordered_set<std::string> ignoredGlobals = { "_G", "base", "package", "coroutine", "string", "table", "math", "io", "os", "debug", "utf8", "jit", "bit32" };

    for (const auto& pair : globals) {
        sol::object key = pair.first;
        sol::object value = pair.second;

        // Check if the value is a table and the key is a string
        if (value.is<sol::table>() && key.is<std::string>()) {
            std::string objectName = key.as<std::string>();

            // Filter out any standard Lua globals
            if (ignoredGlobals.find(objectName) == ignoredGlobals.end()) {
                // Extract ID from the key
                size_t underscorePos = objectName.find_last_of('_');
                int id = std::stoi(objectName.substr(underscorePos + 1));

                // Extract the name from the inner Name table
                sol::table objectTable = value.as<sol::table>();
                if (objectTable["Name"].valid()) {
                    sol::table nameTable = objectTable["Name"].get<sol::table>();
                    std::string name = nameTable["name"].get<std::string>();

                    // Add to the map
                    idNameMap[id] = name + "_" + std::to_string(id);
                }
            }
        }
    }

    return idNameMap;
}


/**
 * \brief Checks if a sub-table exists within a parent Lua table.
 * \param parentTable The name of the parent table.
 * \param subTable The name of the sub-table to check.
 * \return True if the sub-table exists, false otherwise.
 */
bool LuaManager::TableExists(const std::string& parentTable, const std::string& subTable) {
    // Access the Lua state and load the specified parent table
    sol::optional<sol::table> tableOpt = lua[parentTable];

    // If the parent table doesn't exist, return false
    if (!tableOpt) {
        return false;
    }

    // Check if the sub-table exists within the parent table
    sol::optional<sol::table> subTableOpt = tableOpt.value()[subTable];
    return subTableOpt.has_value();
}


/**
 * \brief Counts the number of custom tables in the Lua globals.
 * \return The number of non-standard tables in the Lua globals.
 */
int LuaManager::countTables() {
    sol::table globals = lua.globals();
    int tableCount = 0;

    // List of names to ignore (standard Lua globals)
    std::unordered_set<std::string> ignoredGlobals = { "_G", "base", "package", "coroutine", "string", "table", "math", "io", "os", "debug", "utf8", "jit", "bit32" };

    for (const auto& pair : globals) {
        sol::object key = pair.first;
        sol::object value = pair.second;

        // Check if the value is a table and the key is a string
        if (value.is<sol::table>() && key.is<std::string>()) {
            std::string objectName = key.as<std::string>();

            // Filter out any standard Lua globals
            if (ignoredGlobals.find(objectName) == ignoredGlobals.end()) {
#ifdef _LOGGING
				ImGuiConsole::Cout("Table: %s\n", objectName.c_str());
#endif // _LOGGING
                ++tableCount; // Increment the count for each valid table
            }
        }
    }

    return tableCount;
}


/**
 * \brief Clears the content of the current Lua file and resets it to an empty configuration.
 */
void LuaManager::ClearLuaFile() {
    // Clear the fileContent vector which holds the Lua file's content
    fileContent.clear();
    fileContent.push_back("--config file");

    // Write the cleared content (empty) back to the Lua file
    if (!writeFile(currentLuaFilePath, fileContent)) {
#ifdef _LOGGING
		ImGuiConsole::Cout("Failed to write empty content to the Lua file.");
#endif // _LOGGING
    }
    else {
#ifdef _LOGGING
		ImGuiConsole::Cout("Lua file cleared successfully.");
#endif // _LOGGING
    }
}


/**
 * \brief Extracts object names, IDs, and parent IDs from the Lua globals table.
 * \return A map where keys are object IDs and values are pairs of object names and parent IDs.
 */
std::unordered_map<int, std::pair<std::string, int>> LuaManager::extractNamesWithParentIDs() {
    sol::table globals = lua.globals();
    std::unordered_map<int, std::pair<std::string, int>> idDataMap;

    // List of names to ignore (standard Lua globals)
    std::unordered_set<std::string> ignoredGlobals = { "_G", "base", "package", "coroutine", "string", "table", "math", "io", "os", "debug", "utf8", "jit", "bit32" };

    for (const auto& pair : globals) {
        sol::object key = pair.first;
        sol::object value = pair.second;

        // Check if the value is a table and the key is a string
        if (value.is<sol::table>() && key.is<std::string>()) {
            std::string objectName = key.as<std::string>();

            // Filter out any standard Lua globals
            if (ignoredGlobals.find(objectName) == ignoredGlobals.end()) {
                // Extract ID from the key
                size_t underscorePos = objectName.find_last_of('_');
                int id = std::stoi(objectName.substr(underscorePos + 1));

                // Extract the name and parentID from the inner Name table
                sol::table objectTable = value.as<sol::table>();
                if (objectTable["Name"].valid()) {
                    sol::table nameTable = objectTable["Name"].get<sol::table>();
                    std::string name = nameTable["name"].get_or<std::string>("");

                    int parentID = -1; // Default value
                    if (nameTable["parentID"].valid() && nameTable["parentID"].is<int>()) {
                        parentID = nameTable["parentID"];
                    }

                    // Add to the map
                    idDataMap[id] = { name + "_" + std::to_string(id), parentID };
                }
            }
        }
    }

    return idDataMap;
}