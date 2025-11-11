-- Lua Level file
InGameUI_0 = {
    Canvas = {
        isCanvas = true,
    },

    Transform = {
        positionX = -1341.94,
        positionY = -742.98,
        scaleX = 100.00,
        scaleY = 100.00,
        rotation = 0.00,
    },

    Name = {
        name = "InGameUI",
        parentID = -1,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_PlayerPose_1 = {
    PauseMenuButton = {
        IsScript = true,
    },

    ButtonComponent = {
        FunctionType = 2,
        PathNextScene = "",
    },

    Transform = {
        positionX = 1005.22,
        positionY = 698.95,
        scaleX = 9.00,
        scaleY = 12.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "PlayerPose",
        Spritelayer = 75,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "Sprite_PlayerPose",
        parentID = 0,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_retry_text_2 = {
    PauseMenuButton = {
        IsScript = true,
    },

    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "Assets/Lua/Scenes/Scene1.lua",
    },

    Transform = {
        positionX = 1819.94,
        positionY = 1000.39,
        scaleX = 4.00,
        scaleY = 2.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "retry_text",
        Spritelayer = 100,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "Sprite_retry_text",
        parentID = 1,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_main_menu_text_3 = {
    PauseMenuButton = {
        IsScript = true,
    },

    ButtonComponent = {
        FunctionType = 0,
        PathNextScene = "Assets/Lua/Scenes/MainMenuScene.lua",
    },

    Transform = {
        positionX = 1704.94,
        positionY = 737.56,
        scaleX = 7.00,
        scaleY = 2.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "main_menu_text",
        Spritelayer = 100,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "Sprite_main_menu_text",
        parentID = 2,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_quit_text_4 = {
    PauseMenuButton = {
        IsScript = true,
    },

    ButtonComponent = {
        FunctionType = 1,
        PathNextScene = "Assets/Lua/Scenes/MainMenuScene.lua",
    },

    Transform = {
        positionX = 1819.94,
        positionY = 472.75,
        scaleX = 4.00,
        scaleY = 2.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "quit_text",
        Spritelayer = 100,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = false,
    },

    Name = {
        name = "Sprite_quit_text",
        parentID = 3,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_hpBar_fill_green_5 = {
    UIComponent = {
        Type = 1,
    },

    Transform = {
        positionX = 904.33,
        positionY = 1107.80,
        scaleX = 2.70,
        scaleY = 0.50,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "hpBar_fill_green",
        Spritelayer = 99,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_hpBar_fill_green",
        parentID = 4,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_hpBar_fill_red_6 = {
    UIComponent = {
        Type = 0,
    },

    Transform = {
        positionX = 904.33,
        positionY = 1107.80,
        scaleX = 2.70,
        scaleY = 0.50,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "hpBar_fill_red",
        Spritelayer = 1,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_hpBar_fill_red",
        parentID = 5,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_hpUI_outline_7 = {
    UIComponent = {
        Type = 0,
    },

    Transform = {
        positionX = 845.00,
        positionY = 1050.00,
        scaleX = 4.00,
        scaleY = 2.00,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "hpUI_outline",
        Spritelayer = 100,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_hpUI_outline",
        parentID = 6,
        tag = "Untagged",
        layer = "Default",
    },
}

Sprite_statusIcon_fill_8 = {
    UIComponent = {
        Type = 2,
    },

    Transform = {
        positionX = 710.44,
        positionY = 1053.67,
        scaleX = 1.20,
        scaleY = 1.70,
        rotation = 0.00,
    },

    SpriteUI = {
        SpritePathName = "statusIcon_fill",
        Spritelayer = 96,
        SpriteFlipX = false,
        SpriteFlipY = false,
        Renderable = true,
    },

    Name = {
        name = "Sprite_statusIcon_fill",
        parentID = 7,
        tag = "Untagged",
        layer = "Default",
    },
}

TimerUI_9 = {
    UIComponent = {
        Type = 3,
    },

    Text = {
        textContent = "SMAPLE:1234d",
        font = "",
        positionX = 375.00,
        positionY = 370.00,
        alpha = 0.00,
        RGB_R = 0.00,
        RGB_G = 0.00,
        RGB_B = 0.00,
        fontSize = 1.00,
    },

    Transform = {
        positionX = 375.00,
        positionY = 370.00,
        scaleX = 0.67,
        scaleY = 0.67,
        rotation = 0.00,
    },

    Name = {
        name = "TimerUI",
        parentID = 8,
        tag = "Untagged",
        layer = "Default",
    },
}

FPS_10 = {
    UIComponent = {
        Type = 4,
    },

    Text = {
        textContent = "FPS: 60",
        font = "",
        positionX = 375.00,
        positionY = 320.00,
        alpha = 0.00,
        RGB_R = 0.00,
        RGB_G = 0.00,
        RGB_B = 0.00,
        fontSize = 1.00,
    },

    Transform = {
        positionX = 375.00,
        positionY = 320.00,
        scaleX = 1.00,
        scaleY = 1.00,
        rotation = 0.00,
    },

    Name = {
        name = "FPS",
        parentID = 9,
        tag = "Untagged",
        layer = "Default",
    },
}

