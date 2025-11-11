/*!****************************************************************
\file:      UISystem.h
\author:    Ridhwan Mohamed Afandi, mohamedridhwan.b, 2301367
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief:     UI System for updating  UI elements in the game world.
\details:   Implements the UpdateUI function to update UI elements
            such as timer, health bar, and FPS counter.

            Jun Jie contributed (30%) of the code with implementing the helper functions and adding more button types.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "UISystem.h"
#include "LayerManager.h"

namespace variables {
    bool isRunning = false;
    bool isRunningBack = false;
    bool isInteractable = true;

    bool isPopupAnimating = false;
    bool isMovingUp = false;
    float popupTimer = 0.0f;

    bool isPopupAnimatingSide = false;
    bool isMovingSide = false;
    float popupTimerSide = 0.0f;

    bool volumeMenuOn = false;
    bool runFadeIntoCutscene = false;
}


/*!****************************************************************
\func  UpdateUI
\brief Updates the UI elements in the game world.
*******************************************************************!*/
void UpdateUI()
{
    if (variables::isInteractable)
    {
        //ImGuiConsole::Cout("Interatable Is On\n");
    }

    if (variables::volumeMenuOn)
    {
        ImGuiConsole::Cout("Volume Menu Bool is ON\n");
    }

    Engine& engine = Engine::GetInstance();
    GameObjectFactory& factory = GameObjectFactory::GetInstance();

    for (const auto& pair : factory.GetAllGameObjects()) {
        GameObject* selectedGO = pair.second;
        ButtonComponent* button = selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON);
        TransformComponent* transform = selectedGO->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        UISpriteComponent* sprite = selectedGO->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
        UITextComponent* text = selectedGO->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);
        UIComponent* ui = selectedGO->GetComponent<UIComponent>(TypeOfComponent::UI);


        // UPDATE UI ELEMENTS
        if (engine.isInGameScene)
        {
            if (ui)
            {
                // update timer
                UIComponentType type = ui->type;
                if (type == UIComponentType::Timer && text && !engine.isPaused)
                {
                    float& time = engine.time;
                    time -= (float)InputManager::GetDeltaTime();
                    int totalSeconds = static_cast<int>(time);
                    int minutes = totalSeconds / 60;
                    int seconds = totalSeconds % 60;
                    std::string timeString = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
                    timeString = timeString;
                    text->SetText(timeString);
                    //text->SetPosition(transform->GetLocalPosition() + engine.cameraManager.GetCurrentCamera()->GetCenter());
                }
                if (type == UIComponentType::FPS && text && !engine.isPaused)
                {
                    std::string fps = "FPS: " + std::to_string((int)InputManager::GetFPS());
                    text->SetText(fps);
                    //text->SetPosition(transform->GetLocalPosition() + engine.cameraManager.GetCurrentCamera()->GetCenter());
                }
                if (type == UIComponentType::PopUp) {

                    UpDownPopup(selectedGO, 1250.f, 1000.f, 5.f);
                }
                if (type == UIComponentType::PopUpLeftRight) {

                    LeftRightPopup(selectedGO, 2400.f, 1850.f);
                }
                if (sprite)
                {
                    GameObject* player = factory.GetPlayerObject();
                    if (!player) return;
                    HealthComponent* healthComponent = player->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
                    if (!healthComponent) return;
                    float currentHealth = static_cast<float>(healthComponent->GetHealth());
                    float maxHealth = static_cast<float>(healthComponent->GetMaxHealth());
                    float healthNormalised = currentHealth / maxHealth;
                    //SpriteAnimation* spriteAnim = sprite->GetCurrentSprite();
                    if (type == UIComponentType::Bar)
                    {
                        // 0.0f uv_x will represent 100% health
                        // 1.0f uv_x will represent 0% health
                        if (ui->originalSize.x == 0.f && ui->originalSize.y == 0.f)
                        {
                            ui->originalSize = transform->GetScale();
                        }
                        float newPosX = transform->GetPosition().x - transform->GetScale().x * 0.5f;
                        transform->SetScale({ ui->originalSize.x * healthNormalised, ui->originalSize.y });
                        newPosX = newPosX + transform->GetScale().x * 0.5f;
                        transform->SetPosition({ newPosX,transform->GetPosition().y });
                    }

                }
            }
        }


        // UPDATING THE BUTTONS
        // IF THE GAME IS PAUSED IN GAME
        if (engine.isPaused && engine.isInGameScene) {
            if (!transform || !sprite || !button) continue;
            //sprite->SetIsRenderable(true);
        }

        else if (!engine.isPaused && engine.isInGameScene) {
            if (!transform || !sprite || !button) continue;
            //sprite->SetIsRenderable(false);
        }

        if (engine.isPaused && !engine.isInGameScene) continue;

        //// IF THE GAME IS PAUSED IN MENU AND IN GAME
        if (engine.cameraManager.GetCurrentMode() == CameraManager::CameraMode::PlayerCamera) {
            if (!transform || !sprite || !button || (button->m_functionType == ButtonFunctionType::PLACEHOLDER)) continue;



#ifdef _IMGUI
            if (engine.scenewindow == nullptr) { return; }
            if (engine.scenewindow->WasActive == false) { return; }

            ImVec2 worldPos = engine.MouseToScreenImGui(engine.GetMousePositionImGui(engine.scenewindow),
                *engine.cameraManager.GetCurrentCamera(),
                engine.scenewindow->Size.x,
                engine.scenewindow->Size.y);
            //if(worldPos)
#else

            // Get mouse position
            double mouseX, mouseY;
            glfwGetCursorPos(InputManager::ptrWindow, &mouseX, &mouseY);
            Vector2 worldPos = engine.MouseToScreen(Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY)),
                *engine.cameraManager.GetCurrentCamera(),
                static_cast<float>(InputManager::GetWidth()),
                static_cast<float>(InputManager::GetHeight()));

            //worldPos = engine.ScreenToWorldFull(Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY)),
            //    *engine.cameraManager.GetCurrentCamera(),
            //    InputManager::GetWidth(),
            //    InputManager::GetHeight());
