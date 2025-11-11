/*!****************************************************************
\file: PlayerControllerComponent.cpp
\author: Goh Jun Jie, g.junjie, 2301293
\co-author: Johny Yong Jun Siang, j.yong, 2301301
            Teng Shi Heng, shiheng.teng,  2301269
\brief: Contains definitions of functions declared in PlayerControllerComponent.h.
        Serialize and Deserialize functions
        Movement and Knockback functions
        Controls of the playerHand object such as:
          - Transformations
          - Grabbing/Dragging enemy objects
          - Direction following mouse position
        Contains data of held object/enemy which transformation updates based on playerHand's transformation as well.


        Jun Jie contributed (25%) of the code with player movements and serialization/deserialization.
        Johny contributed (65%) of the code with playerHand controls/transformations and shooting and grabbing of interactable objects
        Shi Heng contributed (10%) of the code with adding of vfx when player grabs the enemy and launching of projectile

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "PlayerControllerComponent.h"
#include "glhelper.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "PlayerSceneControls.h"
#include "Camera.h"
#include "ExplosionComponent.h"
#include "AnimationController.h"
#include "ParticleSystem.h"
#include <cstdlib>  // For rand()
#include <ctime>    // For seeding rand()
#include "DespawnManager.h"

static bool grabbingBack = false;
static float footstepTimer = 0.0f;   // Timer for footstep interval
static const float footstepInterval = 0.3f; // Footstep sound interval in seconds

/**
 * @brief Default constructor for PlayerControllerComponent
 */
PlayerControllerComponent::PlayerControllerComponent() :
    particleSystemInitialized(false)
{
    Engine::GetInstance().showCursor = false;
    Engine::GetInstance().fadeIntoCutScene = false;

}
/**
 * @brief Constructor with parent GameObject to initialize the component
 *
 * @param parent Pointer to the parent GameObject
 */
PlayerControllerComponent::PlayerControllerComponent(GameObject* parent)
    : Component(parent), moveSpeed(0.f), playerRigidBody(nullptr), playerSprite(nullptr),
    particleSystemInitialized(false)
{
    Engine::GetInstance().showCursor = false;
    Engine::GetInstance().fadeIntoCutScene = false;

}
/**
 * @brief Constructor to initialize PlayerControllerComponent with movement speed, rigid body, and sprite
 *
 * @param parent Pointer to the parent GameObject
 * @param newMove Initial movement speed
 * @param rigidBody Pointer to the RigidBodyComponent
 * @param sprite Pointer to the SpriteComponent
 */
PlayerControllerComponent::PlayerControllerComponent(GameObject* parent, float newMove, RigidBodyComponent* rigidBody, SpriteComponent* sprite)
    : Component(parent), moveSpeed(newMove), playerRigidBody(rigidBody), playerSprite(sprite),
    particleSystemInitialized(false)
{
    if (changeBGM)
    {
        changeBGM = false;
    }

    if (playBGM)
    {
        playBGM = false;
    }
    Engine::GetInstance().showCursor = false;
    Engine::GetInstance().fadeIntoCutScene = false;

}
PlayerControllerComponent::~PlayerControllerComponent()
{
    AudioManager::GetInstance().StopAudio(1); //Remove walking sound if any
    AudioManager::GetInstance().StopAudio(2); //Remove sound if any
    AudioManager::GetInstance().StopAudio(0); //Remove sound if any
    AudioManager::GetInstance().StopAudio(30); //Remove sound if any
    AudioManager::GetInstance().StopAudio(31); //Remove sound if any
    AudioManager::GetInstance().StopAudio(14);
    AudioManager::GetInstance().StopAudio(13);


    grabbingBack = false;
    draggingObject = nullptr;
    heldObject = nullptr;
    setHand = false;
    handOffset = { 300.f };
    heldOBJOffset = { 200.f };
    ImGuiConsole::Cout("HELLO3\n");
    Engine::GetInstance().cameraManager.GetPlayerCamera().HandleShake(false, false);    //camera shake stops
    changeBGM = false;

    suctionVFX = nullptr;
    trailingVFX.clear();
    AudioManager::GetInstance().RemoveLowPassFilter(AudioManager::GetInstance().bgmChannel);
}
/**
 * @brief Serializes the moveSpeed data to a Lua file
 *
 * @param luaFilePath The path to the Lua file
 * @param tableName The table name in the Lua file
 */
void PlayerControllerComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    std::vector<std::string> keys = { "moveSpeed" };
    LuaManager::LuaValueContainer values = { moveSpeed };

    LuaManager luaManager(luaFilePath);
    luaManager.LuaWrite(tableName, values, keys, "PlayerController");
}

