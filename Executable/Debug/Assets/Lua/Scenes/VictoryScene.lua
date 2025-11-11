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

MainMenu_1 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "Assets/Lua/Scenes/MainMenuScene.lua",
    },

    Transform = {
        positionX = 541.00,
        positionY = -266.00,
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
        Spritelayer = 10,
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

Background_2 = {
    Transform = {
        positionX = 0.00,
        positionY = 0.00,
        scaleX = 1920.00,
        scaleY = 1080.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "VictoryBG",
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

MainMenu_Hover_3 = {
    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "",
    },

    Transform = {
        positionX = 541.00,
        positionY = -266.00,
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
        Spritelayer = 10,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "MainMenu_Hover",
        parentID = 0,
        tag = "Hovering",
        layer = "UI",
    },
}

