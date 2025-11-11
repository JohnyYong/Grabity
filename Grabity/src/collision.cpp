/*!****************************************************************
\file:  Collision.cpp
\author: Goh Jun Jie, g.junjie, 2301293
\co-author: Lee Yu Jie Brandon, l.yujiebrandon, 2301232
\brief: Functions for collision detection and response.
		AABBvsAABB (static/dynamic)
		CirclevsCircle (static/dynamic)
		CricelvsAABB (static)
		Collision response for AABB overlap
		Collision Update 

		Brandon contributed (10%) of the code with implementing the float up component for the damage indicator.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "Collision.h"
#include "GLHelper.h"
#include "GameObjectFactory.h"
#include "GameObject.h"
#include "LayerManager.h"
#include "DespawnManager.h"
#include "FloatUpComponent.h"
#include <cassert>
#include <tuple>
#include <ctime>
#include <set>
#include <chrono>

struct FilePayload {
	char path[256];           // Buffer for the file path
	bool isPrefab;           // Flag to distinguish between prefabs and scenes
	bool isTexture;         // Flag for textures
	bool isSound;          // Flag for sound files
	char textureName[256];  // Buffer for texture name
	char soundName[256];   // Buffer for sound name
	int soundID;          // ID of the sound in the AssetManager
};

namespace globVar {
	const float knockBackStrength = 1200.f;
	const float knockBackStrengthSkeleton = 500.f;
	const float knockBackStrengthEnemy = 200.f;
	const float knockBackLockDuration = 0.2f;
	bool damagePopUpAppeared = false;
}

/*! \namespace GRID
	\brief Defines constants and variables for the spatial grid used
		   in collision detection and spatial partitioning.
*/
namespace GRID {
	//Define Grid
	std::unordered_map<int, std::vector<GameObject*>> grid; // Key is a cell index
	const int GRID_WIDTH = 4000;  // Game world width
	const int GRID_HEIGHT = 4000; // Game world height
	const int CELL_WIDTH = 200; // Grid cell width
	const int NUM_CELLS_X = GRID_WIDTH / CELL_WIDTH;
	const int NUM_CELLS_Y = GRID_HEIGHT / CELL_WIDTH;
}



/*!****************************************************************
\fn AABB CreateAABB(TransformComponent* transform)
\brief Creates an axis-aligned bounding box (AABB) based on the
	   transform component's position and scale.
\param transform Pointer to the TransformComponent of the object.
\return An AABB structure containing min and max bounds.
*******************************************************************/
AABB CreateAABB(TransformComponent* transform) {
	AABB box;
	Vector2 halfSize = { transform->GetLocalScale().x * 0.5f, transform->GetLocalScale().y * 0.5f};

	// Calculate min and max
	box.min = { transform->GetLocalPosition().x - halfSize.x, transform->GetLocalPosition().y - halfSize.y};
	box.max = { transform->GetLocalPosition().x + halfSize.x, transform->GetLocalPosition().y + halfSize.y };
	return box;
}


/*!****************************************************************
\fn Circle CreateCircle(TransformComponent* transform)
\brief Creates a circle collider based on the transform component.
\param transform Pointer to the TransformComponent of the object.
\return A Circle structure containing the center position and radius.
*******************************************************************/
Circle CreateCircle(TransformComponent* transform) {
	// Calculate radius based on dimensions for circular shape
	float radius = std::min(transform->GetLocalScale().x, transform->GetLocalScale().y) / 2.0f; // Circle
	return { transform->GetLocalPosition(), radius };
}


/*!****************************************************************
\fn Vector2 CalculateAABBPenetration(const AABB& first, const AABB& second)
\brief Calculates the penetration vector between two overlapping AABBs.
\param first The first AABB.
\param second The second AABB.
\return A vector representing the depth and direction of penetration.
*******************************************************************/
Vector2 CalculateAABBPenetration(const AABB& first, const AABB& second) {

	// Calculate overlap on both axes
	float overlapX = 0;
	float overlapY = 0;

	// Check for X-axis overlap
	if (first.max.x > second.min.x && first.min.x < second.max.x) {
		overlapX = std::min(first.max.x - second.min.x, second.max.x - first.min.x);
	}

	// Check for Y-axis overlap
	if (first.max.y > second.min.y && first.min.y < second.max.y) {
		overlapY = std::min(first.max.y - second.min.y, second.max.y - first.min.y);
	}

	// Log the overlaps
	//ImGuiConsole::Cout("OverlapX: " << overlapX << ", OverlapY: " << overlapY << "\n";

	Vector2 penetration = { 0, 0 };

	if (overlapX > 0 && overlapY > 0) {
		// Determine which axis to resolve
		if (overlapX < overlapY) {
			// Colliding horizontally
			penetration.x = overlapX;
			penetration.y = 0;

			// Determine direction
			if (first.max.x > second.max.x) {
				//ImGuiConsole::Cout("obj1 is to the right of obj2\n";
				penetration.x = -penetration.x; // obj1 is to the right of obj2
			}
			else {
				//ImGuiConsole::Cout("obj1 is to the left of obj2\n";
			}
		}
		else {
			// Colliding vertically
			penetration.x = 0;
			penetration.y = overlapY;

			// Determine direction
			if (first.max.y > second.max.y) {
				penetration.y = -penetration.y; // obj1 is above obj2
				//ImGuiConsole::Cout("obj1 is above obj2\n";
			}
			else {
				//ImGuiConsole::Cout("obj1 is below obj2\n";
			}
		}
	}

	// Log the penetration vector before returning
	//ImGuiConsole::Cout("Penetration before resolution: (" << penetration.x << ", " << penetration.y << ")\n";

	return penetration;

}



/**
 * @brief Handles the collision between a player and an AI-controlled game object.
 *
 * This function resolves the collision by separating the two objects based on the
 * penetration vector, applies knockback to both objects, and handles damage and
 * audio playback effects when applicable. It also ensures that trigger colliders
 * are ignored during collision resolution.
 *
 * @param obj1 Pointer to the first GameObject involved in the collision (potentially the player).
 * @param obj2 Pointer to the second GameObject involved in the collision (potentially the AI).
 * @param penetration The penetration vector indicating the overlap between the two colliders.
 *
 * @note This function assumes both GameObjects have necessary components like
 *       TransformComponent, RigidBodyComponent, HealthComponent, and others
 *       depending on their role (Player or AI).
 *
 * ### Collision Resolution:
 * - If the penetration is greater along the X-axis, the objects are separated along the X-axis.
 * - If the penetration is greater along the Y-axis, the objects are separated along the Y-axis.
 * - Each object is moved by half the penetration distance to resolve the overlap.
 *
 * ### Player-Specific Behavior:
 * - If `obj1` or `obj2` has a PlayerControllerComponent:
 *   - Checks if the opposing object has an AIStateMachineComponent (indicating it's an AI entity).
 *   - If the AI is not a projectile, calculates damage based on its mass and applies it to the player.
 *   - Plays a random hit audio clip if the player has an AudioComponent.
 *   - Applies knockback to the player and the AI in opposite directions.
 *
 * ### AI-Specific Behavior:
 * - If the AI object has the `isProjectile` flag set to true, the collision is ignored.
 * - AI knockback is adjusted based on penetration direction and configurable global variables.
 *
 * @warning Ensure that all components (e.g., HealthComponent, RigidBodyComponent)
 *          are valid pointers before invoking their methods to prevent null-pointer dereferences.
 */
