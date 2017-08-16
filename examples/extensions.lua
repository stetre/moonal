#!/usr/bin/env lua
-- MoonAL example: extensions.lua
--
-- Lists the supported extensions for the default device.

al = require("moonal")

device = al.open_device()
context = al.create_context(device)

exts = al.get_extensions(context)
print("Found "..#exts.. " extensions: ")
print(table.concat(exts, ', '))
print()

function check(name) 
   print(name..": "..(al.is_extension_present(context, name) and "yes" or "no")) 
end

check("AL_LOKI_IMA_ADPCM_format")
check("AL_LOKI_WAVE_format")
check("AL_EXT_vorbis")
check("AL_LOKI_quadriphonic")
check("AL_EXT_float32")
check("AL_EXT_double")
check("AL_EXT_MULAW")
check("AL_EXT_ALAW")
check("AL_EXT_MCFORMATS")
check("AL_EXT_MULAW_MCFORMATS")
check("AL_EXT_IMA4")
check("AL_EXT_STATIC_BUFFER")
check("AL_EXT_source_distance_model")
check("AL_SOFT_buffer_sub_data")
check("AL_SOFT_loop_points")
check("AL_EXT_FOLDBACK")
check("AL_SOFT_buffer_samples")
check("AL_SOFT_direct_channels")
check("AL_EXT_STEREO_ANGLES")
check("AL_EXT_SOURCE_RADIUS")
check("AL_SOFT_source_latency")
check("AL_SOFT_deferred_updates")
check("AL_SOFT_block_alignment")
check("AL_SOFT_MSADPCM")
check("AL_SOFT_source_length")
check("AL_EXT_BFORMAT")
check("AL_EXT_MULAW_BFORMAT")
check("AL_SOFT_gain_clamp_ex")
check("AL_SOFT_source_resampler")
check("AL_SOFT_source_spatialize")
check("ALC_LOKI_audio_channel")
check("ALC_EXT_EFX")
check("ALC_EXT_disconnect")
check("ALC_EXT_thread_local_context")
check("ALC_EXT_DEDICATED")
check("ALC_SOFT_loopback")
check("ALC_EXT_DEFAULT_FILTER_ORDER")
check("ALC_SOFT_pause_device")
check("ALC_SOFT_HRTF")
check("ALC_SOFT_output_limiter")

print()