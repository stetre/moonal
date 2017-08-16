#!/bin/bash
# Executes a lua script with the PulseAudio OSS wrapper (see man padsp(1) for details).
# Use this instead of the plain Lua interpreter if, when running the examples on GNU/Linux,
# you get an error such as "Could not open /dev/dsp (etc.)".

padsp lua $@

