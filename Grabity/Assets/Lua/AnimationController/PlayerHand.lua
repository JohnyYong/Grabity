AnimationController = {}

AnimationController.Parameters = {
  ["EnemyGrab"] = { type = "bool", value = false },
}

AnimationController.States = {
  { name = "HandOpen", texture = "HandOpen", FramesX = 1, FramesY = 1, FramesTotal = 1, FramesPS = 1 }, 
  { name = "HandClose", texture = "HandClose", FramesX = 1, FramesY = 1, FramesTotal = 1, FramesPS = 1 }, 
}

AnimationController.Transitions = {
  { from = "HandOpen", to = "HandClose", condition = { param = "EnemyGrab", compare = "True", threshold = 1 }, time = 0 },
  { from = "HandClose", to = "HandOpen", condition = { param = "EnemyGrab", compare = "False", threshold = 0 }, time = 0 },
}