void HandlePlayerCollidingWithAI(GameObject* obj1, GameObject* obj2, const Vector2& penetration) {

	auto* otherCollider = obj2->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
	if (otherCollider->GetTrigger())
		return; //If other collider is in trigger mode, skips


	float absX = std::abs(penetration.x);
	float absY = std::abs(penetration.y);
	float halfPenetrationX = penetration.x / 2.0f;
	float halfPenetrationY = penetration.y / 2.0f;
	auto* transform1 = obj1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
	auto* transform2 = obj2->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
	// Check if either object has a PlayerComponent
	auto* playerComponent1 = obj1->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);
	auto* playerComponent2 = obj2->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);
	// Get Rigidbody components for both objects
	auto* rigidBody1 = obj1->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
	auto* rigidBody2 = obj2->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

	// Handle pushing logic based on the larger penetration axis
	if (absX > absY) {  // Resolve along the X-axis
		if (penetration.x < 0) {  // obj1 is to the right of obj2

			transform1->SetLocalPosition({ transform1->GetLocalPosition().x + std::abs(halfPenetrationX), transform1->GetLocalPosition().y });
			transform2->SetLocalPosition({ transform2->GetLocalPosition().x - std::abs(halfPenetrationX), transform2->GetLocalPosition().y });

		}
		else {  // obj1 is to the left of obj2

			transform1->SetLocalPosition({ transform1->GetLocalPosition().x - std::abs(halfPenetrationX), transform1->GetLocalPosition().y });
			transform2->SetLocalPosition({ transform2->GetLocalPosition().x + std::abs(halfPenetrationX), transform2->GetLocalPosition().y });

		}
	}
	else if (absY > absX) {  // Resolve along the Y-axis
		if (penetration.y < 0) {  // obj1 is above obj2

			transform1->SetLocalPosition({ transform1->GetLocalPosition().x, transform1->GetLocalPosition().y + std::abs(halfPenetrationY) });
			transform2->SetLocalPosition({ transform2->GetLocalPosition().x, transform2->GetLocalPosition().y - std::abs(halfPenetrationY) });

		}
		else {  // obj1 is below obj2

			transform1->SetLocalPosition({ transform1->GetLocalPosition().x, transform1->GetLocalPosition().y - std::abs(halfPenetrationY) });
			transform2->SetLocalPosition({ transform2->GetLocalPosition().x, transform2->GetLocalPosition().y + std::abs(halfPenetrationY) });

		}
	}

	if (playerComponent1) {
		//Determine if obj2 is an enemy by checking if it has an AIStateMachineComponent
		auto* aiComponent2 = obj2->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
		if (aiComponent2) {

			if (!aiComponent2->isProjectile)
			{
				float enemyMass = rigidBody2 ? rigidBody2->GetMass() : 1.0f; //Default mass to 1.0f if RigidBodyComponent is missing

				float baseDamage = 1.0f; //Base damage value
				float damage = baseDamage * (enemyMass / 10.0f); //Adjust divisor to control scaling

				std::srand(static_cast<unsigned int>(std::time(nullptr)));
				//int randomAudioID = 11 + (std::rand() % 2); // Generate either 11 or 12

				obj1->GetComponent<HealthComponent>(TypeOfComponent::HEALTH)->TakeDamage(static_cast<int>(damage));

				//for player
				Vector2 knockbackDirection = (penetration.x < 0) ? Vector2(1.0f, 0.0f) : Vector2(-1.0f, 0.0f);
				knockbackDirection.y = (penetration.y < 0) ? 1.0f : -1.0f;
				ApplyKnockback(rigidBody1, knockbackDirection, globVar::knockBackStrength, globVar::knockBackLockDuration);


				//for enemy
				knockbackDirection = (penetration.x < 0) ? Vector2(-1.0f, 0.0f) : Vector2(1.0f, 0.0f);
				knockbackDirection.y = (penetration.y < 0) ? -1.0f : 1.0f;
				ApplyKnockback(rigidBody2, knockbackDirection, globVar::knockBackStrengthEnemy, globVar::knockBackLockDuration);
			}
		}
	}

	if (playerComponent2) {
		auto* aiComponent1 = obj1->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
		if (aiComponent1) {
			if (!aiComponent1->isProjectile)
			{
				float enemyMass = rigidBody1 ? rigidBody1->GetMass() : 1.0f;

				float baseDamage = 1.0f; //Base damage value
				float damage = baseDamage * (enemyMass / 10.0f); //Adjust divisor to control scaling

				std::srand(static_cast<unsigned int>(std::time(nullptr)));
				//int randomAudioID = 11 + (std::rand() % 2); // Generate either 11 or 12

				//auto* playerAudio = obj1->GetComponent<AudioComponent>(TypeOfComponent::AUDIO);
				//if (playerAudio) {
				//	playerAudio->PlayAudio(randomAudioID);
				//}
				//else {
				//	ImGuiConsole::Cout("Error: AudioComponent is null!");
				//}
				obj2->GetComponent<HealthComponent>(TypeOfComponent::HEALTH)->TakeDamage(static_cast<int>(damage)); //Deal damage to player
				
				//for player
				Vector2 knockbackDirection = (penetration.x < 0) ? Vector2(-1.0f, 0.0f) : Vector2(1.0f, 0.0f);
				knockbackDirection.y = (penetration.y < 0) ? -1.0f : 1.0f;
				ApplyKnockback(rigidBody2, knockbackDirection, globVar::knockBackStrength, globVar::knockBackLockDuration);

				//for enemy
				knockbackDirection = (penetration.x < 0) ? Vector2(1.0f, 0.0f) : Vector2(-1.0f, 0.0f);
				knockbackDirection.y = (penetration.y < 0) ? 1.0f : -1.0f;
				ApplyKnockback(rigidBody1, knockbackDirection, globVar::knockBackStrengthEnemy, globVar::knockBackLockDuration);
			}

		}


	}

}


/**
 * @brief Handles collisions between a player-controlled object and a static object.
 *
 * Resolves the collision by separating the objects along the dominant penetration axis
 * (X or Y). Stops the velocity of the colliding objects and considers trigger colliders
 * to skip collision resolution. The player object is assumed to be moving.
 *
 * @param obj1 The player-controlled object involved in the collision.
 * @param obj2 The static object involved in the collision.
 * @param penetration The penetration vector representing the overlap between the colliders.
 */
