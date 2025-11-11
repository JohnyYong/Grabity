/*!****************************************************************
\file: CanvasComponent.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief:
	The CanvasComponent class function definition

	Shi Heng created the file and functions needed for this class. (50%)
	Jun Jie provided the serialize and deserialize codes. (50%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "CanvasComponent.h"
#include "GLhelper.h"
#include "LuaConfig.h"

// Default constructor for CanvasComponent.
CanvasComponent::CanvasComponent() : isCanvas(true)
{
}

// Constructor for CanvasComponent with parent GameObject.
CanvasComponent::CanvasComponent(GameObject* parent) : Component(parent), isCanvas(true)
{
}

// Destructor for CanvasComponent.
CanvasComponent::~CanvasComponent()
{
}

// Updates the canvas component state.
void CanvasComponent::Update()
{

}

// Serializes the canvas component data to a Lua file.
void CanvasComponent::Serialize(const std::string& luaFilePath, const std::string& tableName)
{
	LuaManager luaManager(luaFilePath);
	// Prepare the keys and values for the Collider table
	std::vector<std::string> keys;
	LuaManager::LuaValueContainer values;

	keys.push_back("isCanvas");
	values.push_back(isCanvas);

	// Call LuaWrite once for all properties in the Collider table
	luaManager.LuaWrite(tableName, values, keys, "Canvas");
}

// Deserializes the canvas component data from a Lua file.
void CanvasComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
	LuaManager luaManager(luaFilePath);
	isCanvas = luaManager.LuaRead<bool>(tableName, { "Canvas", "isCanvas" });
}

// Returns debug information for the canvas component.
std::string CanvasComponent::DebugInfo() const
{
	return std::string();
}