#endif // _IMGUI

            // Define buttonPos and buttonScale using the transform component
            Vector2 buttonPos = transform->GetPosition();
            Vector2 buttonScale = transform->GetScale();



            bool isHovered = (worldPos.x >= (buttonPos.x - buttonScale.x * 0.5f) && worldPos.x <= (buttonPos.x + buttonScale.x * 0.5f) &&
                worldPos.y >= (buttonPos.y - buttonScale.y * 0.5f) && worldPos.y <= (buttonPos.y + buttonScale.y * 0.5f));

            if (variables::isRunning) {
                for (GameObject* Achievement : GameObjectFactory::GetInstance().FindGameObjectsByTag("Achievement")) {

                    TransformComponent* AchievementTransform = Achievement->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    RigidBodyComponent* RBAchievement = Achievement->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
                    if (AchievementTransform->GetLocalPosition().y >= -5.f) {
                        variables::isRunning = false;
                        RBAchievement->SetVelocity({ 0, 0.f });
                        AchievementTransform->SetLocalPosition({ 0,0 });
                    }

                }
                for (GameObject* Options : GameObjectFactory::GetInstance().FindGameObjectsByTag("Options")) {

                    TransformComponent* OptionsTransform = Options->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    RigidBodyComponent* RBOptions = Options->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
                    if (OptionsTransform->GetLocalPosition().y >= -5.f) {
                        variables::isRunning = false;
                        RBOptions->SetVelocity({ 0, 0.f });
                        OptionsTransform->SetLocalPosition({ 0,0 });
                    }

                }
                for (GameObject* HTPlay : GameObjectFactory::GetInstance().FindGameObjectsByTag("HowToPlay")) {

                    TransformComponent* HTPlayTransform = HTPlay->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    RigidBodyComponent* RBHTPlay = HTPlay->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
                    if (HTPlayTransform->GetLocalPosition().y >= -5.f) {
                        variables::isRunning = false;
                        RBHTPlay->SetVelocity({ 0, 0.f });
                        HTPlayTransform->SetLocalPosition({ 0,0 });
                    }

                }

            }
            if (variables::isRunningBack) {
                // Move all Achievements
                for (GameObject* Achievement : GameObjectFactory::GetInstance().FindGameObjectsByTag("Achievement")) {
                    TransformComponent* AchievementTransform = Achievement->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    RigidBodyComponent* RBAchievement = Achievement->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                    RBAchievement->SetVelocity({ 0, 0.f });
                    AchievementTransform->SetLocalPosition({ 0, -1000.f });
                    variables::volumeMenuOn = false;
                }

                // Move all Options
                for (GameObject* Options : GameObjectFactory::GetInstance().FindGameObjectsByTag("Options")) {
                    TransformComponent* OptionsTransform = Options->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    RigidBodyComponent* RBOptions = Options->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                    RBOptions->SetVelocity({ 0, 0.f });
                    OptionsTransform->SetLocalPosition({ 0, -1000.f });
                    variables::volumeMenuOn = false;

                }

                // Move all HowToPlay
                for (GameObject* HTPlay : GameObjectFactory::GetInstance().FindGameObjectsByTag("HowToPlay")) {
                    TransformComponent* HTPlayTransform = HTPlay->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    RigidBodyComponent* RBHTPlay = HTPlay->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                    RBHTPlay->SetVelocity({ 0, 0.f });
                    HTPlayTransform->SetLocalPosition({ 0, -1000.f });
                    variables::volumeMenuOn = false;
                }

                // Now that all objects are set, disable isRunningBack
                variables::isRunningBack = false;
            }

            if (isHovered) {
                // Change sprite color on hover 
                if (!variables::volumeMenuOn && selectedGO->GetTag() == "Hovering") {
                    if (engine.showCursor) {
                        sprite->SetIsRenderable(true);
                    }
                }
            }

            static bool clickHandled = false; // Track if the click was already handled for audio
            if (isHovered && sprite->GetIsRenderable()) {
                if (!engine.showCursor && selectedGO->GetTag() == "Hovering") {
                    sprite->SetIsRenderable(false);
                    continue;
                }

                if (!selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->sfxPlay)
                {
                    if (!variables::volumeMenuOn)
                    {
                        AudioManager::GetInstance().PlayAudio(10);
                        AudioManager::GetInstance().SetChannelVolume(10, 0.3f);
                        selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->sfxPlay = true;
                    }
                }
                //// Change sprite color on hover 
                //if (!variables::volumeMenuOn && selectedGO->GetTag() == "Hovering") {
                //    //sprite->SetColor(Vector4(0.4f, 0.4f, 0.4f, 1.0f));
                //    sprite->SetIsRenderable(true);
                //    
                //}

                // Check if the button is clicked
                if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {


                    if (!clickHandled && variables::isInteractable) {  // Prevent multiple triggers
                        
                        if (selectedGO->GetTag() == "StartButton")
                        {
                            AudioManager::GetInstance().PlayAudio(19);
                            AudioManager::GetInstance().SetChannelVolume(19, 1.2f);  // Make this audio louder
                        }
                        else
                        {
                            AudioManager::GetInstance().PlayAudio(9);
                            AudioManager::GetInstance().SetChannelVolume(19, 0.5f);  // Make this audio softer
                        }
                        clickHandled = true;
                    }


                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::LOAD_NEXT_SCENE && variables::isInteractable)
                    {
                        EventSystem::GetInstance().ShutDown();
                        engine.cameraManager.GetCurrentCamera()->SetCenter(Vector2(0.0f, 0.0f));
                        if (selectedGO) {
                            if (button)
                            {
                                if (button->pathNextScene == "Assets/Lua/Scenes/MainMenuScene.lua") //If next scenes is         not game, so it will play if go into main menu and others
                                {
                                    AudioManager::GetInstance().PlayAudio(16);
                                    engine.time = engine.maxTime;

                                    variables::isInteractable = true;
                                    variables::volumeMenuOn = false;
                                    engine.showCursor = true;
                                }
                                else if (button->pathNextScene == "Assets/Lua/Scenes/GameScene.lua")
                                {
                                    AudioManager::GetInstance().StopAudio(16);
                                }
                            }
                        }

                        if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->pathNextScene == "Assets/Lua/Scenes/CutScene.lua")
                        {
                            variables::runFadeIntoCutscene = true;
                            AudioManager::GetInstance().PlayAudio(18);
                        }
                        else
                        {
                            engine.LoadSceneFromLua(selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->pathNextScene); //Goes into scene immediately
                        }

                        engine.isInGameScene = true;

                        if (engine.isInGameScene) {
                            engine.isPaused = false;
                            engine.time = engine.maxTime;

                            for (GameObject* player : GameObjectFactory::GetInstance().FindGameObjectsByTag("Player")) {
                                auto* playController = player->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);

                                if (playController) {
                                    playController->playBGM = false;
                                    playController->changeBGM = false;
                                }
                            }

                            //// ?? If BGM is not playing, restart it
                            //if (!AudioManager::GetInstance().IsPlaying(8)) {
                            //    AudioManager::GetInstance().PlayAudio(8);
                            //    ImGuiConsole::Cout("Restarting BGM after scene load\n");
                            //}
                        }

                        return;
                    }
                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::EXIT_GAME)
                    {
                        if (variables::isInteractable)
                            engine.Exit();
                    }
                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::RESUME_GAME)
                    {
                        if (engine.isPaused)
                        {
                            engine.isPaused = false;
                        }
                        engine.showCursor = ~engine.showCursor;

                        for (GameObject* PauseUI : GameObjectFactory::GetInstance().FindGameObjectsByTag("UI"))
                        {
                            auto* pauseMenuComponent = PauseUI->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                            if (pauseMenuComponent)
                            {
                                pauseMenuComponent->SetIsRenderable(false);
                                AudioManager::GetInstance().PlayAudio(9);
                                AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
                            }
                        }
                        return;
                    }
                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::RESTART_LEVEL)
                    {

                        engine.cameraManager.GetCurrentCamera()->SetCenter(Vector2(0.0f, 0.0f));
                        //AudioManager::GetInstance().PlayAudio(16);
                        AudioManager::GetInstance().PlayAudio(10);
                        AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
                        engine.RestartScene();

                        if (engine.isInGameScene)
                        {
                            engine.isPaused = false;
                            engine.time = engine.maxTime;

                            for (GameObject* player : GameObjectFactory::GetInstance().FindGameObjectsByTag("Player"))
                            {
                                auto* playController = player->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);

                                if (playController)
                                {
                                    //playController->playBGM = false;
                                    //playController->changeBGM = false;
                                }
                            }
                        }
                        return;
                    }
                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::ACHIEVEMENT) {
                        if (variables::isInteractable) {
                            for (GameObject* Achievement : GameObjectFactory::GetInstance().FindGameObjectsByTag("Achievement")) {


                                RigidBodyComponent* RBAchievement = Achievement->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                                variables::isRunning = true;
                                variables::isInteractable = false;
                                RBAchievement->SetVelocity({ 0, 5000.f });
                                std::cout << "GO: " << Achievement->GetName() << " is pressed to go up\n";
                            }
                        }
                    }
                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::ACHIEVEMENTBACK) {
                        for (GameObject* Achievement : GameObjectFactory::GetInstance().FindGameObjectsByTag("Achievement")) {

                            RigidBodyComponent* RBAchievement = Achievement->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                            variables::isRunningBack = true;
                            variables::isInteractable = true;
                            RBAchievement->SetVelocity({ 0, -5000.f });
                            std::cout << "GO: " << Achievement->GetName() << " is pressed to go down\n";

                        }
                        for (GameObject* Options : GameObjectFactory::GetInstance().FindGameObjectsByTag("Options")) {


                            RigidBodyComponent* RBOptions = Options->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                            variables::isRunningBack = true;
                            variables::isInteractable = true;
                            RBOptions->SetVelocity({ 0, -5000.f });

                        }
                        for (GameObject* HTPlay : GameObjectFactory::GetInstance().FindGameObjectsByTag("HowToPlay")) {


                            RigidBodyComponent* RBHTPlay = HTPlay->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                            variables::isRunningBack = true;
                            variables::isInteractable = true;
                            RBHTPlay->SetVelocity({ 0, -5000.f });
                            AudioManager::GetInstance().PlayAudio(9);
                            AudioManager::GetInstance().SetChannelVolume(9, 0.4f);

                        }
                    }
                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::OPTIONS) {
                        if (variables::isInteractable) {
                            for (GameObject* Options : GameObjectFactory::GetInstance().FindGameObjectsByTag("Options")) {


                                RigidBodyComponent* RBOptions = Options->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                                variables::isRunning = true;
                                variables::isInteractable = false;
                                RBOptions->SetVelocity({ 0, 5000.f });
                                variables::volumeMenuOn = true;
                                AudioManager::GetInstance().PlayAudio(9);
                                AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
                            }
                        }
                    }
                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::HOWTOPLAY) {
                        if (variables::isInteractable) {
                            for (GameObject* Options : GameObjectFactory::GetInstance().FindGameObjectsByTag("HowToPlay")) {


                                RigidBodyComponent* RBOptions = Options->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

                                variables::isRunning = true;
                                variables::isInteractable = false;
                                RBOptions->SetVelocity({ 0, 5000.f });
                                variables::volumeMenuOn = true;
                                AudioManager::GetInstance().PlayAudio(9);
                                AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
                                

                            }
                        }
                    }




                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::SOUND_POPUP)
                    {
                        GameObjectFactory::GetInstance().FindGameObjectsByTag("SoundPopUp");

                        bool soundPU = true;

                        if (soundPU) {

                        }

                    }

                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::CHANGEIMAGEFORWARD)
                    {
                        std::vector<GameObject *> holder = GameObjectFactory::GetInstance().FindGameObjectsByTag("ChangeHowToPlay");
                        for (GameObject* obj : holder)
                        {
                            if (obj)
                            {
                                UISpriteComponent* spritehtp = obj->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                                if (spritehtp)
                                {
                                    spritehtp->ChangeSprite(std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite("HowToPlay2"))); // Assuming 1 is the next state
                                }
                            }
                            
                        }

                        std::vector<GameObject*> holder2 = GameObjectFactory::GetInstance().FindGameObjectsByTag("ChangeButton2");
                        for (GameObject* obj : holder2)
                        {
                            if (obj)
                            {
                                UISpriteComponent* spritehtp = obj->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                                if (spritehtp)
                                {
                                    spritehtp->SetIsRenderable(false);
                                    AudioManager::GetInstance().PlayAudio(9);
                                    AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
                                }
                            }

                        }
                        std::vector<GameObject*> holder3 = GameObjectFactory::GetInstance().FindGameObjectsByTag("ChangeButton1");
                        for (GameObject* obj : holder3)
                        {
                            if (obj)
                            {
                                UISpriteComponent* spritehtp = obj->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                                if (spritehtp)
                                {
                                    spritehtp->SetIsRenderable(true);
                                    AudioManager::GetInstance().PlayAudio(9);
                                    AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
                                }
                            }

                        }

                    }

                    if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->m_functionType == ButtonFunctionType::CHANGEIMAGEBACKWARD)
                    {
                        std::vector<GameObject*> holder = GameObjectFactory::GetInstance().FindGameObjectsByTag("ChangeHowToPlay");
                        for (GameObject* obj : holder)
                        {
                            if (obj)
                            {
                                UISpriteComponent* spritehtp = obj->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                                if (spritehtp)
                                {
                                    spritehtp->ChangeSprite(std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite("HowToPlay1"))); // Assuming 1 is the next state
                                }
                            }
                        }

                        std::vector<GameObject*> holder2 = GameObjectFactory::GetInstance().FindGameObjectsByTag("ChangeButton1");
                        for (GameObject* obj : holder2)
                        {
                            if (obj)
                            {
                                UISpriteComponent* spritehtp = obj->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                                if (spritehtp)
                                {
                                    spritehtp->SetIsRenderable(false);
                                    AudioManager::GetInstance().PlayAudio(9);
                                    AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
                                }
                            }

                        }
                        std::vector<GameObject*> holder3 = GameObjectFactory::GetInstance().FindGameObjectsByTag("ChangeButton2");
                        for (GameObject* obj : holder3)
                        {
                            if (obj)
                            {
                                UISpriteComponent* spritehtp = obj->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                                if (spritehtp)
                                {
                                    spritehtp->SetIsRenderable(true);
                                    AudioManager::GetInstance().PlayAudio(9);
                                    AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
                                }
                            }

                        }

                    }

                }
            }
            else {
                // Reset sprite color
                sprite->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
                //sprite->SetIsRenderable(true);

                if (!variables::volumeMenuOn && selectedGO->GetTag() == "Hovering") {
                    sprite->SetIsRenderable(false);
                }

                if (selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->sfxPlay)
                {
                    selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON)->sfxPlay = false;
                }


            }
            if (!InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
                //reset audio trigger
                clickHandled = false;

            }
        }
    }
}



