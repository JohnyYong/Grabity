-- Lua Level file
Light_Enemy_0 = {
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
        CollisionSizeX_0 = 50.00,
        CollisionSizeY_0 = 43.00,
        CollisionCenterX_0 = -2.00,
        CollisionCenterY_0 = -32.00,
    },

    Health = {
        health = 20,
        maxHealth = 20,
    },

    RigidBody = {
        mass = 10.00,
        velocityX = -20.22,
        velocityY = 36.79,
        drag = 2.00,
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
        positionX = -445.27,
        positionY = 84.93,
        scaleX = 120.00,
        scaleY = 120.00,
        rotation = 0.00,
    },

    AIState = {
        currentStateName = "CHASE",
        waitTime = 0.10,
        moveSpeed = 100.00,
        target = 25,
    },

    Name = {
        name = "Light_Enemy",
        parentID = -1,
        tag = "Enemy",
        layer = "Default",
    },

    Splitting = {
        numSplits = 2,
        healthMultiplier = 0.1,
        speedMultiplier = 1.5,
        sizeMultiplier = 0.65,
        hasSplit = false,
        splitPrefabPath = "Assets/Lua/Prefabs/Light_Enemy.lua",
        splitPrefabName = "Light_Enemy_0",
    },


}

