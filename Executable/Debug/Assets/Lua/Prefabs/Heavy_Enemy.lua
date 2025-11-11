-- Lua Level file
Heavy_Enemy_0 = {
    Animator = {
        ControllerPathName = "Assets/Lua/AnimationController/HeavyEnemy.lua",
    },
    
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "",
    },

    Collider = {
        ColliderCount = 1,
        isTrigger = false,
        CollisionSizeX_0 = 160.00,
        CollisionSizeY_0 = 64.00,
        CollisionCenterX_0 = 0.00,
        CollisionCenterY_0 = -70.00,
    },

    Health = {
        health = 50,
        maxHealth = 50,
    },

    RigidBody = {
        mass = 40.00,
        velocityX = 0.00,
        velocityY = 0.00,
        drag = 3.00,
        accelerationX = 0.00,
        accelerationY = 0.00,
    },

    Sprite = {
        SpriteAnimationFrame_0 = 6.00,
        SpriteAnimationFrameX_0 = 2.00,
        SpriteAnimationFrameY_0 = 3.00,
        SpritePathName_0 = "Animation_Heavy_Enemy",
        Spritelayer = 80,
        SpriteFlipX = false,
        SpriteFlipY = false,
        SpriteCurrentID = 0,
        SpriteMaxID = 3,
    },

    Transform = {
        positionX = 19.09,
        positionY = -85.36,
        scaleX = 300.00,
        scaleY = 300.00,
        rotation = 0.00,
    },

    AIState = {
        currentStateName = "CHASE",
        waitTime = 0.10,
        moveSpeed = 50.00,
        target = 43,
    },

    Name = {
        name = "Heavy_Enemy",
        parentID = -1,
        tag = "HeavyEnemy",
        layer = "Default",
    },
}

