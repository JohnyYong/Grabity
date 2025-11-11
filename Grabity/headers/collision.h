/*!****************************************************************
\file:  Collision.h
\author: Goh Jun Jie, g.junjie, 2301293
\brief: Functions for collision detection and response.
		AABBvsAABB (static/dynamic)
		CirclevsCircle (static/dynamic)
		CricelvsAABB (static)
		Collision Update

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include "Maths.h"
#include "TransformComponent.h"
#include "PlayerControllerComponent.h"



struct AABB {
	Vector2 min; 
	Vector2 max; 
	Vector2 center;

	Vector2 GetCenter() const {
		// Calculate the center as the midpoint between min and max
		return (min + max) * 0.5f;
	}

	float GetWidth() const {
		return max.x - min.x;
	}

	float GetHeight() const {
		return max.y - min.y;
	}
	AABB() : min(0, 0), max(0, 0) {}

	// Constructor to easily create an AABB from two points
	AABB(Vector2 minPoint, Vector2 maxPoint)
		: min(minPoint), max(maxPoint) {}

	// Check if two AABBs overlap
	bool CheckCollision(const AABB& other) const {
		return min.x < other.max.x &&
			max.x > other.min.x &&
			min.y < other.max.y &&
			max.y > other.min.y;
	}
};

struct Circle {
	Vector2 center;  // Position of the circle (px, py)
	float radius;    // Radius of the circle
};

/*!****************************************************************
\fn AABB CreateAABB(TransformComponent* transform)
\brief Creates an axis-aligned bounding box (AABB) based on the
	   transform component's position and scale.
\param transform Pointer to the TransformComponent of the object.
\return An AABB structure containing min and max bounds.
*******************************************************************/
AABB CreateAABB(TransformComponent* transform);

/*!****************************************************************
\fn Circle CreateCircle(TransformComponent* transform)
\brief Creates a circle collider based on the transform component.
\param transform Pointer to the TransformComponent of the object.
\return A Circle structure containing the center position and radius.
*******************************************************************/
Circle CreateCircle(TransformComponent* transform);

/*!****************************************************************
\fn Vector2 CalculateAABBPenetration(const AABB& first, const AABB& second)
\brief Calculates the penetration vector between two overlapping AABBs.
\param first The first AABB.
\param second The second AABB.
\return A vector representing the depth and direction of penetration.
*******************************************************************/
Vector2 CalculateAABBPenetration(const AABB& first, const AABB& second);

/*!****************************************************************
\fn void ResolveCollision(GameObject* obj1, GameObject* obj2, const Vector2& penetration)
\brief Resolves the collision between two GameObjects by adjusting
	   their positions and applying knockback if one is a player.
\param obj1 Pointer to the first GameObject.
\param obj2 Pointer to the second GameObject.
\param penetration The penetration vector between the two objects.
*******************************************************************/
void ResolveCollision(GameObject * obj1, GameObject* obj2, const Vector2& penetration);



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
void HandlePlayerCollidingWithAI(GameObject* obj1, GameObject* obj2, const Vector2& penetration);


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
void HandleProjectileCollidingWithAI(GameObject* obj1, GameObject* obj2, const Vector2& penetration);


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
void HandlePlayerCollidingWithStatic(GameObject* obj1, GameObject* obj2, const Vector2& penetration);


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
void HandleAICollidingWithAI(GameObject* obj1, GameObject* obj2, const Vector2& penetration);

/*!****************************************************************
\fn bool TestAABBAABB(AABB a, AABB b)
\brief Tests for static intersection between two AABBs.
\param a The first AABB.
\param b The second AABB.
\return True if the AABBs intersect, false otherwise.
*******************************************************************/
bool TestAABBAABB(AABB a, AABB b);

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
	float& firstTimeOfCollision); //Output: the calculated value of tFirst, must be returned here

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
bool Collision_CircleCircle(
	const Circle& c1,           // Input: Circle 1
	const Vector2& vel1,        // Input: Velocity of Circle 1
	const Circle& c2,           // Input: Circle 2
	const Vector2& vel2,        // Input: Velocity of Circle 2
	float& firstTimeOfCollision // Output: Time of first collision
);

/*!****************************************************************
\fn bool Collision_CircleRect(const Circle& circle, const AABB& aabb)
\brief Tests for collision between a circle and an AABB.
\param circle The circle to test.
\param aabb The AABB to test against.
\return True if the circle and AABB collide, false otherwise.
*******************************************************************/
bool Collision_CircleRect(
	const Circle& circle,       // Input: Circle
	const AABB& aabb           // Input: Axis-Aligned Bounding Box
	);

/*!****************************************************************
\fn bool PointvsAABB(const Vector2& point, const AABB& box)
\brief Tests if a point lies within an axis-aligned bounding box (AABB).
\param point The point to test.
\param box The AABB to test against.
\return True if the point is inside the AABB, false otherwise.
*******************************************************************/
bool PointvsAABB(const Vector2& point, const AABB& box);

/*!****************************************************************
\fn void CollisionUpdate()
\brief Updates collision states for all game objects.
	   Uses spatial partitioning if enabled, otherwise performs a brute-force check.
*******************************************************************/
void CollisionUpdate();


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
void AssignObjectsToGrid(std::vector<std::string> allActiveLayer, std::unordered_map<int, std::vector<GameObject*>>& grid);



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
void DetectCollisionsUsingGrid(const std::unordered_map<int, std::vector<GameObject*>>& grid, std::vector<std::tuple<GameObject*, GameObject*, Vector2>>& collisions);






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
void ApplyKnockback(RigidBodyComponent* player, const Vector2& direction, float strength, float duration);



