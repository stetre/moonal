## MoonAL: Lua bindings for OpenAL

MoonAL is a Lua binding library for the [OpenAL 3D Audio Library API](https://openal.org).

It runs on GNU/Linux and on Windows (MSYS2/MinGW) and requires
[Lua](http://www.lua.org/) (>=5.3) and [OpenAL Soft](http://openal-soft.org) (>= 1.18.0).


_Author:_ _[Stefano Trettel](https://www.linkedin.com/in/stetre)_

[![Lua logo](./doc/powered-by-lua.gif)](http://www.lua.org/)

#### License

MIT/X11 license (same as Lua). See [LICENSE](./LICENSE).

#### Documentation

See the [Reference Manual](https://stetre.github.io/moonal/doc/index.html).

#### Getting and installing

Setup the build environment as described [here](https://github.com/stetre/moonlibs), then:

```sh
$ git clone https://github.com/stetre/moonal
$ cd moonal
moonal$ make
moonal$ sudo make install
```

Note: that MoonAL does not link directly to the OpenAL library (`libopenal.so` on Linux), 
but it builds dynamically its own internal dispatch tables instead.
As a consequence, `libopenal.so` is not needed at compile time but it is required 
at runtime, so you have to make sure that it is reachable by the linker. 
You can do this by installing it in the standard search directories (e.g. `/usr/lib/`),
or by properly setting the LD_LIBRARY_PATH environment variable in the shell where you execute
the Lua scripts. 

#### Example

The example below generates and plays a sinusoidal tone on the default output device.

Other examples can be found in the **examples/** directory contained in the release package.

```lua
-- MoonAL example: hello.lua
--
-- Plays a 440hz sinusoidal tone for a few seconds on the default device.
--
-- This example is derived from the altonegen.c example that comes with
-- OpenAL Soft version 1.18.0.
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
al.source_play(source)

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
```

The script can be executed at the shell prompt with the standard Lua interpreter:

```shell
$ lua hello.lua
```

#### See also

* [MoonLibs - Graphics and Audio Lua Libraries](https://github.com/stetre/moonlibs).
