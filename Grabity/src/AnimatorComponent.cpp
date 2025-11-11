/*!****************************************************************
\file: AnimatorComponent.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269

\brief
    This source file implements the AnimatorComponent class, which
    manages animation states within a GameObject by utilizing an
    AnimationController and updating the associated SpriteComponent.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/

#include "AnimatorComponent.h"
#include "GameObject.h"

/*!****************************************************************
\brief
    Default constructor for AnimatorComponent.
*******************************************************************/
AnimatorComponent::AnimatorComponent()
{
}

/*!****************************************************************
\brief
    Constructor that assigns the animator to a parent GameObject.
\param parent
    Pointer to the parent GameObject.
*******************************************************************/
AnimatorComponent::AnimatorComponent(GameObject* parent) : Component(parent)
{
}

/*!****************************************************************
\brief
    Destructor for AnimatorComponent.
*******************************************************************/
AnimatorComponent::~AnimatorComponent()
{
}

/*!****************************************************************
\brief
    Updates the animation state each frame.
*******************************************************************/
void AnimatorComponent::Update() {
    if (!animationController)
        return;

    if (!spriteComponent) {
        spriteComponent = GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        return;
    }

    // Update animation state
    int currentState = animationController->currentStateIndex;
    animationController->Update();
    if (currentState != animationController->currentStateIndex)
    {
        currentState = animationController->currentStateIndex;
        // Set the sprite's texture to the animation state's texture
        spriteComponent->ChangeSprite(std::move(animationController->states[currentState]->texture->Clone()));
    }
}

/*!****************************************************************
\brief
    Serializes the animator data to a Lua file.
\param luaFilePath
    Path to the Lua script file.
\param tableName
    Name of the Lua table where the data will be stored.
*******************************************************************/
void AnimatorComponent::Serialize(const std::string& luaFilePath, const std::string& tableName)
{
    if (!animationName.empty())
    {
        AnimationController tmp;
        LoadFromLua(tmp, animationName);
        animationController = std::move(std::make_unique<AnimationController>(std::move(tmp)));
    }

    // Serialization testing, save into lua
    LuaManager luaManager(luaFilePath);

    std::vector<std::string> keys;
    LuaManager::LuaValueContainer values;

    keys.push_back("ControllerPathName");
    values.push_back(animationName);

    luaManager.LuaWrite(tableName, values, keys, "Animator");
}

/*!****************************************************************
\brief
    Deserializes the animator data from a Lua file.
\param luaFilePath
    Path to the Lua script file.
\param tableName
    Name of the Lua table from which the data will be loaded.
*******************************************************************/
void AnimatorComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{

    //Load from lua
    LuaManager luaManager(luaFilePath);

    animationName = luaManager.LuaRead<std::string>(tableName, { "Animator", "ControllerPathName" });
    AnimationController tmp;
    LoadFromLua(tmp, animationName);
    animationController = std::move(std::make_unique<AnimationController>(std::move(tmp)));
}

/*!****************************************************************
\brief
    Provides debugging information for the animator.
\return
    A string containing debug information.
*******************************************************************/
std::string AnimatorComponent::DebugInfo() const
{
    return std::string();
}

/*!****************************************************************
\brief
    Sets the animation controller for the component.
\param animCtrl
    Unique pointer to the AnimationController to be assigned.
*******************************************************************/
void AnimatorComponent::SetAnimationController(std::unique_ptr<AnimationController> animCtrl) {
    if (!animCtrl) {
        std::cerr << "Error: Trying to set a null AnimationController!" << std::endl;
        return;
    }

    animationController = std::move(animCtrl);
    std::cout << "Successfully set AnimationController!\n";
}

