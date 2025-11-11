/*!****************************************************************
\file:      ParticleSystem.cpp
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232
\brief:     Implementation of a 2D particle system with OpenGL rendering
\details:   Provides a component-based particle system that manages particle
            emission, lifecycle, and rendering. Supports dynamic particle
            creation, updating, and removal with customizable properties.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "ParticleSystem.h"
#include "glhelper.h"
#include "assetmanager.h"
#include "GameObject.h"

/*------------------------------------------------------------------------------
// Particle Class Implementation
------------------------------------------------------------------------------*/

/**
 * @brief Default constructor for Particle class
 * @details Initializes particle as inactive
 */
Particle::Particle() : active(false) {}


/**
 * @brief Initialize a particle with specific properties
 * @param x_ - Initial x position
 * @param y_ - Initial y position
 * @param vx_ - Initial x velocity
 * @param vy_ - Initial y velocity
 * @param size_ - Initial size
 * @param lifetime_ - Total lifetime in seconds
 */
void Particle::init(float x_, float y_, float vx_, float vy_,
    Vector2 size_, float lifetime_) {
    x = x_;
    y = y_;
    vx = vx_;
    vy = vy_;
    size = size_;
    currentSize = size_;
    lifetime = lifetime_;
    age = 0.0f;
    active = true;
}


/**
 * @brief Updates particle position, size, and lifetime
 * @param deltaTime - Time elapsed since last update
 * @details Updates particle age and deactivates if lifetime exceeded.
 *          Updates position based on velocity and shrinks size over time.
 */
void Particle::update(float deltaTime) {
    if (!active) return;

    age += deltaTime;
    if (age >= lifetime) {
        active = false;
        return;
    }

    x += vx * deltaTime;
    y += vy * deltaTime;

    // Update size (shrink over time)
    //float lifeRatio = age / lifetime;
    //currentSize = size * (1.0f - lifeRatio);
    accumulatedTime += Engine::GetInstance().fixedDT;
    Animation->UpdateSprite(accumulatedTime);
}


/**
 * @brief Renders the particle using OpenGL
 * @details Renders particle as a quad with fade-out based on lifetime.
 *          Uses alpha blending for smooth transitions.
 */
void Particle::render() const {
    if (!active) return;

    float alpha = 1.0f - (age / lifetime);
    glColor4f(0.5f, 0.5f, 1.0f, alpha);

    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    Vector2 halfSize = currentSize * 0.5f;
    glBegin(GL_QUADS);
    glVertex2f(-halfSize.x, -halfSize.y);
    glVertex2f(halfSize.x, -halfSize.y);
    glVertex2f(halfSize.x, halfSize.y);
    glVertex2f(-halfSize.x, halfSize.y);
    glEnd();

    glPopMatrix();
}


/**
 * @brief Check if particle is currently active
 * @return bool - Active status of particle
 */
bool Particle::isActive() const {
    return active;
}


/**
 * @brief Get current particle properties
 * @return ParticleData - Struct containing current particle state
 */
ParticleData Particle::GetParticleData() const {
    return ParticleData{
        x,
        y,
        currentSize,
        active,
        std::move(Animation->Clone())
    };
}

/*------------------------------------------------------------------------------
// ParticleSystem Class Implementation
------------------------------------------------------------------------------*/

/**
 * @brief Constructor for ParticleSystem component
 * @param parent - Parent GameObject this system is attached to
 * @param maxParticles - Maximum number of particles in the pool
 * @details Initializes particle pool and random number generator
 */
ParticleSystem::ParticleSystem(GameObject* parent, size_t maxParticles)
    : Component(parent),
    particlePool(maxParticles),
    sourceX(0.0f),
    sourceY(0.0f),
    particleSize({ 0.5f,0.5f }),
    particleLifetime(2.0f),
    spread(30.0f),
    gen(rd()),
    randomFloat(-1.0f, 1.0f) {
}


/**
 * @brief Updates all particles and handles emission
 * @details Updates particle positions, removes inactive particles,
 *          and emits new particles when mouse button is pressed
 */
void ParticleSystem::Update() {
    //float deltaTime = 1.0f / 60.0f; // You might want to get actual deltaTime from your engine

    // Update all active particles
    for (auto& particle : particlePool.GetActiveParticles()) {
        particle->update((float)Engine::GetInstance().fixedDT);
        // Remove inactive particles
        if (!particle->isActive()) {
            particlePool.Remove(particle);
        }
    }

    timer -= (float)Engine::GetInstance().fixedDT;
    TransformComponent* trans = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    if (trans->GetPosition().x == 0 && trans->GetPosition().y == 0)
        return;

    // Update particle source to object position
    setSource(trans->GetPosition().x, trans->GetPosition().y);

    // Emit particles when left mouse button is held
    //if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    //    emit(5);
    //}
    //timer -= (float)Engine::GetInstance().fixedDeltaTimeSeconds;
    if (timer <= 0.f)
    {
        if (looping)
            emit(1);
        else if (loopOnce)
        {
            emit(1);
            loopOnce = false;
        }
        timer = duration;
    }
}