void HandlePlayerCollidingWithStatic(GameObject* obj1, GameObject* obj2, const Vector2& penetration) {

	auto* otherCollider = obj2->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
	if (otherCollider->GetTrigger())
		return; //If other collider is in trigger mode, skips


	float absX = std::abs(penetration.x);
	float absY = std::abs(penetration.y);
	auto* transform1 = obj1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
	auto* transform2 = obj2->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
	// Get Rigidbody components for both objects
	auto* rigidBody1 = obj1->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
	auto* rigidBody2 = obj2->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

	// Check if either object has a PlayerComponent
	auto* playerComponent1 = obj1->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);
	auto* playerComponent2 = obj2->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);

	//Check which is the moving object
	bool moveObj1 = false;
	bool moveObj2 = false;

	if (rigidBody1 && rigidBody2) {
		// Determine which object is moving by velocity
		if (playerComponent1) {
			moveObj1 = true;
			//ImGuiConsole::Cout(obj1->GetName() << " is moving!!!\n";
		}
		if(playerComponent2){
			moveObj2 = true;
			//ImGuiConsole::Cout(obj2->GetName() << " is moving!!!\n";
		}
	}

	// If not player-AI interaction, just resolve the collision as usual
	if (absX > absY) {  // Resolve along the X-axis
		if (penetration.x < 0) {  // obj1 is to the right of obj2
			if (moveObj1) {
				transform1->SetLocalPosition({ transform1->GetLocalPosition().x + absX, transform1->GetLocalPosition().y });
			}
			if(moveObj2) {
				transform2->SetLocalPosition({ transform2->GetLocalPosition().x - absX, transform2->GetLocalPosition().y });
			}
		}
		else {
			if (moveObj1) {
				transform1->SetLocalPosition({ transform1->GetLocalPosition().x - absX, transform1->GetLocalPosition().y });
			}
			if (moveObj2) {
				transform2->SetLocalPosition({ transform2->GetLocalPosition().x + absX, transform2->GetLocalPosition().y });
			}
		}
	}
	else if (absY > absX) {  // Resolve along the Y-axis
		if (penetration.y < 0) {  // obj1 is above obj2
			if (moveObj1) {
				transform1->SetLocalPosition({ transform1->GetLocalPosition().x, transform1->GetLocalPosition().y + absY });
			}
			if (moveObj2) {
				transform2->SetLocalPosition({ transform2->GetLocalPosition().x, transform2->GetLocalPosition().y - absY });
			}
		}
		else {
			if (moveObj1) {
				transform1->SetLocalPosition({ transform1->GetLocalPosition().x, transform1->GetLocalPosition().y - absY });
			}
			if (moveObj2) {
				transform2->SetLocalPosition({ transform2->GetLocalPosition().x, transform2->GetLocalPosition().y + absY });
			}
		}
	}

	if (rigidBody1) rigidBody1->SetVelocity({ 0, 0 });
	if (rigidBody2) rigidBody2->SetVelocity({ 0, 0 });


}


/**
 * @brief Handles collision resolution between two AI-controlled objects.
 *
 * Resolves the collision by separating the two objects along the dominant penetration axis
 * (X or Y). The velocity of both objects is stopped upon collision. The function checks
 * the velocity of each object to determine which one is moving and adjusts positions accordingly.
 * If either object has a trigger collider, the collision is skipped for that object.
 *
 * @param obj1 The first AI object involved in the collision.
 * @param obj2 The second AI object involved in the collision.
 * @param penetration The penetration vector representing the overlap between the colliders.
 */
void HandleAICollidingWithAI(GameObject* obj1, GameObject* obj2, const Vector2& penetration) {

	auto* otherCollider = obj2->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
	if (otherCollider->GetTrigger())
		return; //If other collider is in trigger mode, skips


	float absX = std::abs(penetration.x);
	float absY = std::abs(penetration.y);
	//auto* transform1 = obj1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
	//auto* transform2 = obj2->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
	// Get Rigidbody components for both objects
	auto* rigidBody1 = obj1->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
	auto* rigidBody2 = obj2->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

	//Check which is the moving object
	bool moveObj1 = false;
	bool moveObj2 = false;

	// Check if either object has a PlayerComponent
	auto* aIComponent1 = obj1->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
	auto* aIComponent2 = obj2->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);

	if (rigidBody1 && rigidBody2) {
		// Determine which object is moving by velocity
		if (aIComponent1) {
			moveObj1 = true;
		}
		if (aIComponent2) {
			moveObj2 = true;
		}
	}
	


	const float knockbackStrength = 50.0f; // Adjust strength as needed

	if (absX > absY) { // Resolve along the X-axis
		if (penetration.x < 0) { // obj1 is to the right of obj2
			if (moveObj1 && rigidBody1) {
				rigidBody1->SetVelocity({ rigidBody1->GetVelocity().x + knockbackStrength, rigidBody1->GetVelocity().y });
			}
			if (moveObj2 && rigidBody2) {
				rigidBody2->SetVelocity({ rigidBody2->GetVelocity().x - knockbackStrength, rigidBody2->GetVelocity().y });
			}
		}
		else {
			if (moveObj1 && rigidBody1) {
				rigidBody1->SetVelocity({ rigidBody1->GetVelocity().x - knockbackStrength, rigidBody1->GetVelocity().y });
			}
			if (moveObj2 && rigidBody2) {
				rigidBody2->SetVelocity({ rigidBody2->GetVelocity().x + knockbackStrength, rigidBody2->GetVelocity().y });
			}
		}
	}
	else if (absY > absX) { // Resolve along the Y-axis
		if (penetration.y < 0) { // obj1 is above obj2
			if (moveObj1 && rigidBody1) {
				rigidBody1->SetVelocity({ rigidBody1->GetVelocity().x, rigidBody1->GetVelocity().y + knockbackStrength });
			}
			if (moveObj2 && rigidBody2) {
				rigidBody2->SetVelocity({ rigidBody2->GetVelocity().x, rigidBody2->GetVelocity().y - knockbackStrength });
			}
		}
		else {
			if (moveObj1 && rigidBody1) {
				rigidBody1->SetVelocity({ rigidBody1->GetVelocity().x, rigidBody1->GetVelocity().y - knockbackStrength });
			}
			if (moveObj2 && rigidBody2) {
				rigidBody2->SetVelocity({ rigidBody2->GetVelocity().x, rigidBody2->GetVelocity().y + knockbackStrength });
			}
		}
	}
}



/**
 * @brief Handles collision between a projectile and an AI object.
 *
 * This function checks whether one of the objects involved in the collision is a projectile and
 * the other is an AI-controlled object. If so, it applies damage to the AI based on the mass of the
 * projectile and plays appropriate audio effects for the collision. The projectile state is then
 * deactivated, and the projectile's visual and rotational states are reset.
 *
 * @param obj1 The first object involved in the collision, which could be a projectile or AI.
 * @param obj2 The second object involved in the collision, which could be a projectile or AI.
 * @param penetration The penetration vector representing the overlap between the colliders (not used in this function).
 */
