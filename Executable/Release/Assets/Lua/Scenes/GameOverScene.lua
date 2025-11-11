-- Lua Level file
Canvas_0 = {
    Canvas = {
        isCanvas = true,
    },

    Transform = {
        positionX = -40.00,
        positionY = -3.00,
        scaleX = 1.00,
        scaleY = 1.00,
        rotation = 0.00,
    },

    Name = {
        name = "Canvas",
        parentID = -1,
        tag = "Untagged",
        layer = "UI",
    },
}

Game_1 = {
    Transform = {
        positionX = 0.00,
        positionY = 0.00,
        scaleX = 100.00,
        scaleY = 100.00,
        rotation = 0.00,
    },

    Name = {
        name = "Game",
        parentID = 0,
        tag = "Untagged",
        layer = "Default",
    },
}

Game_2 = {
    Transform = {
        positionX = 0.00,
        positionY = 0.00,
        scaleX = 100.00,
        scaleY = 100.00,
        rotation = 0.00,
    },

    Name = {
        name = "Game",
        parentID = 0,
        tag = "Untagged",
        layer = "Default",
    },
}

TryAgain_3 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "Assets/Lua/Scenes/GameScene.lua",
    },

    Transform = {
        positionX = 500.00,
        positionY = -280.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "TryAgain",
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
        name = "TryAgain",
        parentID = 0,
        tag = "Untagged",
        layer = "UI",
    },
}

Background_4 = {
    Transform = {
        positionX = 0.00,
        positionY = 0.00,
        scaleX = 1920.00,
        scaleY = 1080.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "GameOverBG",
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
        name = "Background",
        parentID = 0,
        tag = "Untagged",
        layer = "Default",
    },
}

Game_5 = {
    Transform = {
        positionX = 0.00,
        positionY = 0.00,
        scaleX = 100.00,
        scaleY = 100.00,
        rotation = 0.00,
    },

    Name = {
        name = "Game",
        parentID = 0,
        tag = "Untagged",
        layer = "Default",
    },
}

MainMenu_6 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "Assets/Lua/Scenes/MainMenuScene.lua",
    },

    Transform = {
        positionX = 500.00,
        positionY = -400.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "MainMenuText",
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
        name = "MainMenu",
        parentID = 0,
        tag = "Untagged",
        layer = "Default",
    },
}

TryAgain_Hover_7 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "",
    },

    Transform = {
        positionX = 500.00,
        positionY = -280.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "TryAgainHovering",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 0,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "TryAgain_Hover",
        parentID = 0,
        tag = "Hovering",
        layer = "UI",
    },
}

MainMenu_Hover_8 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "",
    },

    Transform = {
        positionX = 500.00,
        positionY = -400.00,
        scaleX = 640.00,
        scaleY = 80.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "MainMenuHovering",
        SpriteNxFrame = 1.00,
        SpriteNyFrame = 1.00,
        FramesPerSecond = 1.00,
        TotalFrame = 1.00,
        Spritelayer = 0,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "MainMenu_Hover",
        parentID = 0,
        tag = "Hovering",
        layer = "Default",
    },
}

