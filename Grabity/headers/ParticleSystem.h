/*!****************************************************************
\file:      ParticleSystem.h
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232
\brief:     Header for a 2D particle system component with sprite animation support
\details:   Defines the Particle and ParticleSystem classes for managing
            particle effects with customizable properties, animations,
            and ImGui controls for runtime manipulation.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include <ObjectPool.h>
#include "Component.h"
#include "SpriteAnimation.h"
#include <random>
#include "pch.h"

/**
 * @struct ParticleData
 * @brief Data structure containing particle state information
 */
struct ParticleData {
    float x;
    float y;
    Vector2 currentSize;
    bool active;
    std::unique_ptr<SpriteAnimation> Animation;
};

/**
 * @class Particle
 * @brief Represents a single particle in the particle system
 */
class Particle {
public:
    Particle();
    void init(float x_, float y_, float vx_, float vy_, Vector2 size_, float lifetime_);
    void update(float deltatime);

    /**
     * @brief Render particle using OpenGL
     */
    void render() const;

    /** @brief Check if particle is active */
    bool isActive() const;

    /** @brief Get current particle state data */
    ParticleData GetParticleData() const;

    /** @brief Get associated sprite animation */
    SpriteAnimation* GetAnimation() { return Animation.get(); }

    /** @brief Set sprite animation */
    void SetAnimation(std::unique_ptr<SpriteAnimation> SpriteAnimation) { Animation = std::move(SpriteAnimation); }

private:
    float x, y;          ///< Position
    float vx, vy;        ///< Velocity
    Vector2 size;          ///< Initial size
    Vector2 currentSize;   ///< Current size
    float lifetime;      ///< Total lifetime
    float age;          ///< Current age
    bool active;        ///< Active status
    double accumulatedTime;
    std::unique_ptr<SpriteAnimation> Animation;  ///< Sprite animation
};

/**
 * @class ParticleSystem
 * @brief Component managing particle creation, updates, and rendering
 */
class ParticleSystem : public Component {
public:
    /**
     * @brief Constructor
     * @param parent Parent GameObject
     * @param maxParticles Maximum number of particles (default 10000)
     */
    explicit ParticleSystem(GameObject* parent, size_t maxParticles = 10000);
    ~ParticleSystem() = default;

    // Component interface implementations
    void Update() override;
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;
    std::string DebugInfo() const override;

    // Particle system specific methods
    void emitParticle();           ///< Emit single particle
    void emit(int count);          ///< Emit multiple particles
    void render();                 ///< Render all particles
    void setSource(float x, float y);  ///< Set emission source position
    void clear();                  ///< Remove all particles
    std::vector<ParticleData> GetParticlePoolData() const;  ///< Get all particle data

    // Getters and setters for ImGui controls
    SpriteAnimation* GetAnimation() { return Animation.get(); }
    void SetAnimation(std::unique_ptr<SpriteAnimation> spriteAnimation) { Animation = std::move(spriteAnimation); }

    float GetDuration() const { return duration; }
    void SetDuration(float value) { duration = value; }
    bool IsLooping() const { return looping; }
    void SetLooping(bool value) { looping = value; }
    float GetLifetime() const { return particleLifetime; }
    void SetLifetime(float value) { particleLifetime = value; }
    float GetSpeed() const { return speed; }
    void SetSpeed(float value) { speed = value; }

    Vector2 GetSize() const { return particleSize; }
    void SetSize(Vector2 value) { particleSize = value; }

    Vector3 GetColor() const { return color; }
    void SetColor(const Vector3& value) { color = value; }

    // Prevent copying
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;



private:
    ObjectPool<Particle> particlePool;   ///< Pool of particle objects
    float sourceX, sourceY;              ///< Emission source position
    Vector2 particleSize;                  ///< Base particle size
    float particleLifetime;              ///< Base particle lifetime
    float spread;                        ///< Emission spread angle
    std::unique_ptr<SpriteAnimation> Animation;  ///< Shared animation

    // ImGui control variables
    float duration = 5.0f;               ///< Animation duration
    float timer = 0.0f;
    bool looping = false;                ///< Animation looping flag
    bool loopOnce = true;
    float speed = 10.0f;                 ///< Particle speed
    Vector3 color = Vector3(1.0f, 1.0f, 1.0f);  ///< Particle color

    // Random number generation
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> randomFloat;
};