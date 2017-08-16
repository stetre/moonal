#!/usr/bin/env lua
-- MoonAL example: listener.lua
--
-- Set/get parameters of the context's single listener.

al = require("moonal")

function tostring3(v) -- on-the-fly tostring() for vec3
   return string.format("{%.2f %.2f %.2f}", v[1], v[2], v[3])
end

device = al.open_device()
context = al.create_context(device)
listener1 = context:listener()

listener1:set('meters per unit', 0.8)
print("meters per unit", listener1:get('meters per unit'))

listener1:set('gain', 1.3)
print("gain", listener1:get('gain'))

listener1:set('orientation', {2, 1.5, 3}, {0, 1, 0})
at, up = listener1:get('orientation')
print("orientation", tostring3(at), tostring3(up))

listener1:set('position', { 10, 11, 12 })
print("position", tostring3(listener1:get('position')))

listener1:set('velocity', {1.1, 2.2, 3.123})
print("velocity", tostring3(listener1:get('velocity')))

-- If we create another context, it will have a separate listener object.
-- Setting its parameters will not affect the first (or any other) listener.
context2 = al.create_context(device)
listener2 = context2:listener()
print("listener2 gain", listener2:get('gain'))
print("listener1 gain", listener1:get('gain'))

