/*!****************************************************************
\file: AnimationController.h
\author: Teng Shi Heng, shiheng.teng, 2301269

\brief
    This header file defines the AnimationController class and related structures,
    which handle sprite animations, state transitions, and animation parameters.
    The AnimationController allows dynamic state changes and updates animation
    properties using Lua scripting.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#pragma once

#include <vector>
#include <string>
#include <memory>

#include "SpriteAnimation.h"
#include <sol/sol.hpp>

/*!****************************************************************
\brief
    Represents an animation state containing a name, sprite animation,
    and its active status.
*******************************************************************/
struct AnimationState {
    std::string name;                          //!< Name of the animation state
    std::unique_ptr<SpriteAnimation> texture;  //!< Pointer to sprite animation
    bool isActive = false;                     //!< Indicates if the state is currently active
};

/*!****************************************************************
\brief
    Enumeration type for different parameter types used in animation control.
*******************************************************************/
enum ParameterType {
    PARAMETER_FLOAT,  //!< Floating-point parameter
    PARAMETER_BOOL,   //!< Boolean parameter
    PARAMETER_INT     //!< Integer parameter
};

/*!****************************************************************
\brief
    Stores animation parameters with type and value.
*******************************************************************/
struct AnimationParameter {
    ParameterType type; //!< Type of the parameter
    union {
        float floatValue; //!< Floating-point value
        bool boolValue;   //!< Boolean value
        int intValue;     //!< Integer value
    };

    /*!****************************************************************
    \brief
        Default constructor.
    *******************************************************************/
    AnimationParameter() : type(PARAMETER_FLOAT), floatValue(0.0f) {}

    /*!****************************************************************
    \brief
        Constructor for float parameter.
    \param value
        value to be assign to the floatValue.
    *******************************************************************/
    AnimationParameter(float value) : type(PARAMETER_FLOAT), floatValue(value) {}

    /*!****************************************************************
    \brief
        Constructor for bool parameter.
    \param value
        value to be assign to the boolValue.
    *******************************************************************/
    AnimationParameter(bool value) : type(PARAMETER_BOOL), boolValue(value) {}

    /*!****************************************************************
    \brief
        Constructor for int parameter.
    \param value
        value to be assign to the intValue.
    *******************************************************************/
    AnimationParameter(int value) : type(PARAMETER_INT), intValue(value) {}
};

/*!****************************************************************
\brief
    Defines a transition between animation states based on a condition parameter.
*******************************************************************/
struct AnimationTransition {
    int fromStateIndex;                                 //!< Index of the starting state
    int toStateIndex;                                   //!< Index of the target state
    std::pair<std::string, std::string> conditionParam; //!< Condition parameter name and value
    ParameterType type;                                 //!< Type of the condition parameter
    float threshold;                                    //!< Threshold for transition
    float transitionTime;                               //!< Time taken for transition
};

/*!****************************************************************
\brief
    The AnimationController class manages animation states, transitions,
    and updates animation parameters dynamically.
*******************************************************************/
class AnimationController {
public:
    /*!****************************************************************
    \brief
         Default constructor.
    *******************************************************************/
    AnimationController() = default;

    /*!****************************************************************
    \brief
         Virtual destructor to ensure proper cleanup.
    *******************************************************************/
    virtual ~AnimationController() = default;

    std::vector<std::pair<std::string, AnimationParameter>> parameters;
    std::vector<std::unique_ptr<AnimationState>> states;
    std::vector<AnimationTransition> transitions;
    AnimationState* currentState = nullptr;
    int currentStateIndex = 0;

    /*!****************************************************************
    \brief
        Sets an animation parameter value by name.
    \param paramName
        Name of the parameter.
    \param value
        float value to set.
    *******************************************************************/
    void SetParameter(const std::string& paramName, float value);

    /*!****************************************************************
    \brief
        Sets an animation parameter value by name.
    \param paramName
        Name of the parameter.
    \param value
        bool value to set.
    *******************************************************************/
    void SetParameter(const std::string& paramName, bool value);

    /*!****************************************************************
    \brief
        Sets an animation parameter value by name.
    \param paramName
        Name of the parameter.
    \param value
        int value to set.
    *******************************************************************/
    void SetParameter(const std::string& paramName, int value);