void HandleProjectileCollidingWithAI(GameObject* obj1, GameObject* obj2, const Vector2& penetration) {
	auto* aiComponent1 = obj1->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
	auto* aiComponent2 = obj2->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
	// Check if one of the objects is a projectile
	bool proj1 = false;
	bool proj2 = false;
	if (aiComponent1) {
		proj1 = obj1->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE)->isProjectile;
	}
	if (aiComponent2) {
		proj2 = obj2->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE)->isProjectile;
	}
	bool projCollidingWithAI = (proj1 && aiComponent2) || (proj2 && aiComponent1);

	if (projCollidingWithAI) {
		// Determine which object is the projectile and which is the AI
		GameObject* projectileObject = (proj1) ? obj1 : obj2;
		GameObject* aiObject = (proj1) ? obj2 : obj1;

		// Calculate damage based on projectile mass
		int damage = 10;
		auto* rigidBodyComponent = projectileObject->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
		if (rigidBodyComponent) {
			float projectileMass = rigidBodyComponent->GetMass();
			float baseDamage = 10.0f; // Base damage value for a standard projectile
			damage = static_cast<int>(baseDamage * (projectileMass / 10.0f));
		}

		// Calculate exact collision point - use the penetration vector to find the point of contact
		Vector2 collisionPoint;
		auto* projectileTransform = projectileObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
		auto* aiTransform = aiObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

		if (projectileTransform && aiTransform) {
			// Calculate the collision point based on the positions and the penetration
			// This finds the point where the objects first made contact
			Vector2 aiPosition = aiTransform->GetLocalPosition();
			Vector2 projPosition = projectileTransform->GetLocalPosition();

			// When penetration is passed in, we can use it to determine collision point more accurately
			if (penetration.x != 0 || penetration.y != 0) {
				// If we have penetration data, use it to find the contact point
				Vector2 contactNormal = Vector2(penetration.x, penetration.y).Normalize();

				// Get colliders for radius/size info
				auto* aiCollider = aiObject->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
				auto* projCollider = projectileObject->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);

				if (aiCollider && projCollider) {
					// Estimate collision point based on positions and collider bounds
					AABB aiBounds = aiCollider->GetAABB(0);
					AABB projBounds = projCollider->GetAABB(0);

					// Use the midpoint between the closest points of the two colliders
					float aiRadius = (aiBounds.max.x - aiBounds.min.x) / 2.0f;
					float projRadius = (projBounds.max.x - projBounds.min.x) / 2.0f;
					(void)projRadius;
					collisionPoint = aiPosition - contactNormal * aiRadius;
				}
				else {
					// Fallback if we don't have collider info
					collisionPoint = (aiPosition + projPosition) * 0.5f;
				}
			}
			else {
				// Simple fallback - use the midpoint between objects
				collisionPoint = (aiPosition + projPosition) * 0.5f;
			}
		}
		else if (aiTransform) {
			// Fallback to just using AI position if we don't have projectile transform
			collisionPoint = aiTransform->GetLocalPosition();
		}
		else if (projectileTransform) {
			// Fallback to just using projectile position if we don't have AI transform
			collisionPoint = projectileTransform->GetLocalPosition();
		}

		//// Create damage text object
		//GameObjectFactory& factory = GameObjectFactory::GetInstance();


		// Apply damage to the AI
		auto* healthComponent = aiObject->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
		if (healthComponent) {
			healthComponent->TakeDamage(damage);
/*

			globVar::damagePopUpAppeared = false;

			if (!globVar::damagePopUpAppeared)
			{
				//globVar::damagePopUpAppeared = true;
				GameObject* damageText = factory.CreateFromLua("Assets/Lua/Prefabs/GameDmgIndicatorText.lua", "GameDmgIndicatorText_0");

				// Set the position of the text using the transform component
				auto* textTransform = damageText->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
				if (textTransform) {
					textTransform->SetLocalPosition(collisionPoint);
				}

				// Use the TextComponent setter methods to modify the text properties
				auto* textComponent = damageText->GetComponent<TextComponent>(TypeOfComponent::TEXT);
				if (textComponent) {
					// Set the text content to show the damage amount
					textComponent->SetText(std::to_string(damage));

					// Set font properties
					textComponent->SetFont("sleepySans", 1.0f);

					// Set color properties
					textComponent->SetRGB(Vector3(1.0f, 0.2f, 0.2f)); // Red color
					textComponent->SetAlpha(1.0f); // Fully visible


					// Set position as local offset (if needed)
					textComponent->SetPosition(collisionPoint);
				}

				// Add a FloatUpComponent to make the text float upward
				damageText->AddComponent<FloatUpComponent>(TypeOfComponent::FLOATUP);
				auto* floatComp = damageText->GetComponent<FloatUpComponent>(TypeOfComponent::FLOATUP);
				if (floatComp) {
					//std::cout << "FloatUpComponent attached successfully!";
					floatComp->SetSpeed(50.0f);
					floatComp->SetLifetime(1.5f);
				}

				// Schedule for despawn after 2 seconds
				DespawnManager::ScheduleDespawn(damageText, 1.5f);

				//spawning of vfx upon collision
				GameObject* damageVfx = factory.CreateFromLua("Assets/Lua/Prefabs/Hit_Vfx.lua", "Hit_Vfx_0");
				auto* vfxTransform = damageVfx->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
				if (vfxTransform) {
					vfxTransform->SetLocalPosition(collisionPoint);
				}
				auto* vfxParticle = damageVfx->GetComponent<ParticleSystem>(TypeOfComponent::PARTICLE);
				DespawnManager::ScheduleDespawn(damageVfx, vfxParticle->GetLifetime());
			}
*/

		}

		//// Turn off projectile state
		//if (proj1) {
		//	aiComponent1->isProjectile = false;
		//	aiComponent1->GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR)->animationController->SetParameter("ThrowEnemy", false);
		//	aiComponent1->GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR)->animationController->SetParameter("HeldEnemy", false);
		//	aiComponent1->GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->SetLocalRotation(0);
		//	if (obj1->GetTag() == "HeavyEnemy") {
		//		//auto* collider = aiComponent1->GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
		//		//collider->RestoreOriginalColliderBox("HeavyEnemy");

		//		auto* rigidBody2 = obj2->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
		//		Vector2 knockbackDirection = (penetration.x < 0) ? Vector2(-1.0f, 0.0f) : Vector2(1.0f, 0.0f);
		//		knockbackDirection.y = (penetration.y < 0) ? -1.0f : 1.0f;
		//		ApplyKnockback(rigidBody2, knockbackDirection, globVar::knockBackStrength, globVar::knockBackLockDuration);
		//	}
		//	if (obj1->GetTag() == "BombEnemy") {
		//		auto* collider = aiComponent1->GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
		//		collider->RestoreOriginalColliderBox("BombEnemy");
		//	}
		//}
		//if (proj2) {
		//	aiComponent2->GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR)->animationController->SetParameter("ThrowEnemy", false);
		//	aiComponent2->GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR)->animationController->SetParameter("HeldEnemy", false);
		//	aiComponent2->GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->SetLocalRotation(0);
		//	aiComponent2->isProjectile = false;

		//	if (obj2->GetTag() == "HeavyEnemy") {
		//		//auto* collider = aiComponent2->GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
		//		//collider->RestoreOriginalColliderBox("HeavyEnemy");

		//		auto* rigidBody1 = obj1->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
		//		Vector2 knockbackDirection = (penetration.x < 0) ? Vector2(-1.0f, 0.0f) : Vector2(1.0f, 0.0f);
		//		knockbackDirection.y = (penetration.y < 0) ? 1.0f : -1.0f;
		//		ApplyKnockback(rigidBody1, knockbackDirection, globVar::knockBackStrength, globVar::knockBackLockDuration);
		//	}
		//	if (obj2->GetTag() == "BombEnemy") {
		//		auto* collider = aiComponent2->GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
		//		collider->RestoreOriginalColliderBox("BombEnemy");
		//	}
		//}

		// Turn off projectile state
		if (proj1) {
			const std::string& tag1 = obj1->GetTag();
			bool isHeavyEnemy = tag1 == "HeavyEnemy";

			if (!isHeavyEnemy) {
				aiComponent1->isProjectile = false;

				auto* animator = aiComponent1->GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);
				if (animator && animator->animationController) {
					animator->animationController->SetParameter("ThrowEnemy", false);
					animator->animationController->SetParameter("HeldEnemy", false);
				}

				auto* transform = aiComponent1->GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
				if (transform) {
					transform->SetLocalRotation(0);
				}
			}

			auto* collider1 = aiComponent1->GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
			if (collider1) {
				if (isHeavyEnemy) {
					//collider1->RestoreOriginalColliderBox("HeavyEnemy");

					auto* rigidBody2 = obj2->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
					Vector2 knockbackDirection = (penetration.x < 0) ? Vector2(-1.0f, 0.0f) : Vector2(1.0f, 0.0f);
					knockbackDirection.y = (penetration.y < 0) ? -1.0f : 1.0f;
					ApplyKnockback(rigidBody2, knockbackDirection, globVar::knockBackStrengthSkeleton, globVar::knockBackLockDuration);
				}
				else if (tag1 == "BombEnemy") {
					collider1->RestoreOriginalColliderBox("BombEnemy");
				}
			}
		}

		if (proj2) {
			const std::string& tag2 = obj2->GetTag();
			bool isHeavyEnemy = tag2 == "HeavyEnemy";

			if (!isHeavyEnemy) {
				aiComponent2->isProjectile = false;

				auto* animator = aiComponent2->GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);
				if (animator && animator->animationController) {
					animator->animationController->SetParameter("ThrowEnemy", false);
					animator->animationController->SetParameter("HeldEnemy", false);
				}

				auto* transform = aiComponent2->GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
				if (transform) {
					transform->SetLocalRotation(0);
				}
			}

			auto* collider2 = aiComponent2->GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
			if (collider2) {
				if (isHeavyEnemy) {
					//collider2->RestoreOriginalColliderBox("HeavyEnemy");
					auto* rigidBody1 = obj1->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
					Vector2 knockbackDirection = (penetration.x < 0) ? Vector2(-1.0f, 0.0f) : Vector2(1.0f, 0.0f);
					knockbackDirection.y = (penetration.y < 0) ? 1.0f : -1.0f;
					ApplyKnockback(rigidBody1, knockbackDirection, globVar::knockBackStrengthSkeleton, globVar::knockBackLockDuration);
				}
				else if (tag2 == "BombEnemy") {
					collider2->RestoreOriginalColliderBox("BombEnemy");
				}
			}
		}

	}
}

