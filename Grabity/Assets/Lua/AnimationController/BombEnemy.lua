AnimationController = {}

AnimationController.Parameters = {
  ["ThrowEnemy"] = { type = "bool", value = false },
  ["HeldEnemy"] = { type = "bool", value = false },
  ["IsDamaged"] = { type = "bool", value = false },
}

AnimationController.States = {
  { name = "Moving", texture = "Animation_Bomb_Enemy", FramesX = 3, FramesY = 1, FramesTotal = 3, FramesPS = 3 }, 
  { name = "Held", texture = "jackbox_heldSprite1", FramesX = 1, FramesY = 1, FramesTotal = 1, FramesPS = 1 }, 
  { name = "Throw", texture = "Animation_Bomb_EnemyExploding", FramesX = 6, FramesY = 1, FramesTotal = 6, FramesPS = 3 }, 
  { name = "Damaged", texture = "Animation_Bomb_EnemyDamaged", FramesX = 5, FramesY = 1, FramesTotal = 5, FramesPS = 5 }, 
}

AnimationController.Transitions = {
  { from = "Moving", to = "Held", condition = { param = "HeldEnemy", compare = "True", threshold = 1 }, time = 0 },
  { from = "Moving", to = "Throw", condition = { param = "ThrowEnemy", compare = "True", threshold = 1 }, time = 0 },
  { from = "Held", to = "Throw", condition = { param = "ThrowEnemy", compare = "True", threshold = 1 }, time = 0 },
  { from = "Held", to = "Moving", condition = { param = "HeldEnemy", compare = "False", threshold = 0 }, time = 0 },
  { from = "Throw", to = "Held", condition = { param = "HeldEnemy", compare = "True", threshold = 1 }, time = 0 },
  { from = "Throw", to = "Moving", condition = { param = "ThrowEnemy", compare = "False", threshold = 0 }, time = 0 },
  { from = "Moving", to = "Damaged", condition = { param = "IsDamaged", compare = "True", threshold = 1 }, time = 0 },
  { from = "Damaged", to = "Moving", condition = { param = "IsDamaged", compare = "False", threshold = 0 }, time = 0 },
}
