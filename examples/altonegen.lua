#!/usr/bin/env lua
-- MoonAL example: altonegen.lua
--
-- OpenAL tone generator. This is the MoonAL version of the altonegen.c
-- example that comes with the OpenAL-SOFT package.

al = require('moonal')
getopt = require('getopt')

PI = math.pi

function printf(...) io.write(string.format(...)) end

--------------------------------------------------------------------------------
-- Command line parsing
--------------------------------------------------------------------------------

USAGE = [[

OpenAL Tone Generator (Lua version)

Usage: ]]..arg[0]..[[ <options>

Available options:
  --help/-h                This help text
  --device/-d              Device name
  -t <seconds>             Time to play a tone (default 5 seconds)
  --waveform/-w <type>     Waveform type: sine (default), square, sawtooth,
                           triangle, impulse, noise
 --freq/-f <hz>            Tone frequency (default 440 hz)
 --srate/-s <sample rate>  Sampling rate (default output rate)

]]

function Usage(s) print(USAGE); os.exit(true) end

optarg, optind, opterr = getopt(arg, 
   "hd:t:w:f:s:", { help='h', device='d', waveform='w', freq='f', srate='s' })

if opterr then print(opterr, 'n') Usage() end
if optarg.h then Usage() end

device_name = optarg.d -- defaults to nil
tone_freq = tonumber(optarg.f) or 440
srate = optarg.s
wavetype  = optarg.w or 'sine'
duration = tonumber(optarg.t) or 5

max_loops = duration - 1 -- @@ check > 0
if tone_freq < 1 then error("Invalid tone frequency: "..tone_freq.." (min: 1hz)") end
if srate and srate < 40 then error("Invalid sample rate: "..srate.." (min: 40hz)") end


--------------------------------------------------------------------------------
-- Waveform generation
--------------------------------------------------------------------------------

function ApplySin(data, g, srate, freq)
   local smps_per_cycle = srate / freq
   for i = 1, srate do
      data[i] = data[i] + math.sin((i-1)/smps_per_cycle * 2.0*PI) * g
   end
end

function CreateWave(context, wavetype, freq, srate)
-- Generates waveforms using additive synthesis. Each waveform is constructed
-- by summing one or more sine waves, up to (and excluding) nyquist.
-- wavetype: 'sine'|'square'|'sawtooth'|'triangle'|'impulse'|'noise'
-- 
   local data = {}
   for i = 1, srate do data[i]=0.0 end
   local imax = math.floor(srate/2/freq) -- nyquist 
   
   if wavetype == 'sine' then
      ApplySin(data, 1.0, srate, freq)
   elseif wavetype == 'square' then
      for i = 1, imax, 2 do
         ApplySin(data, 4.0/PI * 1.0/i, srate, freq*i)
      end
   elseif wavetype == 'sawtooth' then
      for i = 1, imax, 1 do
         ApplySin(data, 2.0/PI * ((i&1)*2 - 1.0) / i, srate, freq*i)
      end
   elseif wavetype == 'triangle' then
      for i = 1, imax, 2 do
         ApplySin(data, 8.0/(PI*PI) * (1.0 - (i&2)) / (i*i), srate, freq*i)
      end
   elseif wavetype == 'impulse' then
      -- NOTE: Impulse isn't handled using additive synthesis, and is instead 
      -- just a non-0 sample at a given rate. This can still be useful to test
      -- (other than resampling, the ALSOFT_DEFAULT_REVERB environment variable 
      -- can prove useful here to test the reverb response).
      for i = 1, srate do
         if (i-1)%(srate/freq) == 0 then data[i] = 1 end
      end
   elseif wavetype == 'noise' then
      -- NOTE: WhiteNoise is just uniform set of uncorrelated values, and is not
      -- influenced by the waveform frequency.
      math.randomseed(os.time())
      for i = 1, srate do 
         data[i] = (math.random() - 0.5)*2 -- (uniform in [-1, 1)
      end
   else
      error("Unhandled waveform: " .. wavetype)
   end

   -- Buffer the audio data into a new buffer object.
   local buffer = al.create_buffer(context)
   al.buffer_data(buffer, 'mono float32', al.pack('float', data), srate)

   return buffer
end

--------------------------------------------------------------------------------
-- MAIN 
--------------------------------------------------------------------------------

--al.trace_objects(true)

-- Open device and create context:
device = al.open_device()
context = al.create_context(device)
if not al.is_extension_present(context, 'AL_EXT_FLOAT32') then
   error("Required AL_EXT_FLOAT32 extension not supported on this device!")
end

-- Get the device's output frequency:
dev_rate = context:get_attribute('frequency')

if not srate then srate = dev_rate end

-- Load the sound into a buffer:
buffer = CreateWave(context, wavetype, tone_freq, srate)

printf("Playing %dhz %s-wave tone with %dhz sample rate and %dhz output, for %d second%s...\n",
           tone_freq, wavetype, srate, dev_rate, max_loops+1, max_loops and "s" or "")

-- Create the source to play the sound with, and set its buffer:
source = al.create_source(context)
source:set('buffer', buffer)

-- Play the sound for a while:
num_loops = 0
last_pos = 0
source:set('looping', max_loops > 0)
source:set('velocity', { 1, 1, 10 })
source:play()

while source:get('state') == 'playing' do
   al.sleep(0.1)
   pos = source:get('sample offset')
   if pos < last_pos then
      num_loops = num_loops + 1
      if num_loops >= max_loops then
         source:set('looping', false)
      end
      print(math.tointeger(max_loops - num_loops + 1) .. "...")
   end
   last_pos = pos
end

al.close_device(device) -- unneeded, actually (automatically done at exit)