/*!****************************************************************
\fn void ResolveCollision(GameObject* obj1, GameObject* obj2, const Vector2& penetration)
\brief Resolves the collision between two GameObjects by adjusting
	   their positions and applying knockback if one is a player.
\param obj1 Pointer to the first GameObject.
\param obj2 Pointer to the second GameObject.
\param penetration The penetration vector between the two objects.
*******************************************************************/

void ResolveCollision(GameObject* obj1, GameObject* obj2, const Vector2& penetration) {
	// Check if either object has a PlayerComponent
	auto* playerComponent1 = obj1->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);
	auto* playerComponent2 = obj2->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);
	// Check if either object has a AIComponent
	auto* aiComponent1 = obj1->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
	auto* aiComponent2 = obj2->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);

	// Check if one of the objects is a projectile
	bool proj1 = false;
	bool proj2 = false;
	if (aiComponent1) {
		proj1 = obj1->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE)->isProjectile;
	}
	if (aiComponent2) {
		proj2 = obj2->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE)->isProjectile;
	}
	bool projCollidingWithAI = (proj1 && aiComponent2) || (proj2 && aiComponent1);

	// Check if one of the objects is the player and the other has an AIStateMachineComponent
	bool playerCollidingWithAI = (playerComponent1 && aiComponent2) || (playerComponent2 && aiComponent1);


	//Handle checking with static first
	if (!playerCollidingWithAI) {

		//Player colliding with statics (no pushing/bouncing)
		if (playerComponent1 || playerComponent2) {
			HandlePlayerCollidingWithStatic(obj1, obj2, penetration);
		}
		//AIvsAI  /  AIvsStatic
		if(aiComponent1 || aiComponent2){
			HandleAICollidingWithAI(obj1, obj2, penetration);
		}
	}
	else  {
		//Handle PlayervsAI
		HandlePlayerCollidingWithAI(obj1, obj2, penetration);
	}

	if (projCollidingWithAI) {
		HandleProjectileCollidingWithAI(obj1, obj2, penetration);
	}

	if (obj1->GetTag() == "ExplosionVFX")
	{
		auto* health = obj2->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
		if (health)
		{
			ImGuiConsole::Cout("ABC1");
			health->TakeDamage(20);

			// Calculate collision point (use obj2's position)
			Vector2 collisionPoint = obj2->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->GetLocalPosition();

			// Create damage text object
			GameObjectFactory& factory = GameObjectFactory::GetInstance();
			GameObject* damageText = factory.CreateFromLua("Assets/Lua/Prefabs/GameDmgIndicatorText.lua", "GameDmgIndicatorText_0");

			// Set the position using the transform component
			auto* textTransform = damageText->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
			if (textTransform) {
				textTransform->SetLocalPosition(collisionPoint);
			}

			// Configure the text component
			auto* textComponent = damageText->GetComponent<TextComponent>(TypeOfComponent::TEXT);
			if (textComponent) {
				// Set the text content to show the damage amount
				textComponent->SetText("20"); // Hard-coded 20 damage

				// Set font properties
				textComponent->SetFont("sleepySans", 1.0f);

				// Set color properties - use orange/red for explosion damage
				textComponent->SetRGB(Vector3(1.0f, 0.5f, 0.0f)); // Orange color
				textComponent->SetAlpha(1.0f); // Fully visible

				// Set position
				textComponent->SetPosition(collisionPoint);
			}

			// Add a FloatUpComponent
			damageText->AddComponent<FloatUpComponent>(TypeOfComponent::FLOATUP);
			auto* floatComp = damageText->GetComponent<FloatUpComponent>(TypeOfComponent::FLOATUP);
			if (floatComp) {
				floatComp->SetSpeed(50.0f);
				floatComp->SetLifetime(1.5f);
			}

			// Schedule for despawn after 3 seconds
			DespawnManager::ScheduleDespawn(damageText, 2.0f);
		}
	}
	if (obj2->GetTag() == "ExplosionVFX")
	{
		/*auto* health = obj1->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);

		if (health)
		{
			ImGuiConsole::Cout("ABC2");

			health->TakeDamage(20);
		}*/

		auto* health = obj1->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
		if (health)
		{
			ImGuiConsole::Cout("ABC2");
			health->TakeDamage(20);

			// Calculate collision point (use obj1's position)
			Vector2 collisionPoint = obj1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->GetLocalPosition();

			// Create damage text object
			GameObjectFactory& factory = GameObjectFactory::GetInstance();
			GameObject* damageText = factory.CreateFromLua("Assets/Lua/Prefabs/GameDmgIndicatorText.lua", "GameDmgIndicatorText_0");

			// Set the position using the transform component
			auto* textTransform = damageText->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
			if (textTransform) {
				textTransform->SetLocalPosition(collisionPoint);
			}

			// Configure the text component
			auto* textComponent = damageText->GetComponent<TextComponent>(TypeOfComponent::TEXT);
			if (textComponent) {
				// Set the text content to show the damage amount
				textComponent->SetText("20"); // Hard-coded 20 damage

				// Set font properties
				textComponent->SetFont("sleepySans", 1.0f);

				// Set color properties - use orange/red for explosion damage
				textComponent->SetRGB(Vector3(1.0f, 0.5f, 0.0f)); // Orange color
				textComponent->SetAlpha(1.0f); // Fully visible

				// Set position
				textComponent->SetPosition(collisionPoint);
			}

			// Add a FloatUpComponent
			damageText->AddComponent<FloatUpComponent>(TypeOfComponent::FLOATUP);
			auto* floatComp = damageText->GetComponent<FloatUpComponent>(TypeOfComponent::FLOATUP);
			if (floatComp) {
				floatComp->SetSpeed(50.0f);
				floatComp->SetLifetime(1.5f);
			}

			// Schedule for despawn after 3 seconds
			DespawnManager::ScheduleDespawn(damageText, 3.0f);
		}

	}
}