/*!****************************************************************
\func  triggerUpDown
\brief Starts the popup animation when triggered.
\details This function is responsible for initiating the popup animation
         by setting the relevant state variables (`isPopupAnimating` and
         `isMovingUp`) and resetting the timer for the animation.
*******************************************************************!*/
void TriggerUpDown() {
    if (!variables::isPopupAnimating) {
        // Start the popup animation
        variables::isPopupAnimating = true;
        variables::isMovingUp = true;  // Start moving up
        variables::popupTimer = 0.f;   // Reset timer
    }
}


/*!****************************************************************
\func  textChange
\brief Updates the text on specific UI elements based on the enemy type.
\param num The current value to be displayed.
\param tagname The tag name for identifying the target UI element.
\param total The total value to be displayed alongside the current number.
\details This function updates the text on specific UI elements for
         different enemy types (light, heavy, and bomb enemies).
         It uses the `tagname` to identify which enemy's text component
         to update with the given values (`num` and `total`).
*******************************************************************!*/
void TextChange(int num, std::string tagname, int total) {
    for (auto& pair : GameObjectFactory::GetInstance().GetAllGameObjects()) {
        GameObject* GO = pair.second;

        //for light enemy
        if (tagname == "TextChangeSlime" && pair.second->GetTag() == "TextChangeSlime" && GO != nullptr) {
            auto* uitext = GO->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);

            if (uitext != nullptr) {
                uitext->SetText(std::to_string(num) + "/" + std::to_string(total));
            }
        }

        //for heavy enemy
        if (tagname == "TextChangeSkeleton" && pair.second->GetTag() == "TextChangeSkeleton" && GO != nullptr) {
            auto* uitext = GO->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);

            if (uitext != nullptr) {
                uitext->SetText(std::to_string(num) + "/" + std::to_string(total));
            }
        }

        //for bomb enemy
        if (tagname == "TextChangeBomb" && pair.second->GetTag() == "TextChangeBomb" && GO != nullptr) {
            auto* uitext = GO->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);

            if (uitext != nullptr) {
                uitext->SetText(std::to_string(num) + "/" + std::to_string(total));
            }
        }
    }

}


