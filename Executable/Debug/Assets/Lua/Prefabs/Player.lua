-- Lua Level file
Player_0 = {
    PlayerController = {
        moveSpeed = 400.00,
    },

    Audio = {
        audioClip0 = 11,
        audioClip1 = 12,
    },

    Collider = {
        ColliderCount = 1,
        isTrigger = false,
        CollisionSizeX_0 = 54.00,
        CollisionSizeY_0 = 57.00,
        CollisionCenterX_0 = 0.00,
        CollisionCenterY_0 = -39.00,
    },

    Health = {
        health = 50,
        maxHealth = 50,
    },

    RigidBody = {
        mass = 1.00,
        velocityX = 0.00,
        velocityY = 0.00,
        drag = 0.00,
        accelerationX = 0.00,
        accelerationY = 0.00,
    },

    Spawner = {
        minInterval = 5.30,
        maxInterval = 14.20,
        nextSpawnTime = 0.00,
    },

    Sprite = {
        SpriteAnimationFrame_0 = 1.00,
        SpriteAnimationFrameX_0 = 1.00,
        SpriteAnimationFrameY_0 = 1.00,
        SpritePathName_0 = "PlayerIdle",
        SpriteAnimationFrame_1 = 8.00,
        SpriteAnimationFrameX_1 = 2.00,
        SpriteAnimationFrameY_1 = 4.00,
        SpritePathName_1 = "Animation_PlayerWalk",
        Spritelayer = 78,
        SpriteFlipX = false,
        SpriteFlipY = false,
        SpriteCurrentID = 0,
        SpriteMaxID = 2,
    },

    Transform = {
        positionX = -1023.61,
        positionY = -2.67,
        scaleX = 150.00,
        scaleY = 150.00,
        rotation = 0.00,
    },

    Name = {
        name = "Player",
        parentID = -1,
        tag = "Player",
        layer = "Default",
    },
}

PlayerHand_1 = {
    Collider = {
        ColliderCount = 1,
        isTrigger = false,
        CollisionSizeX_0 = 54.00,
        CollisionSizeY_0 = 38.00,
        CollisionCenterX_0 = 0.00,
        CollisionCenterY_0 = 0.00,
    },

    RigidBody = {
        mass = 1.00,
        velocityX = 0.00,
        velocityY = 0.00,
        drag = 0.00,
        accelerationX = 0.00,
        accelerationY = 0.00,
    },

    Sprite = {
        SpriteAnimationFrame_0 = 1.00,
        SpriteAnimationFrameX_0 = 1.00,
        SpriteAnimationFrameY_0 = 1.00,
        SpritePathName_0 = "HandOpen",
        Spritelayer = 99,
        SpriteFlipX = false,
        SpriteFlipY = false,
        SpriteCurrentID = 0,
        SpriteMaxID = 1,
    },

    Transform = {
        positionX = 59.27,
        positionY = -9.63,
        scaleX = 0.50,
        scaleY = 0.50,
        rotation = -90.00,
    },

    Name = {
        name = "PlayerHand",
        parentID = 0,
        tag = "Untagged",
        layer = "Default",
    },
}

