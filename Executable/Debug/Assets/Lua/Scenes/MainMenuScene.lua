-- Lua Level file
Sprite_MainMenuPlay_0 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "Assets/Lua/Scenes/CutScene.lua",
    },

    Transform = {
        positionX = -450.00,
        positionY = 0.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "MainMenuStartUpdatedNoHover",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_MainMenuPlay",
        parentID = 3,
        tag = "StartButton",
        layer = "Default",
    },
}

Sprite_MainMenuExit_1 = {
    ButtonComponent = {
        FunctionType = 1,
        PathNextScene = "",
    },

    Transform = {
        positionX = -450.00,
        positionY = -300.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "MainMenuExitUpdatedNoHover",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_MainMenuExit",
        parentID = 3,
        tag = "Untagged",
        layer = "UI",
    },
}

Sprite_Menu_2 = {
    Transform = {
        positionX = 0.00,
        positionY = 0.14,
        scaleX = 1600.00,
        scaleY = 900.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "MainMenuUpdated",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 0,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_Menu",
        parentID = 3,
        tag = "Untagged",
        layer = "Default",
    },
}

Canvas_3 = {
    Canvas = {
        isCanvas = true,
    },

    Transform = {
        positionX = 0.00,
        positionY = 0.00,
        scaleX = 1.00,
        scaleY = 1.00,
        rotation = 0.00,
    },

    Name = {
        name = "Canvas",
        parentID = -1,
        tag = "UI",
        layer = "UI",
    },
}