/*!****************************************************************
\func  SetGreen
\brief Sets the color of a specific enemy's sprite to green.
\param enemytype The type of enemy to be updated.
\details This function changes the color of a specified enemy type (light, heavy, or bomb)
         to green (indicating a completed or successful state).
         It uses the `enemytype` to determine which enemy to update and sets the color
         of its sprite accordingly.
*******************************************************************!*/
void SetGreen(EnemyType enemytype) {

    for (auto& pair : GameObjectFactory::GetInstance().GetAllGameObjects()) {
        GameObject* GO = pair.second;
            
        if (enemytype == EnemyType::Light && pair.second->GetTag() == "SlimeHunt" && GO != nullptr) {
            UISpriteComponent* sprite = GO->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);

            if (sprite != nullptr) {
                // Set the color to green
                sprite->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // RGBA: green
            }

        }

        if (enemytype == EnemyType::Heavy && pair.second->GetTag() == "SkeletonHunt" && GO != nullptr) {
            UISpriteComponent* sprite = GO->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);

            if (sprite != nullptr) {
                // Set the color to green
                sprite->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // RGBA: green
            }

        }

        if (enemytype == EnemyType::Bomb && pair.second->GetTag() == "BombHunt" && GO != nullptr) {
            UISpriteComponent* sprite = GO->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);

            if (sprite != nullptr) {
                // Set the color to green
                sprite->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // RGBA: green
            }

        }
    }


}


