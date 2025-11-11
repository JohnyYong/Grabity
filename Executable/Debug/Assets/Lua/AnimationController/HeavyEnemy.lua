AnimationController = {}

AnimationController.Parameters = {
  ["ThrowEnemy"] = { type = "bool", value = false },
  ["HeldEnemy"] = { type = "bool", value = false },
  ["IsDamaged"] = { type = "bool", value = false },
}

AnimationController.States = {
  { name = "Moving", texture = "Animation_Heavy_Enemy", FramesX = 2, FramesY = 1, FramesTotal = 2, FramesPS = 6 }, 
  { name = "Held", texture = "HeavyEnemy_heldSprite1", FramesX = 1, FramesY = 1, FramesTotal = 1, FramesPS = 1 }, 
  { name = "Throw", texture = "HeavyEnemy_projectileSprite1", FramesX = 1, FramesY = 1, FramesTotal = 1, FramesPS = 1 }, 
  { name = "Damaged", texture = "Animation_Heavy_EnemyDamaged", FramesX = 2, FramesY = 3, FramesTotal = 6, FramesPS = 6 }, 
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