/**
 * @brief Deserializes the moveSpeed data from a Lua file
 *
 * @param luaFilePath The path to the Lua file
 * @param tableName The table name in the Lua file
 */
void PlayerControllerComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);
    moveSpeed = luaManager.LuaReadFromPlayer<float>(tableName, "moveSpeed");
}

/**
 * @brief Updates the player's movement, actions, and interactions based on input and game state
 */
void PlayerControllerComponent::Update() {

    if (!GetActive())
        return;

    GLFWwindow* window = InputManager::ptrWindow;
    (void*)window;
    Vector2 force(0.0f, 0.0f); // Used only if `GameObjectFactory::GetInstance().useForce` is true
    Vector2 velocity(0.0f, 0.0f); // Used only if `GameObjectFactory::GetInstance().useForce` is false
    float currentSpeed = moveSpeed;
    bool walking = false;
    static bool walkingSoundPlaying = false;
    static bool suctionSoundPlaying = false;

    footstepTimer += (float)InputManager::GetDeltaTime();

    SpriteComponent* spriteComponent = GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
    (void)spriteComponent;
    RigidBodyComponent* rbComponent = GetParentGameObject()->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
    AnimatorComponent* animComponent = GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);

    //Control knockback
    //Cannot move when knocking back or grabbing something
    if (!rbComponent->isInKnockback || !grabbingBack) {
        // Accumulate movement input
        if (InputManager::IsKeyDown(GLFW_KEY_W)) {
            if (GameObjectFactory::GetInstance().useForce) force.y += currentSpeed;
            else velocity.y += currentSpeed;
            //spriteComponent->ChangeState(1); // Player is walking
            walking = true;
        }
        if (InputManager::IsKeyDown(GLFW_KEY_S)) {
            if (GameObjectFactory::GetInstance().useForce) force.y -= currentSpeed;
            else velocity.y -= currentSpeed;
            walking = true;
            //spriteComponent->ChangeState(1); // Player is walking
        }
        if (InputManager::IsKeyDown(GLFW_KEY_A)) {
            if (GameObjectFactory::GetInstance().useForce) force.x -= currentSpeed;
            else velocity.x -= currentSpeed;
            walking = true;
            //spriteComponent->ChangeState(1); // Player is walking
            GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)->SetFlipX(false);
        }
        if (InputManager::IsKeyDown(GLFW_KEY_D)) {
            if (GameObjectFactory::GetInstance().useForce) force.x += currentSpeed;
            else velocity.x += currentSpeed;
            walking = true;
            //spriteComponent->ChangeState(1); // Player is walking
            GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)->SetFlipX(true);
        }

        if (InputManager::IsKeyDown(GLFW_KEY_W) || InputManager::IsKeyDown(GLFW_KEY_S) ||
            InputManager::IsKeyDown(GLFW_KEY_A) || InputManager::IsKeyDown(GLFW_KEY_D)) {

            walking = true; // Player is moving

            // Play a footstep sound at intervals while moving
            if (footstepTimer >= footstepInterval) {
                randomFootstepID = 20 + (std::rand() % 10); // Pick a random footstep SFX (20-29)
                AudioManager::GetInstance().PlayAudio(randomFootstepID);
                AudioManager::GetInstance().SetChannelVolume(randomFootstepID, 0.3f);
                footstepTimer = 0.0f; // Reset timer for next step
            }
        }

        // Stop footsteps when movement stops
        if (!walking) {
            footstepTimer = 0.0f; // Reset timer to avoid instant playback when resuming movement
        }

        //if (!walkingSoundPlaying)
        //{
        //    randomFootstepID = 20 + (std::rand() % 10);
        //    AudioManager::GetInstance().PlayAudio(randomFootstepID);
        //    walkingSoundPlaying = true;
        //}

        //if (!walking && spriteComponent) {
        //    //spriteComponent->ChangeState(0); // Player is idle
        //    AudioManager::GetInstance().StopAudio(randomFootstepID);
        //    walkingSoundPlaying = false;
        //}
        animComponent->animationController.get()->SetParameter("IsMoving", walking);
    }

    if ((velocity.x != 0.0f && velocity.y != 0.0f) || (force.x != 0.0f && force.y != 0.0f)) {
        // If using velocity-based movement
        if (!GameObjectFactory::GetInstance().useForce) {
            float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            if (length > 0.0f) {
                velocity.x = (velocity.x / length) * currentSpeed;
                velocity.y = (velocity.y / length) * currentSpeed;
            }
        }
        // If using force-based movement
        else {
            float length = std::sqrt(force.x * force.x + force.y * force.y);
            if (length > 0.0f) {
                force.x = (force.x / length) * currentSpeed;
                force.y = (force.y / length) * currentSpeed;
            }
        }
    }

    RigidBodyComponent* rigidBody = GetParentGameObject()->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
    TransformComponent* transform = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

    //Whether to use force or velocity
    if (GameObjectFactory::GetInstance().useForce && rigidBody) {
        rigidBody->ApplyForce(force);
        rigidBody->Update();
    }
    else if (!GameObjectFactory::GetInstance().useForce && rigidBody) {
        rigidBody->SetVelocity(velocity);
    }

    if (transform && rigidBody) {
        transform->SetLocalPosition(transform->GetLocalPosition() + rigidBody->GetVelocity() * static_cast<float>((long long)Engine::GetInstance().currentNumberOfSteps * Engine::GetInstance().fixedDT));
    }

    if (!GameObjectFactory::GetInstance().useForce && !InputManager::IsKeyDown(GLFW_KEY_W) && !InputManager::IsKeyDown(GLFW_KEY_S) &&
        !InputManager::IsKeyDown(GLFW_KEY_A) && !InputManager::IsKeyDown(GLFW_KEY_D)) {
        if (rigidBody) {
            rigidBody->SetVelocity({ 0, 0 });
        }
    }

    //By Johny, for initially finding hand at game start
    if (!setHand)
    {
        for (auto* child : GetParentGameObject()->GetChildren()) {
            // Find the hand
            if (child->GetName() == "PlayerHand") {
                playerHand = child;

                //Check sprite component exists
                auto* spriteComponentHand = playerHand->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
                if (spriteComponentHand != nullptr) {
                    spriteComponentHand->ChangeSprite(std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite("HandClose"))); //Add hand close sprite at
                }

                //Check if RectColliderComponent exists and set trigger
                auto* rectColliderComponent = playerHand->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                if (rectColliderComponent != nullptr) {
                    rectColliderComponent->SetTrigger(true); //Set as trigger so that collision doesn't push around
                }

                setHand = true; //Successfully initialize, can't add at constructor as the hand might not exist yet as game object
                break;
            }
        }

        // Initialize particle system if not already done
        //if (setHand && !particleSystemInitialized) 
        //{
        //    // Create a new GameObject from Lua for the particles
        //    GameObject* particleObj = GameObjectFactory::GetInstance().CreateFromLua("Assets/Lua/Prefabs/SuctionVFX.lua", "SuctionVFX_0");

        //    if (particleObj) {
        //        // Get the particle system component from the created object
        //        suctionParticles = particleObj->GetComponent<ParticleSystem>(TypeOfComponent::PARTICLE);

        //        if (suctionParticles) {
        //            // Configure for a single persistent particle
        //            suctionParticles->SetDuration(0.1f);        // Very short duration
        //            suctionParticles->SetLooping(false);        // We'll control emission manually
        //            suctionParticles->SetLifetime(3.0f);        // Longer lifetime for persistent effect
        //            suctionParticles->SetSpeed(0.0f);           // No initial speed - we'll position it manually

        //            particleSystemInitialized = true;
        //        }
        //    }
        //}
    }

