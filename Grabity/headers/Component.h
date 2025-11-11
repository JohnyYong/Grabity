/*!****************************************************************
\file: Component.h
\author: Johny Yong Jun Siang
\par: j.yong\@digipen.edu
\brief: Header file declaring the base class for all components like TransformComponents, SpriteComponent etcs... 
        Includes virtual functions which are overridden by other component classes such as Update(), Serialize(), Deserialize() and DebugInfo()

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include <string>

// The base Component class for all components like Transform, Sprite, etc.
// Each component must implement update and serialize methods.
class GameObject;

enum TypeOfComponent {

    TRANSFORM = 0,
    RECTCOLLIDER,
    RIGIDBODY,
    TEXT,
    AUDIO,
    GRAVITY,
    SPRITE,
    PLAYER,
    AISTATE,
    HEALTH,
    SPAWNER,
    PARTICLE,
    BUTTON,
    UI,
    CANVAS_UI,
    TEXT_UI,
    SPRITE_UI,
    EXPLOSION,
    PAUSEMENUBUTTON,
    ANIMATOR,
    SLIDER,
    FLOATUP,
    //WIP
    SPLITTING,
    VIDEO,
    VFX_FOLLOW
};

class Component {
    GameObject* parentGO;

    bool isActive = true;

public:

    Component() {};
    Component(GameObject* parent) : parentGO(parent) {};
    virtual ~Component() = default;

    // This method updates the state of the component, specific to the component's type.
    virtual void Update() = 0;

    // Serialize method for saving the component's data to a file (using the provided Lua configuration).
    virtual void Serialize (const std::string& luaFilePath, const std::string& tableName) = 0;

    virtual void Deserialize(const std::string& luaFilePath, const std::string& tableName) = 0;

    //Function overriding for all components
    virtual std::string DebugInfo() const = 0;

    void SetActive(const bool state) { isActive = state; }
    const bool GetActive() { return isActive; }

    //This means the gameobject the component is attached to
    GameObject* GetParentGameObject() const { return parentGO; }
};
