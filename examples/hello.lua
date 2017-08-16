#!/usr/bin/env lua
-- MoonAL example: hello.lua
--
-- Plays a 440hz sinusoidal tone for a few seconds on the default device.
--
-- This example is derived from the altonegen.c example that comes with
-- OpenAL Soft version 1.18.0.
--
al = require('moonal')

-- Open device and create context:
device = al.open_device()
context = al.create_context(device)

-- Get the device's output frequency:
srate = context:get_attribute('frequency')

-- Generate the sine wave:
data = {}
for i = 1, srate do data[i]=0.0 end
tone_freq = 440
max_loops = 4
smps_per_cycle = srate / tone_freq
for i = 1, srate do
   data[i] = data[i] + math.sin((i-1)/smps_per_cycle*2*math.pi)
end
data = al.pack('float', data)

-- Buffer the audio data into a buffer object:
buffer = al.create_buffer(context)
al.buffer_data(buffer, 'mono float32', data, srate)

-- Create the source to play the sound with, and set its buffer to
-- the buffer containing the sine wave:
source = al.create_source(context)
source:set('buffer', buffer)

-- Play the sound for a while:
num_loops = 0
last_pos = 0
source:set('looping', max_loops > 0)
source:play()

print("Playing for ".. (max_loops +1) .." seconds ...")
while source:get('state') == 'playing' do
   al.sleep(0.1)
   pos = source:get('sample offset')
   if pos < last_pos then
      num_loops = num_loops + 1
      if num_loops >= max_loops then
         source:set('looping', false)
      end
      print(math.tointeger(max_loops - num_loops + 1) .. " ...")
   end
   last_pos = pos
end

al.close_device(device) -- unneeded, actually (automatically done at exit)