#ifdef  _IMGUI
    Vector2 mousePosition = PlayerSceneControls::GetInstance().GetMousePositionInGameWorld();

#else

    double mouseX, mouseY;
    glfwGetCursorPos(InputManager::ptrWindow, &mouseX, &mouseY);

    Vector2 mousePosition = { (float)mouseX,(float)mouseY };

    mousePosition = Vector2(mousePosition.x - InputManager::GetWidth() * 0.5f, -mousePosition.y + InputManager::GetHeight() * 0.5f) + Engine::GetInstance().cameraManager.GetPlayerCamera().GetCenter();
#endif // _IMGUI
    //Done by Johny, mainly for hand movement and controls
    Vector2 heldOriginalSize = { 0,0 };
    if (!GetParentGameObject()->GetChildren().empty()) {

        if (playerHand) {
#pragma region HandMovementLogic
            auto* playerHandSprite = playerHand->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);

            if (!grabbingBack) {
                // Get the mouse position in screen space
                TransformComponent* playerTransform = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                TransformComponent* handTransform = playerHand->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

                if (playerTransform && handTransform) {
                    Vector2 playerWorldPosition = playerTransform->GetPosition();

                    Vector2 mousePositionWorld(mousePosition.x, mousePosition.y);

                    Vector2 direction = mousePositionWorld - playerWorldPosition;
                    float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                    if (magnitude > 0) {
                        direction.x /= magnitude; // Normalize X
                        direction.y /= magnitude; // Normalize Y
                    }

                    //Rotate the hand to face the mouse direction
                    float angle = std::atan2(direction.y, direction.x);
                    angle = angle * (180.0f / 3.14159f) - 90.0f; //Adjust to match rotation convention
                    if (angle < 0) {
                        angle += 360.0f;
                    }

                    float currentRotation = handTransform->GetLocalRotation();
                    float smoothRotation = currentRotation + (angle - currentRotation) * 0.1f; // Smooth rotation
                    handTransform->SetLocalRotation(smoothRotation);

                    //Calculate the target local position of the hand relative to the player
                    //Vector2 targetLocalPosition = direction * handOffset; //Local offset from player's position

                    Vector2 playerToMouse = mousePositionWorld - playerWorldPosition;

                    float distanceToMouse = std::sqrt(playerToMouse.x * playerToMouse.x + playerToMouse.y * playerToMouse.y);
                    if (distanceToMouse > handOffset) {
                        playerToMouse.x = (playerToMouse.x / distanceToMouse) * handOffset;
                        playerToMouse.y = (playerToMouse.y / distanceToMouse) * handOffset;
                    }
                    Vector2 targetLocalPosition = playerToMouse;

                    Vector2 currentLocalPosition = handTransform->GetLocalPosition();
                    Vector2 smoothLocalPosition = currentLocalPosition + (targetLocalPosition - currentLocalPosition) * 0.8f; // Smooth movement
                    handTransform->SetLocalPosition(smoothLocalPosition);

                    if (heldObject)
                    {
                        auto* heldTrans = heldObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                        auto* heldCollider = heldObject->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);

                        if (heldCollider)
                        {
                            if (heldObject->GetTag() == "HeavyEnemy") {

                                heldCollider->SetBulletColliderBox("HeavyEnemy");
                            }

                            if (heldObject->GetTag() == "BombEnemy")
                            {
                                heldCollider->SetBulletColliderBox("BombEnemy");
                            }
                        }

                        float heldCurrentRotation = heldTrans->GetLocalRotation();
                        float heldSmoothRotation = heldCurrentRotation + (angle - heldCurrentRotation) * 0.1f; // Smooth rotation
                        heldTrans->SetLocalRotation(heldSmoothRotation);

                        Vector2 targetHeldLocalPosition = direction * heldOBJOffset; //Local offset from player's position
                        Vector2 currentHeldLocalPosition = heldTrans->GetLocalPosition();
                        Vector2 smoothHeldLocalPos = currentHeldLocalPosition + (targetHeldLocalPosition - currentHeldLocalPosition) * 0.1f; // Smooth movement

                        heldObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->SetLocalPosition(smoothHeldLocalPos);

                        Vector4 playerHandSpriteRGBA = playerHandSprite->GetRGB();
                        playerHandSpriteRGBA.w = 0.f;
                        playerHandSprite->SetRGB(playerHandSpriteRGBA); //Set invisible

                    }
                    //// Debugging outputs
                    //ImGuiConsole::Cout("Player World Position: (" << playerWorldPosition.x << ", " << playerWorldPosition.y << ")\n";
                    //ImGuiConsole::Cout("Hand Local Position: (" << smoothLocalPosition.x << ", " << smoothLocalPosition.y << ")\n";

                }
            }