/*!****************************************************************
\fn bool TestAABBAABB(AABB a, AABB b)
\brief Tests for static intersection between two AABBs.
\param a The first AABB.
\param b The second AABB.
\return True if the AABBs intersect, false otherwise.
*******************************************************************/
bool TestAABBAABB(AABB a, AABB b)
{
	// Exit with no intersection if separated along an axis
	if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
	// Overlapping on all axes means AABBs are intersecting
	return true;
}



/*!****************************************************************
\fn bool Collision_RectRect(const AABB& aabb1, const Vector2& vel1,
							const AABB& aabb2, const Vector2& vel2,
							float& firstTimeOfCollision)
\brief Tests for dynamic collision between two moving AABBs.
\param aabb1 The first AABB.
\param vel1 The velocity of the first AABB.
\param aabb2 The second AABB.
\param vel2 The velocity of the second AABB.
\param firstTimeOfCollision Output parameter for the time of collision.
\return True if a collision occurs, false otherwise.
*******************************************************************/
bool Collision_RectRect(
							const AABB& aabb1,            //Input
							const Vector2& vel1,          //Input 
							const AABB& aabb2,            //Input 
							const Vector2& vel2,          //Input
							float& firstTimeOfCollision) //Output: the calculated value of tFirst, must be returned here
{
	Engine& engine = Engine::GetInstance();
	float tfirst = 0.f;								
	float tlast = static_cast<float>(engine.fixedDT * (long long)engine.currentNumberOfSteps);

	//(static collision)----------------------------------------------------------------------------
	if (TestAABBAABB(aabb1, aabb2)) {
		tfirst = tlast = 0.0f;
		return true;
	}
	//(static collision)----------------------------------------------------------------------------

	Vector2 velr = vel2 - vel1;// Calculate relative velocity

//(x-axis)-------------------------------------------------------------------
	if (velr.x < 0) {
		//case 1
		if (aabb1.min.x > aabb2.max.x) { return false; }				//no intersection (case 1)

		//case 4
		if (aabb1.max.x < aabb2.min.x) {
			if (((aabb1.max.x - aabb2.min.x) / velr.x) > tfirst) {	//find the max tfirst between the 2 (greater)
				tfirst = (aabb1.max.x - aabb2.min.x) / velr.x;		//update tFirst
			}

		}
		if (aabb1.min.x < aabb2.max.x) {
			if ((aabb1.min.x - aabb2.max.x) / velr.x < tlast) {		//find the min tlast between the 2 (lesser)
				tlast = (aabb1.min.x - aabb2.max.x) / velr.x;		//update tLast
			}
		}
	}
	else if (velr.x > 0) {

		//case 2
		if (aabb1.min.x > aabb2.max.x) {
			if (((aabb1.min.x - aabb2.max.x) / velr.x) > tfirst) {
				tfirst = (aabb1.min.x - aabb2.max.x) / velr.x;		//update tFirst
			}

		}
		if (aabb1.max.x > aabb2.min.x) {
			if (((aabb1.max.x - aabb2.min.x) / velr.x) < tlast) {
				tlast = (aabb1.max.x - aabb2.min.x) / velr.x;		//update tLast
			}
		}

		//case 3
		if (aabb1.max.x < aabb2.min.x) { return false; }			//no intersection (case 3)
	}
	else { //(velr_x == 0) case 5
		if (aabb1.max.x < aabb2.min.x) { return false; }
		else if (aabb1.min.x > aabb2.max.x) { return false; }
	}

	//case 6
	if (tfirst > tlast) { return false; }							//no collision
	//(x-axis)-------------------------------------------------------------------
	// repeat on the y - axis
	//(y-axis)-------------------------------------------------------------------
	if (velr.y < 0) {
		//case 1
		if (aabb1.min.y > aabb2.max.y) { return false; } //no intersection (case 1)

		//case 4
		if (aabb1.max.y < aabb2.min.y) {
			if (((aabb1.max.y - aabb2.min.y) / velr.y) > tfirst) {
				tfirst = (aabb1.max.y - aabb2.min.y) / velr.y;
			}

		}
		if (aabb1.min.y < aabb2.max.y) {
			if ((aabb1.min.y - aabb2.max.y) / velr.y < tlast) {
				tlast = (aabb1.min.y - aabb2.max.y) / velr.y;
			}
		}
	}
	else if (velr.y > 0) {

		//case 2
		if (aabb1.min.y > aabb2.max.y) {
			if (((aabb1.min.y - aabb2.max.y) / velr.y) > tfirst) {
				tfirst = (aabb1.min.y - aabb2.max.y) / velr.y;
			}

		}
		if (aabb1.max.y > aabb2.min.y) {
			if (((aabb1.max.y - aabb2.min.y) / velr.y) < tlast) {
				tlast = (aabb1.max.y - aabb2.min.y) / velr.y;
			}
		}

		//case 3
		if (aabb1.max.y < aabb2.min.y) { return false; } //no intersection (case 3)
	}
	else { //(velr_x == 0)
		if (aabb1.max.y < aabb2.min.y) { return false; }
		else if (aabb1.min.y > aabb2.max.y) { return false; }
	}

	//case 6
	if (tfirst > tlast) { return false; }
	//(y-axis)-------------------------------------------------------------------

	//set time of first collision
	firstTimeOfCollision = tfirst;
	return true;

}


