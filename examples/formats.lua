#!/usr/bin/env lua
-- MoonAL example: formats.lua
--
-- Prints a list of all formats, together with their frame size and no. of channels

al = require("moonal")

function printf(...) io.write(string.format(...)) end

-- Get list of all formats:
formats = al.enum("format")

for k, fmt in ipairs(formats) do
   local framesize, channels = al.formatsize(fmt)
   printf("%2d) %-20s framesize: %2d channels: %d\n", k, fmt, framesize, channels)
end
   
