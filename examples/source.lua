#!/usr/bin/env lua
-- MoonAL example: source.lua
-- 
-- Get/set parameters of a source object.

al = require("moonal")

al.trace_objects(true)

-- Open a device, create a context and a source object
device = al.open_device()
context = al.create_context(device)
source = al.create_source(context)

-- Utilities to set/get/print parameters:
function tostring3(v) return "{"..v[1]..", "..v[2]..", "..v[3].."}" end
function set(param, ...) source:set(param, ...) end
function get(param) print(param, source:get(param)) end
function get3(param) print(param, tostring3(source:get(param))) end

set("pitch", .7)
get("pitch")
get("gain")
get("min gain")
get("max gain")
get("cone inner angle")
get("cone outer angle")
get("cone outer gain")
get("cone outer gainhf")
get("max distance")
get("rolloff factor")
get("room rolloff factor")
get("air absorption factor")
get("doppler factor")
get("reference distance")
get("sec offset")
get("sample offset")
get("byte offset")
get("sec length")
get("radius")
set("position", { 0, -1, 1 })
get3("position")
set("velocity", { 10, 5, 3 })
get3("velocity")
get3("direction")
get("relative")
get("looping")
get("direct filter gainhf auto")
get("auxiliary send filter gain auto")
get("auxiliary send filter gainhf auto")
get("direct channels")
get("buffers queued")
get("buffers processed")
get("byte length")
get("sample length")
get("sec offset latency")
get("sample offset latency")
at, up = source:get("orientation")
print("orientation", tostring3(at), tostring3(up))
set("stereo angles", 0.8, 0.3)
get("stereo angles")
get("type")
get("state")
get("distance model")
get("resampler")
get("spatialize")
get("buffer")
--get("direct filter")
--get("auxiliary send filter")

