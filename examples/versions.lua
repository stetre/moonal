#!/usr/bin/env lua
-- MoonAL example: versions.lua
--
-- Dealing with versions.

al = require("moonal")

-- The functions we'll use below refer to the runtime (openal.so) version and in
-- order to be used they need an AL context to be created.
-- So let's create one, using the default device:

device = al.open_device()
context = al.create_context(device)

print("MoonAL version  : " .. al._VERSION)
print("OpenAL version  : " .. al.get_version())
print("OpenAL ALC version  : " .. al.get_alc_version())
print("OpenAL EFX version  : " .. al.get_efx_version())
print("OpenAL renderer : " .. al.get_renderer())
print("OpenAL vendor   : " .. al.get_vendor())