/*!****************************************************************
\func  UpDownPopup
\brief Moves a popup UI element up and down with a pause in between.
\param selectedGO The GameObject representing the popup.
\param maxY The maximum Y position (top position).
\param minY The minimum Y position (bottom position).
\param pauseTime The time to pause at the maxY position before moving down.
\details This function moves a popup UI element up towards `minY` and then,
         after a pause, moves it back down towards `maxY`. The animation
         logic ensures that the popup moves smoothly and stays at the top
         for a defined period (`pauseTime`) before moving back down.
*******************************************************************!*/
void UpDownPopup(GameObject* selectedGO, float maxY, float minY, float pauseTime) {

    UITextComponent* uiCom = selectedGO->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);
    TransformComponent* transform = selectedGO->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    Engine& engine = Engine::GetInstance();



    if (variables::isPopupAnimating) {
        float currentY = transform->GetLocalPosition().y;
        float currentUIY = uiCom->GetPosition().y;

        if (variables::isMovingUp) {
            
            if (currentY > minY) {
                transform->SetLocalPosition({ transform->GetLocalPosition().x, currentY - 10.f });
                uiCom->SetPosition({ uiCom->GetPosition().x, currentUIY - 10.f });
            }
            else {
                
                variables::isMovingUp = false;
                variables::popupTimer = engine.time;  // Store current engine time
            }
        }
        else {
            // Wait for 3 seconds before moving down
            if (variables::popupTimer - engine.time >= pauseTime) {
                if (currentY < maxY) {  // Ensure it moves fully off-screen
                    transform->SetLocalPosition({ transform->GetLocalPosition().x, currentY + 5.f });
                    uiCom->SetPosition({ uiCom->GetPosition().x, currentUIY + 5.f });
                }
                else {
                    // Stop animation once it's fully out of the screen
                    variables::isPopupAnimating = false;
                }
            }
        }
    }
}

