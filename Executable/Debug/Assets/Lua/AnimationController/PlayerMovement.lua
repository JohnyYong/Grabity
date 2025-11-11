AnimationController = {}

AnimationController.Parameters = {
  ["IsMoving"] = { type = "bool", value = false },
}

AnimationController.States = {
  { name = "Idle", texture = "PlayerIdle", FramesX = 1, FramesY = 1, FramesTotal = 1, FramesPS = 1 }, 
  { name = "Moving", texture = "Animation_PlayerWalk", FramesX = 4, FramesY = 2, FramesTotal = 8, FramesPS = 15 }, 
}

AnimationController.Transitions = {
  { from = "Idle", to = "Moving", condition = { param = "IsMoving", compare = "True", threshold = 1 }, time = 0 },
  { from = "Moving", to = "Idle", condition = { param = "IsMoving", compare = "False", threshold = 0 }, time = 0 },
}
