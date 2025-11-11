-- Lua Level file
BabyEnemy_0 = {
    Animator = {
        ControllerPathName = "Assets/Lua/AnimationController/LightEnemy.lua",
    },

    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "",
    },

    Collider = {
        ColliderCount = 1,
        isTrigger = false,
        CollisionSizeX_0 = 30.00,  -- Smaller collision size
        CollisionSizeY_0 = 25.00,  -- Smaller collision size
        CollisionCenterX_0 = -2.00,
        CollisionCenterY_0 = -20.00,  -- Adjusted for smaller size
    },

    Health = {
        health = 5,  -- Lower health for baby enemy
        maxHealth = 5,  -- Lower max health
    },

    RigidBody = {
        mass = 5.00,  -- Lower mass for smaller enemy
        velocityX = -20.22,
        velocityY = 36.79,
        drag = 1.50,  -- Slightly lower drag for faster movement
        accelerationX = 0.00,
        accelerationY = 0.00,
    },

    Sprite = {
        SpriteAnimationFrame_0 = 5.00,
        SpriteAnimationFrameX_0 = 4.00,
        SpriteAnimationFrameY_0 = 1.00,
        SpritePathName_0 = "Animation_Light_Enemy",  
        Spritelayer = 75,
        SpriteFlipX = false,
        SpriteFlipY = false,
        SpriteCurrentID = 0,
        SpriteMaxID = 3,
    },

    Transform = {
        positionX = 0.00,  -- Will be overridden by the SplittingComponent
        positionY = 0.00,  -- Will be overridden by the SplittingComponent
        scaleX = 72.00,    -- Default scale (60% of original 120.00)
        scaleY = 72.00,    -- Default scale (60% of original 120.00)
        rotation = 0.00,
    },

    AIState = {
        currentStateName = "CHASE",
        waitTime = 0.05,  -- More responsive
        moveSpeed = 150.00,  -- Faster than the light enemy
        target = 25,
    },

    Name = {
        name = "BabyEnemy",
        parentID = -1,
        tag = "Enemy",
        layer = "Default",
    },
}