#pragma endregion

            auto* collider = playerHand->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
            //Hand has a collider
            if (collider) {
                //LastColliderOBJ is currently only taken from TriggerEnter 
                GameObject* collidedObject = collider->GetCurrentlyColliding();
#pragma region ShootingLogic
                //If held object is not nullptr, means is holding onto something
                //Begin shooting via mass
                if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && heldObject)
                {
                    // Access held object's collider and rigidbody
                    auto* heldCollider = heldObject->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                    auto* heldRigidBody = heldObject->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
                    auto* playerTransform = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

                    if (heldCollider && heldRigidBody && playerTransform) {

                        heldCollider->SetTrigger(false);
                        heldObject->UnsetParent();
                        auto* aiComponent = heldObject->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                        auto* explosionComponent = heldObject->GetComponent<ExplosionComponent>(TypeOfComponent::EXPLOSION);
                        if (explosionComponent) {
                            explosionComponent->wasShotOut = true;
                        }
                        aiComponent->isProjectile = true;
                        aiComponent->projectileTimer = 0.f;
                        AudioManager::GetInstance().PlayAudio(0);
                        Vector2 shootDirection = mousePosition - playerTransform->GetPosition();
                        float magnitude = std::sqrt(shootDirection.x * shootDirection.x + shootDirection.y * shootDirection.y);

                        if (magnitude > 0) {
                            shootDirection.x /= magnitude; // Normalize X
                            shootDirection.y /= magnitude; // Normalize Y
                        }

                        //Calculate force based on enemy's mass (heavier objects receive less force)
                        constexpr float baseImpulse = 10000.0f; //Base impulse strength
                        Vector2 impulse = shootDirection * (baseImpulse / (heldRigidBody->GetMass() + 1.0f)); // Inverse relationship with mass


                        // Apply the force to the held object's rigidbody
                        heldRigidBody->SetVelocity(impulse);
                        Engine::GetInstance().cameraManager.GetPlayerCamera().HandleShake(false, true);    //small camera shake

                    }

                    //spawning of vfx upon launching
                    GameObject* damageVfx = GameObjectFactory::GetInstance().CreateFromLua("Assets/Lua/Prefabs/Hit_Vfx.lua", "Hit_Vfx_0");
                    auto* vfxTransform = damageVfx->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    if (vfxTransform) {
                        auto* heldTransform = heldObject->GetComponent<TransformComponent>(TRANSFORM);
                        vfxTransform->SetLocalPosition(heldTransform->GetLocalPosition());
                    }
                    auto* vfxParticle = damageVfx->GetComponent<ParticleSystem>(TypeOfComponent::PARTICLE);
                    DespawnManager::ScheduleDespawn(damageVfx, vfxParticle->GetLifetime());

                    heldObject = nullptr;
                    collidedObject = nullptr;
                    collider->SetCurrentlyColliding(nullptr);

                    if (playerHand) {
                        auto* handSprite = playerHand->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
                        if (handSprite) {
                            //handSprite->ChangeState(0); // Set hand to idle state
                            playerHand->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR)->animationController->SetParameter("EnemyGrab", false);
                        }
                    }

                    Vector4 playerHandSpriteRGBA = playerHandSprite->GetRGB();
                    playerHandSpriteRGBA.w = 1.f;
                    playerHandSprite->SetRGB(playerHandSpriteRGBA); //Set visible               
                    }

