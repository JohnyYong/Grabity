/*!****************************************************************
\file:      System.h
\author:    Lee Yu Jie Brandon, l.yujiebrandon, 2301232
\brief:     Defines the abstract base class for systems in the engine.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once

/**
 * @class System
 * @brief Abstract base class for all engine systems.
 *
 * Systems in the engine perform specific tasks like updating physics. 
 * Each derived class must implement the `Update` function to define its behavior.
 */
class System {
public:

    /**
     * @brief Updates the system's behavior.
     *
     * This function must be implemented by all derived classes to perform
     * system-specific updates.
     */
    virtual void Update() = 0;
private:
};