void ParticleSystem::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    // Implement serialization logic here
    // You might want to save:
    // - particleSize
    // - particleLifetime
    // - spread
    // - source position

    // Serialization testing, save into lua
     // Serialization testing, save into lua
    LuaManager luaManager(luaFilePath);


    //int totalID = maxID;
    std::vector<std::string> keys;
    LuaManager::LuaValueContainer values;

    // Serialize each collider's size and center
    keys.push_back("SpritePathName_0");
    values.push_back(Animation->GetSpriteTexture()->codeName.c_str());

    keys.push_back("particleSizeX");
    values.push_back(particleSize.x);

    keys.push_back("particleSizeY");
    values.push_back(particleSize.y);

    keys.push_back("particleLifetime");
    values.push_back(particleLifetime);

    keys.push_back("spread");
    values.push_back(spread);

    keys.push_back("loop");
    values.push_back(looping);

    keys.push_back("duration");
    values.push_back(duration);

    keys.push_back("speed");
    values.push_back(speed);

    // Call the LuaWrite function with the keys and values
    luaManager.LuaWrite(tableName, values, keys, "ParticleSystem");
}

void ParticleSystem::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    // Implement deserialization logic here
    // Load saved values for:
    // - particleSize
    // - particleLifetime
    // - spread
    // - source position
     //Load from lua
    LuaManager luaManager(luaFilePath);

    Animation = std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite(luaManager.LuaRead<std::string>(tableName, { "ParticleSystem", "SpritePathName_0" })));

    particleSize.x = luaManager.LuaRead<float>(tableName, { "ParticleSystem", "particleSizeX" });
    particleSize.y = luaManager.LuaRead<float>(tableName, { "ParticleSystem", "particleSizeY" });
    particleLifetime = luaManager.LuaRead<float>(tableName, { "ParticleSystem", "particleLifetime" });
    spread = luaManager.LuaRead<float>(tableName, { "ParticleSystem", "spread" });
    looping = luaManager.LuaRead<bool>(tableName, { "ParticleSystem", "loop" });
    duration = luaManager.LuaRead<float>(tableName, { "ParticleSystem", "duration" });
    speed = luaManager.LuaRead<float>(tableName, { "ParticleSystem", "speed" });
}

// Rest of the ParticleSystem implementation remains the same

/**
 * @brief Emits a single particle with randomized properties
 * @details Creates new particle with random velocity and lifetime variations
 */
void ParticleSystem::emitParticle() {
    Particle* p = particlePool.Create();
    if (p) {
        float vx = randomFloat(gen) * spread;
        float vy = randomFloat(gen) * spread;
        float lifetime = particleLifetime * (0.8f + randomFloat(gen) * 0.4f);

        p->init(sourceX, sourceY, vx, vy, particleSize, lifetime);
        p->SetAnimation(std::move(Animation->Clone()));
    }
}


/**
 * @brief Emits multiple particles
 * @param count - Number of particles to emit
 */
void ParticleSystem::emit(int count) {
    for (int i = 0; i < count; ++i) {
        emitParticle();
    }
}


/**
 * @brief Renders all active particles
 * @details Handles OpenGL state for proper particle blending
 */
void ParticleSystem::render() {
    // Save the current blend state
    GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);
    GLint previousBlendSrc, previousBlendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &previousBlendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &previousBlendDst);

    // Set up particle blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render particles
    for (const auto& particle : particlePool.GetActiveParticles()) {
        particle->render();
    }

    // Restore previous blend state
    if (!blendWasEnabled) {
        glDisable(GL_BLEND);
    }
    glBlendFunc(previousBlendSrc, previousBlendDst);
}


/**
 * @brief Sets the emission source position
 * @param x - X coordinate
 * @param y - Y coordinate
 */
void ParticleSystem::setSource(float x, float y) {
    sourceX = x;
    sourceY = y;
}


/**
 * @brief Removes all active particles
 */
void ParticleSystem::clear() {
    particlePool.Clear();
}


/**
 * @brief Gets data for all active particles
 * @return vector<ParticleData> - List of current particle states
 */
std::vector<ParticleData> ParticleSystem::GetParticlePoolData() const {
    std::vector<ParticleData> particleDataList;
    const auto& activeParticles = particlePool.GetActiveParticles();
    particleDataList.reserve(activeParticles.size());

    for (const auto& particle : activeParticles) {
        particleDataList.push_back(particle->GetParticleData());
    }

    return particleDataList;
}

/**
 * @brief Gets data for all active particles
 * @return vector<ParticleData> - List of current particle states
 */
std::string ParticleSystem::DebugInfo() const {
    return "ParticleSystem - Active Particles: " +
        std::to_string(particlePool.GetActiveParticles().size());
}