/*!****************************************************************
\fn bool Collision_CircleCircle(const Circle& c1, const Vector2& vel1,
								 const Circle& c2, const Vector2& vel2,
								 float& firstTimeOfCollision)
\brief Tests for dynamic collision between two moving circles.
\param c1 The first circle.
\param vel1 The velocity of the first circle.
\param c2 The second circle.
\param vel2 The velocity of the second circle.
\param firstTimeOfCollision Output parameter for the time of collision.
\return True if a collision occurs, false otherwise.
*******************************************************************/
bool Collision_CircleCircle(const Circle& c1,
	const Vector2& vel1,
	const Circle& c2,
	const Vector2& vel2,
	float& firstTimeOfCollision) {

	const float EPSILON = 1e-6f; // Small value for comparisons

	// (Static Collision)-------------------------------------------------------------------------------
	float distSq = (c1.center.x - c2.center.x) * (c1.center.x - c2.center.x) +
		(c1.center.y - c2.center.y) * (c1.center.y - c2.center.y);
	float radiusSum = c1.radius + c2.radius;

	if (distSq <= radiusSum * radiusSum) {
		firstTimeOfCollision = 0.0f; // If they are already overlapping
		return true;
	}
	//return false; (works well)
	// (Dynamic Collision)--------------------------------------------------------------------------------
	Vector2 centerDiff = c1.center - c2.center; // Vector between circle centers
	Vector2 relativeVel = vel1 - vel2;           // Relative velocity
	float c = centerDiff.Dot(centerDiff) - radiusSum * radiusSum; // Quadratic constant term

	float a = relativeVel.Dot(relativeVel);
	if (a < EPSILON) return false; // Spheres not moving relative to each other

	float b = relativeVel.Dot(centerDiff);
	if (b >= 0.0f) return false; // Spheres not moving towards each other

	float discriminant = b * b - 4.0f * (a * c);
	if (discriminant < 0.0f) return false; // No real-valued root, spheres do not intersect

	firstTimeOfCollision = (-b - std::sqrt(discriminant)) / (2.0f * a);

	// Ensure time of collision is non-negative and within frame
	if (firstTimeOfCollision < 0.0f || firstTimeOfCollision > 1.0f) return false;
	return true;
}



/*!****************************************************************
\fn bool Collision_CircleRect(const Circle& circle, const AABB& aabb)
\brief Tests for collision between a circle and an AABB.
\param circle The circle to test.
\param aabb The AABB to test against.
\return True if the circle and AABB collide, false otherwise.
*******************************************************************/
bool Collision_CircleRect(
	const Circle& circle,       // Input: Circle
	const AABB& aabb           // Input: AABB
) {
	// Temporary variables to set edges for testing
	float testX = circle.center.x;
	float testY = circle.center.y;

	// Which edge is closest?
	/*
	If the circle is to the RIGHT of the square, check against the LEFT edge.
	If the circle is to the LEFT of the square, check against the RIGHT edge.
	If the circle is ABOVE the square, check against the TOP edge.
	If the circle is to the BELOW the square, check against the BOTTOM edge.	
	*/

	if (circle.center.x < aabb.min.x) {
		testX = aabb.min.x;      // Test left edge
	}         
	else if (circle.center.x > aabb.max.x) {
		testX = aabb.max.x;      // Test right edge
	}   
	if (circle.center.y < aabb.min.y) {
		testY = aabb.min.y;      // Test top edge
	}        
	else if (circle.center.y > aabb.max.y) {
		testY = aabb.max.y;      // Test bottom edge
	}   

	// Get distance from closest edges
	float distX = circle.center.x - testX;
	float distY = circle.center.y - testY;
	float distance = std::sqrt(distX * distX + distY * distY);

	// If the distance is less than the radius there is collision
	return distance <= circle.radius;

}

/*!****************************************************************
\fn bool PointvsAABB(const Vector2& point, const AABB& box)
\brief Tests if a point lies within an axis-aligned bounding box (AABB).
\param point The point to test.
\param box The AABB to test against.
\return True if the point is inside the AABB, false otherwise.
*******************************************************************/
bool PointvsAABB(const Vector2& point, const AABB& box) {
	return (point.x >= box.min.x && point.x <= box.max.x &&
			point.y >= box.min.y && point.y <= box.max.y);
}



/*!****************************************************************
\fn void CollisionUpdate()
\brief Updates collision states for all game objects.
	   Uses spatial partitioning if enabled, otherwise performs a brute-force check.
*******************************************************************/
void CollisionUpdate() {
	//auto start = std::chrono::high_resolution_clock::now();
	if (GameObjectFactory::GetInstance().isSpatial) {
		//std::cout << " Spatial Collision" << std::endl;
		//Spatial partioning version here
		//Uniform grid 

		// Step 1: Assign objects to grid
		AssignObjectsToGrid(LayerManager::GetInstance().GetActiveLayers(), GRID::grid);

		// Step 2: Detect collisions using grid
		std::vector<std::tuple<GameObject*, GameObject*, Vector2>> collisions;
		DetectCollisionsUsingGrid(GRID::grid, collisions);

		// Step 3: Resolve collisions
		std::sort(collisions.begin(), collisions.end(), [](const auto& a, const auto& b) {
			const Vector2& penetrationA = std::get<2>(a);
			const Vector2& penetrationB = std::get<2>(b);
			float depthA = std::abs(penetrationA.x) + std::abs(penetrationA.y);
			float depthB = std::abs(penetrationB.x) + std::abs(penetrationB.y);
			return depthA > depthB;
			});

		std::set<std::pair<GameObject*, GameObject*>> resolvedCollisions;

		for (const auto& collision : collisions) {
			GameObject* object1 = std::get<0>(collision);
			GameObject* object2 = std::get<1>(collision);
			
			//Skip if layer inactive
			//if (!LayerManager::GetInstance().IsLayerActive(object2->GetLayer()) || 
			//	!LayerManager::GetInstance().IsLayerActive(object1->GetLayer()))
			//{
			//	continue;
			//}

			// Skip if already resolved
			if (resolvedCollisions.count({ object1, object2 }) || resolvedCollisions.count({ object2, object1 })) {
				continue;
			}



			Vector2 penetration = std::get<2>(collision);

			RectColliderComponent* collider1 = object1->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
			RectColliderComponent* collider2 = object2->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);

			if (!collider1->GetTrigger() && !collider2->GetTrigger()) {
				ResolveCollision(object1, object2, penetration);

				// Add to resolved collisions set
				resolvedCollisions.insert({ object1, object2 });
			}
		}

	
	}
	else {
		//USING THIS VERSION OF COLLISION UPDATING=======================================================================================================================

		std::vector<std::string> allActiveLayer = LayerManager::GetInstance().GetActiveLayers();
		std::vector<GameObject*> gameObjectMap;
		for (std::string str : allActiveLayer)
		{
			std::vector<GameObject*> layer = LayerManager::GetInstance().GetSpecifiedLayer(str);
			for (GameObject* object : layer)
				gameObjectMap.push_back(object);
		}

		// Container to store detected collisions
		std::vector<std::tuple<GameObject*, GameObject*, Vector2>> collisions;
		//int attemptedCollisionChecks = 0;

		// First Phase: Collision Detection
		for (auto it1 = gameObjectMap.begin(); it1 != gameObjectMap.end(); ++it1) {
			GameObject* object1 = *it1;
			RectColliderComponent* collider1 = object1->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);

			if (!collider1) continue;
			
			//Layer check
			//if (!LayerManager::GetInstance().IsLayerActive(object1->GetLayer()))
			//{
			//	continue;
			//}
			
			for (auto it2 = std::next(it1); it2 != gameObjectMap.end(); ++it2) {
				GameObject* object2 = *it2;
				RectColliderComponent* collider2 = object2->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);

				if (!collider2) continue;

				//if (!LayerManager::GetInstance().IsLayerActive(object2->GetLayer()))
				//{
				//	continue;
				//}

				// Check and store collision details
				if (collider1->CheckCollision(*collider2)) {
					Vector2 penetration = CalculateAABBPenetration(
						collider1->GetAABB(0),
						collider2->GetAABB(0)
					);

					// Store the collision details
					collisions.emplace_back(object1, object2, penetration);

				}
			}
		}
		//ImGuiConsole::Cout("Total collision checks attempted: " << attemptedCollisionChecks);

		// Second Phase: Prioritized Collision Resolution
		std::sort(collisions.begin(), collisions.end(), [](const auto& a, const auto& b) {
			const Vector2& penetrationA = std::get<2>(a);
			const Vector2& penetrationB = std::get<2>(b);
			float depthA = std::abs(penetrationA.x) + std::abs(penetrationA.y);
			float depthB = std::abs(penetrationB.x) + std::abs(penetrationB.y);
			return depthA > depthB; 
			});

		// Resolve collisions in prioritized order
		for (const auto& collision : collisions) {
			GameObject* object1 = std::get<0>(collision);
			GameObject* object2 = std::get<1>(collision);
			const Vector2& penetration = std::get<2>(collision);

			// Resolve only if neither collider is in trigger mode
			RectColliderComponent* collider1 = object1->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
			RectColliderComponent* collider2 = object2->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);

			if (!collider1->GetTrigger() && !collider2->GetTrigger()) {
				ResolveCollision(object1, object2, penetration);

			}
		}
		//Old update code, no spatial partitioning=======================================================================================================================
	
	}




	//auto end = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double> elapsed = end - start;
	//ImGuiConsole::Cout("Elapsed time: " << elapsed.count() << " seconds\n";
}



