/*!****************************************************************
\file: VfxFollowComponent.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief  Implementation of the VfxFollowComponent class that makes 
        a visual effect follow a target GameObject.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#include "VfxFollowComponent.h"
#include "GameObject.h"

/*!****************************************************************
\brief Updates the VfxFollowComponent.
*******************************************************************!*/
void VfxFollowComponent::Update()
{
    if (target)
    {
        TransformComponent* trans = GetParentGameObject()->GetComponent<TransformComponent>(TRANSFORM);
        if (target->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM))
        {
            trans->SetLocalPosition(target->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->GetPosition());
            ImGuiConsole::Cout("Position %f\n", trans->GetPosition().y);
        }
    }
}