    /*!****************************************************************
    \brief
        Retrieves an animation parameter by name.
    \param paramName
        Name of the parameter.
    \return
        The requested animation parameter.
    *******************************************************************/
    AnimationParameter GetParameter(const std::string& paramName);

    /*!****************************************************************
    \brief
        Adds a new animation state to the controller.
    \param name
        Name of the new state.
    \param texture
        unique_ptr sprite animation associated with the state.
    *******************************************************************/
    void AddState(const std::string& name, std::unique_ptr<SpriteAnimation> texture = nullptr);

    /*!****************************************************************
    \brief
        Adds a transition between animation states.
    \param fromStateIndex
        Index of the initial state.
    \param toStateIndex
        Index of the target state.
    \param conditionParam
        Condition parameter name and value.
    \param type
        Type of the condition parameter.
    \param threshold
        Threshold value for transition.
    \param transitionTime
        Duration of the transition.
    *******************************************************************/
    void AddTransition(int fromStateIndex, int toStateIndex,
        std::pair<std::string, std::string> conditionParam,
        ParameterType type, float threshold, float transitionTime);

    /*!****************************************************************
    \brief
        Updates the animation state based on transitions and parameters.
    *******************************************************************/
    void Update();

    /*!****************************************************************
    \brief
        Sets the active animation state by name.
    \param stateName
        Name of the state to activate.
    *******************************************************************/
    void SetState(const std::string& stateName);

    // Move constructor
    AnimationController(AnimationController&& other) noexcept
        : parameters(std::move(other.parameters)),
        states(std::move(other.states)),
        transitions(std::move(other.transitions)),
        currentState(nullptr) { // Ensure currentState doesn't point to moved data
        if (!states.empty()) {
            currentState = states.front().get(); // Set a valid state if possible
        }
    }

    // Move assignment operator
    AnimationController& operator=(AnimationController&& other) noexcept {
        if (this != &other) {
            parameters = std::move(other.parameters);
            states = std::move(other.states);
            transitions = std::move(other.transitions);
            currentState = nullptr; // Prevent dangling pointer
            if (!states.empty()) {
                currentState = states.front().get();
            }
        }
        return *this;
    }

    // Delete the copy constructor and copy assignment operator
    AnimationController(const AnimationController&) = delete;
    AnimationController& operator=(const AnimationController&) = delete;
};

/*!****************************************************************
\brief
    Loads animation data from a Lua script file.
\param controller
    Reference to the animation controller to load data into.
\param luaFilePath
    Path to the Lua script file.
*******************************************************************/
void LoadFromLua(AnimationController& controller, const std::string& luaFilePath);

/*!****************************************************************
\brief
    Saves animation controller data to a Lua script file.
\param controller
    Reference to the animation controller to save.
\param filePath
    Path to the output Lua script file.
*******************************************************************/
void SaveAnimationControllerToLua(const AnimationController& controller, const std::string& filePath);

/*!****************************************************************
\brief
    Renders the animation controller UI for debugging and visualization.
\param controller
    Reference to the animation controller instance.
*******************************************************************/
void RenderAnimationControllerUI(AnimationController& controller);

/*!****************************************************************
\brief
    Manages Lua scripting and provides utilities for reading script data.
*******************************************************************/
class LuaManagerAlpha {
public:
    sol::state lua; //!< Lua state for scripting

    /*!****************************************************************
    \brief
        Constructor that loads a Lua script file.
    \param luaFilePath
        Path to the Lua script file to load.
    *******************************************************************/
    LuaManagerAlpha(const std::string& luaFilePath) {
        lua.open_libraries(sol::lib::base);
        lua.script_file(luaFilePath);
    }

    /*!****************************************************************
    \brief
        Reads a value from a Lua table using keys.
    \tparam T
        Type of the value to read.
    \param tableName
        Name of the Lua table.
    \param keys
        List of keys to search within the table.
    \return
        The retrieved value of type T.
    *******************************************************************/
    template<typename T>
    T LuaRead(const std::string& tableName, const std::vector<std::string>& keys) {
        sol::table table = lua[tableName];
        T value;
        for (const auto& key : keys) {
            if (table[key].valid()) {
                value = table[key].get<T>();
                break;
            }
        }
        return value;
    }
};