/*!
 * \brief Assigns game objects to a spatial grid for efficient collision detection.
 *
 * This function iterates through all game objects that have a RectColliderComponent
 * and assigns them to the appropriate grid cells based on their axis-aligned bounding box (AABB).
 * The grid is cleared at the beginning of each frame to ensure up-to-date object placement.
 *
 * \param gameObjectMap A map of game object IDs to their corresponding GameObject pointers.
 * \param grid A reference to the spatial grid, where each cell contains a list of objects.
 */
void AssignObjectsToGrid(std::vector<std::string> allActiveLayer, std::unordered_map<int, std::vector<GameObject*>>& grid) {
	grid.clear(); // Clear previous frame's data

	for (std::string str : allActiveLayer)
	{
		std::vector<GameObject*> layer = LayerManager::GetInstance().GetSpecifiedLayer(str);
		for (GameObject* object : layer)
		{
			RectColliderComponent* collider = object->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);

			if (!collider) continue;

			// Get the AABB of the object
			AABB bounds = collider->GetAABB(0); // Assuming GetAABB(0) returns world-space bounds

			// Calculate the range of cells the AABB overlaps
			int minCellX = static_cast<int>(std::floor(bounds.min.x / GRID::CELL_WIDTH));
			int minCellY = static_cast<int>(std::floor(bounds.min.y / GRID::CELL_WIDTH));
			int maxCellX = static_cast<int>(std::floor(bounds.max.x / GRID::CELL_WIDTH));
			int maxCellY = static_cast<int>(std::floor(bounds.max.y / GRID::CELL_WIDTH));

			// Assign the object to all overlapping cells
			for (int cellY = minCellY; cellY <= maxCellY; ++cellY) {
				for (int cellX = minCellX; cellX <= maxCellX; ++cellX) {
					int cellIndex = cellY * GRID::NUM_CELLS_X + cellX;
					grid[cellIndex].push_back(object);
				}
			}
		}
	}
}


/*!
 * \brief Detects collisions between game objects using a spatial grid.
 *
 * This function iterates through each grid cell and its neighboring cells
 * to check for potential collisions between game objects. It ensures that
 * each collision pair is processed only once using an unordered_set.
 *
 * \param gridHolder A map representing the spatial grid, where each key is a cell index
 *                   and each value is a vector of GameObjects within that cell.
 * \param collisions A reference to a vector storing detected collision pairs along
 *                   with their penetration vectors.
 */
void DetectCollisionsUsingGrid( const std::unordered_map<int, std::vector<GameObject*>>& gridHolder, std::vector<std::tuple<GameObject*, GameObject*, Vector2>>& collisions)
{
	
	//int attemptedCollisionChecks = 0;

	//Offsets for neighbor cells
	const int neighborOffsets[] = {
		0,                             // Current cell
		1,                             // East
		GRID::NUM_CELLS_X,             // South
		GRID::NUM_CELLS_X + 1,         // Southeast
		GRID::NUM_CELLS_X - 1          // Southwest
	};

	// Hash function for unordered_set of pairs
	auto pairHash = [](const std::pair<GameObject*, GameObject*>& pair) {
		return std::hash<GameObject*>()(pair.first) ^ std::hash<GameObject*>()(pair.second);
		};

	// Use unordered_set for processed pairs for O(1) lookup
	std::unordered_set<std::pair<GameObject*, GameObject*>, decltype(pairHash)> processedPairs(0, pairHash);

	for (const auto& cell : gridHolder) {
		int cellIndex = cell.first;
		const auto& objects = cell.second;

		// Iterate through all neighbors (including current cell)
		for (int offset : neighborOffsets) {
			int neighborIndex = cellIndex + offset;

			auto neighborIt = gridHolder.find(neighborIndex);
			if (neighborIt == gridHolder.end()) continue;

			const auto& neighborObjects = neighborIt->second;

			// Check for collisions between objects in the current cell and neighbor cell
			for (GameObject* object1 : objects) {
				auto* collider1 = object1->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
				if (!collider1) continue;

				for (GameObject* object2 : neighborObjects) {
					if (object1 == object2) continue;

					// Ensure collision pair is not processed twice
					GameObject* minObj = std::min(object1, object2);
					GameObject* maxObj = std::max(object1, object2);
					if (!processedPairs.emplace(minObj, maxObj).second) continue;

					auto* collider2 = object2->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
					if (!collider2) continue;

					// Increment attempted collision checks
					//++attemptedCollisionChecks;

					if (collider1->CheckCollision(*collider2)) {
						Vector2 penetration = CalculateAABBPenetration(collider1->GetAABB(0), collider2->GetAABB(0));
						collisions.emplace_back(object1, object2, penetration);
						
					}
				}
			}
		}
	}


	//ImGuiConsole::Cout("Total collision checks attempted: " << attemptedCollisionChecks);

}


/**
 * @brief Applies knockback force to a Rigidbody, simulating a push effect.
 *
 * This function applies a knockback force to the specified Rigidbody in the given direction, with
 * a specified strength and duration. The direction is normalized to ensure consistent knockback
 * strength, and the Rigidbody's velocity is immediately set to the calculated force. Additionally,
 * the knockback effect is activated, and the remaining duration of the effect is set.
 *
 * @param rigidBody A pointer to the Rigidbody component that will be affected by the knockback.
 * @param direction The direction in which the knockback force should be applied. This vector will
 *        be normalized to maintain consistent force strength.
 * @param strength The strength of the knockback, determining how far the object will be pushed.
 * @param duration The duration for which the knockback effect will last, in seconds.
 */
void ApplyKnockback(RigidBodyComponent* rigidBody, const Vector2& direction, float strength, float duration) {

	if (rigidBody) {
		// Normalize the direction to ensure consistent knockback strength
		Vector2 knockbackForce = direction.Normalize() * strength;

		// Apply the force to the player's Rigidbody (assuming you want instant knockback)
		rigidBody->SetVelocity(knockbackForce);

		// Start the knockback effect by setting the duration
		rigidBody->isInKnockback = true;
		rigidBody->knockbackTimeLeft = duration;
	}
		
}