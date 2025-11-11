/*!****************************************************************
\file: EventSystem.h
\author: Ridhwan Afandi (mohamedridhwan.b)
\brief  Contains the declaration of GameEventComponent which is the
        struct contatining the data needed for an event of its type.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include "EventSystem.h"

/*!****************************************************************
\brief
	 Defines the different types of events that can occur in the game.
*******************************************************************/
enum class EventType {
	SpawnEnemies,
	Mission,
};

/*!****************************************************************
\brief
	 Defines the different types of enemies that can appear in the game.
*******************************************************************/
enum class EnemyType {
	Light,
	Heavy,
	Bomb,
};

/*!****************************************************************
\brief
	 Represents an event where enemies are spawned in the game.
*******************************************************************/
struct GameEventComponent {
	EventType eventType;

	EnemyType enemyType;
    int numEnemies;
    float startTime;

    // Constructor for convenience, callback can be omitted if not needed.
    GameEventComponent(EnemyType enemyType, int num, float start)
        : eventType(EventType::SpawnEnemies), enemyType(enemyType), numEnemies(num), startTime(start)  {
    }
};

/*!****************************************************************
\brief
	 Represents a mission that tracks enemy eliminations or progress.
*******************************************************************/
struct GameMissionComponent {
	EventType eventType;

	EnemyType enemyType;
	bool isCompleted;
	int numEnemies;
	int totalEnemies;
    
	GameMissionComponent(EnemyType enemyType, int num)
		: eventType(EventType::Mission), enemyType(enemyType), numEnemies(num), isCompleted(false), totalEnemies(num){
			
	}
};