/*!****************************************************************
\func  LeftRightPopup
\brief Moves a popup UI element left and right.
\param selectedGO The GameObject representing the popup.
\param maxX The maximum X position (right position).
\param minX The minimum X position (left position).
\details This function moves a popup UI element left and right between the
         `maxX` (right) and `minX` (left) positions based on user input.
         The popup will move outwards and inwards based on the state of
         `isMovingOut` and the `Tab` key press. It also adjusts the positions
         of any child UI elements to stay synchronized with the parent.
*******************************************************************!*/
void LeftRightPopup(GameObject* selectedGO, float maxX, float minX) {


    static bool isMovingOut = true;  // State to track whether the popup is moving out or in

    TransformComponent* transform = selectedGO->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    std::vector<GameObject*>& children = selectedGO->GetChildren();

    float speed = 1000.f; // Units per second
    float moveAmount = speed * static_cast<float>(InputManager::deltaTime);

    if (InputManager::IsKeyPressed(GLFW_KEY_TAB)) {
        // Toggle the isMovingOut state when Tab is pressed
        isMovingOut = !isMovingOut;
        AudioManager::GetInstance().PlayAudio(9);
        AudioManager::GetInstance().SetChannelVolume(9, 0.4f);
    }

    float currentX = transform->GetLocalPosition().x;
    float newX;

    if (isMovingOut) {
        // Move out to the right
        newX = currentX + moveAmount;
        if (newX > maxX) newX = maxX; // Clamp to maxX
    }
    else {
        // Move in to the left
        newX = currentX - moveAmount;
        if (newX < minX) newX = minX; // Clamp to minX
    }

    transform->SetLocalPosition({ newX, transform->GetLocalPosition().y });

    // Move each child's UITextComponent
    for (GameObject* child : children) {
        UITextComponent* uiText = child->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);
        TransformComponent* transformchild = child->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (transformchild) {
            float childNewposX = transformchild->GetLocalPosition().x; // Adjust proportionally
            transformchild->SetLocalPosition({ childNewposX, transformchild->GetLocalPosition().y });
        }
        if (uiText) {
            float childNewX = uiText->GetPosition().x + (newX - currentX); // Adjust proportionally
            uiText->SetPosition({ childNewX, uiText->GetPosition().y });
        }
    }


}