#pragma endregion
                if (collidedObject)
                {
                    auto* collidedIsEnemy = collidedObject->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                    if (collidedIsEnemy)
                    {
                        //ImGuiConsole::Cout("Collided Enemy: %s", collidedIsEnemy->GetParentGameObject()->GetTag().c_str());
                        if (collidedIsEnemy->GetParentGameObject()->GetTag() == "BombEnemy")
                        {
                            auto* explosiveComponent = collidedObject->GetComponent<ExplosionComponent>(TypeOfComponent::EXPLOSION);
                            if (explosiveComponent)
                            {
                                if (explosiveComponent->GetExplodingSFXState()) //If already blinking
                                {
                                    return; //Cannot be grabbed while blinking
                                }
                            }
                        }
                    }

                    if (!collidedIsEnemy)
                        return;
                }

                getCurrentlyColliding = collidedObject;

                //if (heldObject) {
                //    ImGuiConsole::Cout("Holding: " << heldObject->GetName());
                //}
                //if (collidedObject) {
                //    ImGuiConsole::Cout("Currently Colliding: " << collidedObject->GetName());
                //}
#pragma region SuctionLogic
                //If left mouse button is pressed and there is a collided object, grab it
                if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && collidedObject) {

                    if (!heldObject) //If player is holding nothing
                    {
                        if (collidedObject)
                        {
                            auto* collidedIsEnemy = collidedObject->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);

                            if (collidedIsEnemy && !collidedIsEnemy->isProjectile)
                            {
                                draggingObject = collidedObject; // Update heldObject to the new collided object
                            }
                        }

                    }

                    if (!draggingObject)
                        return;


                    auto* enemySprite = draggingObject->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);

                    if (enemySprite)
                    {
#ifdef _LOGGING
#endif // _LOGGING
                        enemySprite->SetRGB(Vector4(1.f, 1.f, 0.f, 1.f));
                    }

                    if (!suctionSoundPlaying)
                    {
                        AudioManager::GetInstance().PlayAudio(2);
                        suctionSoundPlaying = true;
                    }

                    if (!particleSystemInitialized)
                    {
                        // Create a new GameObject from Lua for the particles
                        suctionVFX = GameObjectFactory::GetInstance().CreateFromLua("Assets/Lua/Prefabs/SuctionVFX.lua", "SuctionVFX_0");
                        if (suctionVFX) 
                        {
                            particleSystemInitialized = true;
                            VfxFollowComponent* vfx = suctionVFX->GetComponent<VfxFollowComponent>(TypeOfComponent::VFX_FOLLOW);
                            if(vfx)
                            {
                                vfx->SetTarget(draggingObject);
                            }
                        }
                    }

                    auto* heldCollider = draggingObject->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                    if (heldCollider) {
                        grabbingBack = true;
                        heldCollider->SetTrigger(true); // Prevent collision while held
                    }

                    //Attach the object to the hand
                    auto* heldTransform = draggingObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    auto* handTransform = playerHand->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    if (heldTransform && handTransform) {
                        heldTransform->SetLocalPosition(handTransform->GetPosition());
                    }

                    //playerHand->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)->ChangeState(1);
                    playerHand->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR)->animationController->SetParameter("EnemyGrab", true);

                    //When holding nothing
                    if (grabbingBack && !heldObject) {

                        // Hand retains its direction that it is facing, but slowly retracts back to player position + offset
                        auto* playerTransform = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                        auto* heldRigidBody = draggingObject->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
                        TransformComponent* playerHandTransform = playerHand->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

                        if (playerTransform && playerHandTransform && heldRigidBody) {
                            Vector2 mousePositionWorld = mousePosition;
                            Vector2 directionToMouse = mousePositionWorld - playerTransform->GetPosition();

                            float magnitude = std::sqrt(directionToMouse.x * directionToMouse.x + directionToMouse.y * directionToMouse.y);
                            if (magnitude > 0) {
                                directionToMouse.x /= magnitude;
                                directionToMouse.y /= magnitude;
                            }

                            Vector2 targetPosition = directionToMouse * 100.f; //Offset distance (100 units)
                            Vector2 currentHandPosition = playerHandTransform->GetLocalPosition();
                            Vector2 direction = targetPosition - currentHandPosition;

                            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                            //Check stopping condition
                            constexpr float epsilon = 0.1f; // Precision threshold
                            if (distance <= 5.0f ||
                                (std::abs(currentHandPosition.x - targetPosition.x) < epsilon &&
                                    std::abs(currentHandPosition.y - targetPosition.y) < epsilon)) {
                                playerHandTransform->SetLocalPosition(targetPosition);
                                grabbingBack = false;
                                heldObject = draggingObject;
                                AudioManager::GetInstance().PlayAudio(68);
                                AudioManager::GetInstance().SetChannelVolume(68, 0.3f);
                                //ImGuiConsole::Cout("Suction Time: %s: %f", draggingObject->GetName(), suctionTime);
                                draggingObject = nullptr;
                                collidedObject = nullptr;
#ifdef _LOGGING         
                                ImGuiConsole::Cout("Hand retracted to player.");

#endif // _LOGGING
                                heldObject->SetParent(GetParentGameObject());//Set this object as the player's child now
                                collider->SetCurrentlyColliding(nullptr);
                                AudioManager::GetInstance().StopAudio(2);
                                suctionSoundPlaying = false;
                                Engine::GetInstance().cameraManager.GetPlayerCamera().HandleShake(false, false);    //camera shake stops
                                suctionTime = 0.0f; // Reset suction acceleration when suction stops
                                prevDraggingObject = nullptr; // Ensure next suction starts fresh

                                if (enemySprite)
                                {
                                    enemySprite->SetRGB(Vector4(1.f, 1.f, 1.f, 1.f));
                                }

                                if (suctionVFX)
                                {
                                    GameObjectFactory::GetInstance().QueueDespawn(suctionVFX);
                                    particleSystemInitialized = false;
                                }

                                if (!trailingVFX.empty())
                                {
                                    for (GameObject* obj : trailingVFX)
                                        GameObjectFactory::GetInstance().QueueDespawn(obj);
                                    trailingVFX.clear();
                                }

                                return;
                            }


                            if (distance > 0) {
                                direction.x /= distance;
                                direction.y /= distance;
                            }

                            if (draggingObject != prevDraggingObject)
                            {
                                suctionTime = 0.0f;
                            }
                            // Gradual acceleration over time for suction effect
                            suctionTime += (float)InputManager::GetDeltaTime(); // Increases while suction is active

                            float suctionAcceleration = std::min(suctionTime * 3.0f, 1.0f); // Starts slow, speeds up

                            float massFactor = heldRigidBody->GetMass();
                            float baseLerpSpeed = 3.0f; // Base suction speed
                            float dynamicLerpSpeed = baseLerpSpeed + (80.0f / (massFactor + 5.0f)) * (1.0f - (distance / 500.0f));

                            // Multiply by acceleration factor to make suction start slow and speed up
                            dynamicLerpSpeed *= suctionAcceleration;

                            float maxSpeed = std::max(12.0f - massFactor * 0.3f, 1.5f); // Reduce max speed for heavier enemies
                            dynamicLerpSpeed = std::clamp(dynamicLerpSpeed, 0.5f, maxSpeed);

                            Vector2 newHandPosition = currentHandPosition + direction * dynamicLerpSpeed;

                            handTransform->SetLocalPosition(newHandPosition);

                            auto* dragHeldTransform = draggingObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                            if (dragHeldTransform) {
                                dragHeldTransform->SetLocalPosition(handTransform->GetPosition());
                            }

                            Vector2 playerPos = GetParentGameObject()->GetComponent<TransformComponent>(TRANSFORM)->GetPosition();
                            
                            if (trailingVFX.empty())
                            {
                                float dotsTrailingGap = handOffset / 8.f; // 7 dots max spawned from max grab
                                for (int i = 1; i <= 7; ++i)
                                {
                                    if (dotsTrailingGap * i < distance)
                                    {
                                        GameObject* dot = GameObjectFactory::GetInstance().CreateFromLua("Assets/Lua/Prefabs/TrailingDots.lua", "TrailingDots_0");
                                        TransformComponent* trans = dot->GetComponent<TransformComponent>(TRANSFORM);

                                        Vector2 newPos;
                                        newPos.x = playerPos.x - direction.x * i * dotsTrailingGap;
                                        newPos.y = playerPos.y - direction.y * i * dotsTrailingGap;

                                        trans->SetLocalPosition(newPos);
                                        trailingVFX.push_back(dot);
                                    }
                                    else
                                        break;
                                }
                            }
                            else
                            {
                                float dotsTrailingGap = handOffset / 8.f;
                                for (int idx = (int)trailingVFX.size() - 1; idx >= 0; --idx)
                                {
                                    int i = idx + 1;
                                    if (dotsTrailingGap * i < distance)
                                    {
                                        TransformComponent* trans = trailingVFX[idx]->GetComponent<TransformComponent>(TRANSFORM);
                                        Vector2 newPos;
                                        newPos.x = playerPos.x - direction.x * i * dotsTrailingGap;
                                        newPos.y = playerPos.y - direction.y * i * dotsTrailingGap;
                                        trans->SetLocalPosition(newPos);
                                    }
                                    else
                                    {
                                        GameObjectFactory::GetInstance().QueueDespawn(trailingVFX[idx]);
                                        trailingVFX.erase(trailingVFX.begin() + idx);
                                    }
                                }
                            }

                            prevDraggingObject = draggingObject;
                        }
                    }

                    Engine::GetInstance().cameraManager.GetPlayerCamera().HandleShake(true, false);    //camera shake
                }
                //Release the object when the left mouse button is released
                else if (!InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {

                    if (draggingObject) {
#ifdef _LOGGING
                        ImGuiConsole::Cout("Released object: %s\n", draggingObject->GetName().c_str());
#endif // _LOGGING
                        auto* enemySprite = draggingObject->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);

                        auto* heldCollider = draggingObject->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                        if (heldCollider) {

                            heldCollider->SetTrigger(false); //Restore collision
                        }

                        auto* heldTransform = draggingObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                        if (heldTransform) {
                            heldTransform->SetLocalPosition(heldTransform->GetPosition()); // Detach the object
                            Engine::GetInstance().cameraManager.GetPlayerCamera().HandleShake(false, false);    //camera shake stops
                            suctionTime = 0.0f; // Reset suction acceleration when suction stops
                            prevDraggingObject = nullptr; // Ensure next suction starts fresh
                        }

                        grabbingBack = false;
                        draggingObject = nullptr;
                        heldObject = nullptr;
                        collidedObject = nullptr;

                        collider->SetCurrentlyColliding(nullptr);
                        AudioManager::GetInstance().StopAudio(2);
                        suctionSoundPlaying = false;

                        if (collider->GetCurrentlyColliding())
                        {
#ifdef _LOGGING
                            ImGuiConsole::Cout("Failed to release currently colliding!");
#endif // _LOGGING
                        }

                        if (enemySprite)
                        {
#ifdef _LOGGING
#endif // _LOGGING          
                            enemySprite->SetRGB(Vector4(1.f, 1.f, 1.f,1.f));
                        }

                        if (suctionVFX)
                        {
                            GameObjectFactory::GetInstance().QueueDespawn(suctionVFX);
                            particleSystemInitialized = false;
                        }

                        if (!trailingVFX.empty())
                        {
                            for (GameObject* obj : trailingVFX)
                                GameObjectFactory::GetInstance().QueueDespawn(obj);
                            trailingVFX.clear();
                        }
                    }
                    //playerHand->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)->ChangeState(0);
                    playerHand->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR)->animationController->SetParameter("EnemyGrab", false);
                }
