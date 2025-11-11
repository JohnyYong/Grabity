/*!****************************************************************
\file: AnimatorComponent.h
\author: Teng Shi Heng, shiheng.teng, 2301269

\brief
    The AnimatorComponent class is responsible for handling animations
    within a GameObject. It utilizes an AnimationController to manage
    different animation states and update the corresponding SpriteComponent.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#pragma once

#include "Component.h"
#include "AnimationController.h"
#include "SpriteComponent.h"

/*!****************************************************************
\brief
    The AnimatorComponent class manages animation states for a
    GameObject by using an AnimationController and updating the
    corresponding SpriteComponent.
*******************************************************************/
class AnimatorComponent : public Component {
public:
    /*!****************************************************************
    \brief
        Default constructor for AnimatorComponent.
    *******************************************************************/
    AnimatorComponent();

    /*!****************************************************************
    \brief
        Constructor that assigns the animator to a parent GameObject.
    \param parent
        Pointer to the parent GameObject.
    *******************************************************************/
    AnimatorComponent(GameObject* parent);

    /*!****************************************************************
    \brief
        Destructor for AnimatorComponent.
    *******************************************************************/
    ~AnimatorComponent();

    /*!****************************************************************
    \brief
        Updates the animation state each frame.
    *******************************************************************/
    void Update() override;

    /*!****************************************************************
    \brief
        Serializes the animator data to a Lua file.
    \param luaFilePath
        Path to the Lua script file.
    \param tableName
        Name of the Lua table where the data will be stored.
    *******************************************************************/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Deserializes the animator data from a Lua file.
    \param luaFilePath
        Path to the Lua script file.
    \param tableName
        Name of the Lua table from which the data will be loaded.
    *******************************************************************/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Provides debugging information for the animator.
    \return
        A string containing debug information.
    *******************************************************************/
    std::string DebugInfo() const override;

    /*!****************************************************************
    \brief
        Sets the animation controller for the component.
    \param animCtrl
        Unique pointer to the AnimationController to be assigned.
    *******************************************************************/
    void SetAnimationController(std::unique_ptr<AnimationController> animCtrl);

    std::unique_ptr<AnimationController> animationController; //!< Animation controller instance
    std::string animationName; //!< Current animation name

private:
    SpriteComponent* spriteComponent; //!< Pointer to the sprite component
};