Sprite_MainMenuHowToPlay_4 = {
    ButtonComponent = {
        FunctionType = 9,
        PathNextScene = "",
    },

    Transform = {
        positionX = -450.00,
        positionY = -100.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "MainMenuHowToPlayNoHover",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 0,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_MainMenuHowToPlay",
        parentID = 3,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_GabityTitle_5 = {
    Transform = {
        positionX = -472.00,
        positionY = 256.00,
        scaleX = 1000.00,
        scaleY = 682.30,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "Animation_GrabityTitle",
        SpriteNxFrame = 2.00,
        SpriteNyFrame = 3.00,
        FramesPerSecond = 10.00,
        TotalFrame = 5.00,
        Spritelayer = 0,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_GabityTitle",
        parentID = 3,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_Options_6 = {
    ButtonComponent = {
        FunctionType = 7,
        PathNextScene = "",
    },

    Transform = {
        positionX = -450.00,
        positionY = -200.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "PauseHover",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 0,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_Options",
        parentID = 3,
        tag = "Untagged",
        layer = "Default",
    },
}

OptionsCanvas_7 = {
    Canvas = {
        isCanvas = true,
    },

    RigidBody = {
        mass = 1.00,
        velocityX = 0.00,
        velocityY = 0.00,
        drag = 0.00,
        accelerationX = 0.00,
        accelerationY = 0.00,
    },

    Transform = {
        positionX = 0.00,
        positionY = -1000.00,
        scaleX = 1.00,
        scaleY = 1.00,
        rotation = 0.00,
    },

    Name = {
        name = "OptionsCanvas",
        parentID = -1,
        tag = "Options",
        layer = "UI",
    },
}

Sprite_Option_PopUp_8 = {
    Transform = {
        positionX = 0.00,
        positionY = 0.00,
        scaleX = 1209.00,
        scaleY = 868.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "Volume_panel_bg",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_Option_PopUp",
        parentID = 7,
        tag = "Untagged",
        layer = "UI",
    },
}

CloseOptionsButton_9 = {
    ButtonComponent = {
        FunctionType = 6,
        PathNextScene = "",
    },

    Transform = {
        positionX = 400.00,
        positionY = 220.00,
        scaleX = 100.00,
        scaleY = 100.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "CloseButton",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "CloseOptionsButton",
        parentID = 7,
        tag = "Untagged",
        layer = "UI",
    },
}

SliderMaster_10 = {
    ButtonComponent = {
        FunctionType = 2,
        PathNextScene = "",
    },

    SliderComponent = {
        maxPosXOffset = 450.00,
        currentValue = 1.00,
        volumeType = "MASTER",
    },

    Collider = {
        ColliderCount = 1,
        isTrigger = false,
        CollisionSizeX_0 = 40.00,
        CollisionSizeY_0 = 40.00,
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

    Transform = {
        positionX = 400.00,
        positionY = 115.00,
        scaleX = 200.00,
        scaleY = 200.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "Volume_panel_Dot",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "SliderMaster",
        parentID = 7,
        tag = "Untagged",
        layer = "UI",
    },
}

HowToPlayCanvas_11 = {
    Canvas = {
        isCanvas = true,
    },

    RigidBody = {
        mass = 1.00,
        velocityX = 0.00,
        velocityY = 0.00,
        drag = 0.00,
        accelerationX = 0.00,
        accelerationY = 0.00,
    },

    Transform = {
        positionX = 0.00,
        positionY = -1000.00,
        scaleX = 1.00,
        scaleY = 1.00,
        rotation = 0.00,
    },

    Name = {
        name = "HowToPlayCanvas",
        parentID = -1,
        tag = "HowToPlay",
        layer = "UI",
    },
}

Sprite_HowToPlay_PopUp_12 = {
    Transform = {
        positionX = 0.00,
        positionY = 0.00,
        scaleX = 1209.00,
        scaleY = 868.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "HowToPlay1",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 0.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_HowToPlay_PopUp",
        parentID = 11,
        tag = "ChangeHowToPlay",
        layer = "UI",
    },
}

CloseHTPlayButton_13 = {
    ButtonComponent = {
        FunctionType = 6,
        PathNextScene = "",
    },

    Transform = {
        positionX = 450.00,
        positionY = 290.00,
        scaleX = 100.00,
        scaleY = 100.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "CloseButton",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "CloseHTPlayButton",
        parentID = 11,
        tag = "Untagged",
        layer = "UI",
    },
}

SliderSFX_14 = {
    ButtonComponent = {
        FunctionType = 2,
        PathNextScene = "",
    },

    SliderComponent = {
        maxPosXOffset = 450.00,
        currentValue = 0.00,
        volumeType = "SFX",
    },

    Collider = {
        ColliderCount = 1,
        isTrigger = false,
        CollisionSizeX_0 = 40.00,
        CollisionSizeY_0 = 40.00,
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

    Transform = {
        positionX = 400.00,
        positionY = 20.00,
        scaleX = 200.00,
        scaleY = 200.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "Volume_panel_Dot",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "SliderSFX",
        parentID = 7,
        tag = "Untagged",
        layer = "UI",
    },
}

SliderBGM_15 = {
    ButtonComponent = {
        FunctionType = 2,
        PathNextScene = "",
    },

    SliderComponent = {
        maxPosXOffset = 450.00,
        currentValue = 1.00,
        volumeType = "BGM",
    },

    Collider = {
        ColliderCount = 1,
        isTrigger = false,
        CollisionSizeX_0 = 40.00,
        CollisionSizeY_0 = 40.00,
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

    Transform = {
        positionX = 400.00,
        positionY = -80.00,
        scaleX = 200.00,
        scaleY = 200.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "Volume_panel_Dot",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "SliderBGM",
        parentID = 7,
        tag = "Untagged",
        layer = "UI",
    },
}

NextButton_16 = {
    ButtonComponent = {
        FunctionType = 11,
        PathNextScene = "",
    },

    Transform = {
        positionX = 450.00,
        positionY = 0.00,
        scaleX = 100.00,
        scaleY = 100.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "NextButton",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 0.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "NextButton",
        parentID = 11,
        tag = "ChangeButton2",
        layer = "UI",
    },
}

BackButton_17 = {
    ButtonComponent = {
        FunctionType = 12,
        PathNextScene = "",
    },

    Transform = {
        positionX = -480.00,
        positionY = 0.00,
        scaleX = 100.00,
        scaleY = 100.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "NextButton",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 0.00,
        TotalFrame = 1.00,
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = true,
        Renderable = false,
    },

    Name = {
        name = "BackButton",
        parentID = 11,
        tag = "ChangeButton1",
        layer = "UI",
    },
}

Sprite_MainMeuHowToPlayHover_18 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "",
    },

    Transform = {
        positionX = -450.00,
        positionY = -100.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "HowToPlayHovering",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 0.00,
        TotalFrame = 1.00,
        Spritelayer = 0,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "Sprite_MainMeuHowToPlayHover",
        parentID = 3,
        tag = "Hovering",
        layer = "Default",
    },
}

Sprite_MainMenuPlayHover_19 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "",
    },

    Transform = {
        positionX = -450.00,
        positionY = 0.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "StartHovering",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 0.00,
        TotalFrame = 1.00,
        Spritelayer = 2,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "Sprite_MainMenuPlayHover",
        parentID = 3,
        tag = "Hovering",
        layer = "Default",
    },
}

Sprite_MainMenuVolumeHover_20 = {
    ButtonComponent = {
        FunctionType = 7,
        PathNextScene = "",
    },

    Transform = {
        positionX = -450.00,
        positionY = -200.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "VolumeHovering",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 0,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_MainMenuVolumeHover",
        parentID = 3,
        tag = "Hovering",
        layer = "Default",
    },
}

Sprite_MainMenuExitHover_21 = {
    ButtonComponent = {
        FunctionType = 1,
        PathNextScene = "",
    },

    Transform = {
        positionX = -450.00,
        positionY = -300.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "ExitHovering",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 2,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "Sprite_MainMenuExitHover",
        parentID = 3,
        tag = "Hovering",
        layer = "Default",
    },
}

FadeObject_22 = {
    Video = {
        elapsedTime = 0.00,
        currentScene = 1,
        switchInterval = 2.00,
        totalScenes = 16,
    },

    Transform = {
        positionX = 0.00,
        positionY = 1600.00,
        scaleX = 1600.00,
        scaleY = 900.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "CutSceneBG",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 99,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "FadeObject",
        parentID = 3,
        tag = "FadeAble",
        layer = "Default",
    },
}

