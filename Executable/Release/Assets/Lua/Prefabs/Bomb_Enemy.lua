-- Lua Level file
Bomb_Enemy_0 = {
    Animator = {
        ControllerPathName = "Assets/Lua/AnimationController/BombEnemyTmp.lua",
    },

    Collider = {
        ColliderCount = 1,
        isTrigger = false,
        CollisionSizeX_0 = 153.00,
        CollisionSizeY_0 = 80.00,
        CollisionCenterX_0 = -39.00,
        CollisionCenterY_0 = -82.00,
    },

    Health = {
        health = 50,
        maxHealth = 50,
    },

    Explosion = {
        countdownTime = 2.00,
        damageRadius = 300.00,
        damageAmount = 20.00,
        hasExploded = false,
    },

    RigidBody = {
        mass = 30.00,
        velocityX = -0.00,
        velocityY = 0.00,
        drag = 2.00,
        accelerationX = 0.00,
        accelerationY = 0.00,
    },

    Sprite = {
        SpriteAnimationFrame_0 = 9.00,
        SpriteAnimationTotalFrame_0 = 3.00,
        SpriteAnimationFrameX_0 = 3.00,
        SpriteAnimationFrameY_0 = 1.00,
        SpritePathName_0 = "Animation_Bomb_Enemy",
        Spritelayer = 76,
        SpriteFlipX = false,
        SpriteFlipY = false,
    },

    Transform = {
        positionX = -0.00,
        positionY = 0.00,
        scaleX = 250.00,
        scaleY = 250.00,
        rotation = 0.00,
    },

    AIState = {
        currentStateName = "CHASE",
        waitTime = 0.10,
        moveSpeed = 100.00,
        target = -1,
    },

    Name = {
        name = "Bomb_Enemy",
        parentID = -1,
        tag = "BombEnemy",
        layer = "Default",
    },
}

