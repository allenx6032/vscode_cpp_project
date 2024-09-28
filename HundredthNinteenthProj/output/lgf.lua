require("lgf_window")
require("lgf_renderer")
require("lgf_rectangle")
require("lgf_mouse")
require("lgf_keyboard")
require("lgf_image")
require("lgf_font")
require("lgf_utils")

LGF = {}
LGF.__index = LGF

LGF.Window = window
LGF.Renderer = renderer
LGF.Rectangle = rectangle
LGF.Mouse = mouse
LGF.Keyboard = keyboard
LGF.Keyboard.Keycode = keycode
LGF.Image = image
LGF.Font = font
LGF.Text = text
LGF.Utils = utils

return LGF
