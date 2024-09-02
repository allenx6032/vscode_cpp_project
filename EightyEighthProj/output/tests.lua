-- function __FILE__() return debug.getinfo(2,'S').source end
-- function __DIR__() return string.gsub(debug.getinfo(2,'S').source, "^@(.+/)[^/]+$", "%1") end
-- function __LINE__() return debug.getinfo(2, 'l').currentline end

-- Usage:
local pprint = require("pprint")
local glfw = require('moonglfw')
local gl = require('moongl')
local vg = require("lunavg")

print(glfw._VERSION)
print(glfw._GLFW_VERSION)
print(glfw.get_version())
print(glfw.get_version_string())

local function error_callback(ec, description)
  io.stderr:write(string.format("Error: %s (%d)\n", description, ec))
end

local function key_callback(window, key, scancode, action)
  print(window, key, scancode, action)
  if key == 'escape' and action == 'press' then
     glfw.set_window_should_close(window, true)
  end
end

glfw.set_error_callback(error_callback)
glfw.window_hint('context version major', 2)
glfw.window_hint('context version minor', 0)

local window = glfw.create_window(640, 480, "Test")
glfw.set_key_callback(window, key_callback)
glfw.make_context_current(window)
-- glfw.version_hint(2, 0, "GLFW_OPENGL_COMPAT_PROFILE")
gl.init()
glfw.swap_interval(1)

collectgarbage()
collectgarbage('stop')
while not glfw.window_should_close(window) do
   local width, height = glfw.get_framebuffer_size(window)
   local ratio = width/height
   gl.viewport(0, 0, width, height)
   gl.clear('color')
   --vg.BeginFrame(vg, width, height, ratio);
   --vg.EndFrame(vg);
   glfw.swap_buffers(window)
   collectgarbage('step')
   glfw.poll_events()
end

glfw.destroy_window(window)