#pragma endregion
            }
        }
    }
    if (AudioManager::GetInstance().IsPlaying(8))
    {
        //ImGuiConsole::Cout("BGM is playing\n");

        float volume = 0.0f;
        bool isPlaying = false;
        float isAudible = false;

        if (AudioManager::GetInstance().bgmChannel) {
            AudioManager::GetInstance().bgmChannel->getVolume(&volume);
            AudioManager::GetInstance().bgmChannel->isPlaying(&isPlaying);
            AudioManager::GetInstance().bgmChannel->getAudibility(&isAudible);



            unsigned int position = 0;
            AudioManager::GetInstance().bgmChannel->getPosition(&position, FMOD_TIMEUNIT_MS);

            if (position == 0) {
                //ImGuiConsole::Cout("BGM is stuck at position 0! Advancing...");
                AudioManager::GetInstance().bgmChannel->setPosition(100, FMOD_TIMEUNIT_MS);
            }

            FMOD_MODE mode;
            AudioManager::GetInstance().bgmChannel->getMode(&mode);

            if (mode & FMOD_VIRTUAL_PLAYFROMSTART) {
                //ImGuiConsole::Cout("BGM is in virtual mode. Forcing playback...");
                AudioManager::GetInstance().bgmChannel->setMode(mode & ~FMOD_VIRTUAL_PLAYFROMSTART);
            }

            FMOD_OUTPUTTYPE outputType;
            AudioManager::GetInstance().GetSystem()->getOutput(&outputType);

            std::string outputDebug = "Current FMOD Output Mode: " + std::to_string(outputType);
            //ImGuiConsole::Cout(outputDebug.c_str());

            int channelsPlaying = 0;
            AudioManager::GetInstance().GetSystem()->getChannelsPlaying(&channelsPlaying, nullptr);

            std::string channelDebug = "FMOD Active Channels: " + std::to_string(channelsPlaying);
            //ImGuiConsole::Cout(channelDebug.c_str());

        }
    }

    playerLifetime += (float)InputManager::GetDeltaTime();
    constexpr float bgmCooldownAfterSpawn = 2.0f;

    if (!playBGM &&
        playerLifetime > bgmCooldownAfterSpawn &&
        Engine::GetInstance().time > Engine::GetInstance().maxTime * 0.5f)

    {
        AudioManager::GetInstance().PlayAudio(8);

        //Sound Effects
        AudioManager::GetInstance().PlayAudio(30);
        AudioManager::GetInstance().SetChannelVolume(30, 0.1f);
        AudioManager::GetInstance().PlayAudio(31);
        AudioManager::GetInstance().SetChannelVolume(31, 0.1f);

        playBGM = true;
    }


    if (!changeBGM && Engine::GetInstance().time <= Engine::GetInstance().maxTime * 0.5f)
    {
        AudioManager::GetInstance().PlayAudio(17);
        AudioManager::GetInstance().SetChannelVolume(17, 0.2f);

        changeBGM = true;
    }

    // Apply or remove low-pass filter based on player x-position
    if (transform && AudioManager::GetInstance().bgmChannel) {
        float playerX = transform->GetPosition().x;

        if (playerX > 650.0f) {
            AudioManager::GetInstance().ApplyLowPassFilterSmooth(AudioManager::GetInstance().bgmChannel, 1200.0f, 1.0f);
        }
        else {
            AudioManager::GetInstance().RemoveLowPassFilterSmooth(AudioManager::GetInstance().bgmChannel, 1.0f);
        }
    }

}


//void PlayerControllerComponent::ApplyKnockback(const Vector2& direction, float strength) {
//    inKnockback = true;
//    knockbackTimer = knockbackDuration;
//    knockbackVelocity = direction * strength; //Calculate knockback direction and strength
//}

/**
 * @brief Returns debug information for the component
 *
 * @return A string with debug information (currently returns an empty string)
 */
std::string PlayerControllerComponent::DebugInfo() const {
    return std::string();
}