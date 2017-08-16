#!/usr/bin/env lua
-- MoonAL example: devices.lua
--
-- List the available devices and try to open/close them.

al = require("moonal")

--al.trace_objects(true)

print()

default, capdefault = al.default_devices()
print("Default device:", default)
print("Default capture device:", capdefault)

print("\nOpening default device")
device = al.open_device()
print("device name:", device:name())
print("Closing default device")
al.close_device(device)

print("\nOpening default capture device")
device = al.open_device()
print("device name:", device:name())
print("Closing default capture device")
al.close_device(device)

-- Get the names of all the available devices:
devices, capdevices = al.available_devices()
print("\nAvailable devices:")
for k, v in ipairs(devices) do print(k .. ": "..v) end
print("\nAvailable capture devices:")
for k, v in ipairs(capdevices) do print(k .. ": "..v) end

-- Try to open/close every available device:
for _, name in ipairs(devices) do
   print("\nOpening device: '"..name.."'")
   local ok, device = pcall(al.open_device, name)
   if not ok then
      print("ERROR", device)
   else
      print("Closing device: '"..name.."'")
      al.close_device(device)
   end
end

-- Try to open/close every available capture device:
for _, name in ipairs(capdevices) do
   print("\nOpening capture device: '"..name.."'")
   local ok, device = pcall(al.capture_open_device, name, 44100, "mono float32", 1024)
   if not ok then
      print("ERROR", device)
   else
      print("Closing capture device: '"..name.."'")
      al.close_device(device)
   end
end

