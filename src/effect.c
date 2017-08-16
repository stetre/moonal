/* The MIT License (MIT)
 *
 * Copyright (c) 2017 Stefano Trettel
 *
 * Software repository: MoonAL, https://github.com/stetre/moonal
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"

static int freeeffect(lua_State *L, ud_t *ud)
    {
    effect_t effect = (effect_t)ud->handle;
    LPALDELETEEFFECTS DeleteEffects = ud->ddt->DeleteEffects;
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(effect, "effect");
    DeleteEffects(1, &effect->name);
    Free(L, effect);
    CheckErrorAl(L); 
    return 0;
    }


static int Create(lua_State *L)
    {
    ALuint name;
    effect_t effect;
    ud_t *ud, *context_ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &context_ud);
    
    CheckDevicePfn(L, context_ud, GenEffects);

    context_ud->ddt->GenEffects(1, &name);
    CheckErrorRestoreAl(L, old_context);

    effect = (object_t*)MallocNoErr(L, sizeof(object_t));
    if(!effect)
        {
        make_context_current(L, old_context);
        context_ud->ddt->DeleteEffects(1, &name);
        return luaL_error(L, errstring(ERR_MEMORY));
        }
    
    effect->name = name;
    ud = newuserdata(L, effect, EFFECT_MT);
    ud->context = context;
    ud->device = context_ud->device;
    ud->parent_ud = context_ud;
    ud->destructor = freeeffect;
    ud->ddt = context_ud->ddt;
    ud->cdt = context_ud->cdt;
    TRACE_CREATE(effect, "effect");
    make_context_current(L, old_context);
    return 1;
    }



static int GetInteger(lua_State *L, effect_t effect, ALenum param)
    {
    ALint val;
    ud_t *ud = userdata(effect);
    CheckDevicePfn(L, ud, GetEffecti);
    ud->ddt->GetEffecti(effect->name, param, &val);
    CheckErrorAl(L);
    lua_pushnumber(L, val);
    return 1;
    }

static int SetInteger(lua_State *L, effect_t effect, ALenum param)
    {
    ALint val;
    ud_t *ud = userdata(effect);
    CheckDevicePfn(L, ud, Effecti);
    val = luaL_checknumber(L, 3);
    ud->ddt->Effecti(effect->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

static int GetFloat(lua_State *L, effect_t effect, ALenum param)
    {
    ALfloat val;
    ud_t *ud = userdata(effect);
    CheckDevicePfn(L, ud, GetEffectf);
    ud->ddt->GetEffectf(effect->name, param, &val);
    CheckErrorAl(L);
    lua_pushnumber(L, val);
    return 1;
    }

static int SetFloat(lua_State *L, effect_t effect, ALenum param)
    {
    ALfloat val;
    ud_t *ud = userdata(effect);
    CheckDevicePfn(L, ud, Effectf);
    val = luaL_checknumber(L, 3);
    ud->ddt->Effectf(effect->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

static int GetFloat3(lua_State *L, effect_t effect, ALenum param)
    {
    ALfloat val[3];
    ud_t *ud = userdata(effect);
    CheckDevicePfn(L, ud, GetEffectfv);
    ud->ddt->GetEffectfv(effect->name, param, val);
    CheckErrorAl(L);
    pushfloat3(L, val);
    return 1;
    }

static int SetFloat3(lua_State *L, effect_t effect, ALenum param)
    {
    ALfloat val[3];
    ud_t *ud = userdata(effect);
    CheckDevicePfn(L, ud, Effectfv);
    checkfloat3(L, 3, val);
    ud->ddt->Effectfv(effect->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

#define GET_ENUM_FUNC(FuncName, param, enumtype)    \
static int FuncName(lua_State *L, effect_t effect)  \
    {                                               \
    ALenum val;                                     \
    ud_t *ud = userdata(effect);                    \
    CheckDevicePfn(L, ud, GetEffecti);              \
    ud->ddt->GetEffecti(effect->name, param, &val); \
    CheckErrorAl(L);                                \
    push##enumtype(L, val);                         \
    return 1;                                       \
    }

GET_ENUM_FUNC(GetChorusWaveform, AL_CHORUS_WAVEFORM, choruswaveform)
GET_ENUM_FUNC(GetRingModulatorWaveform, AL_RING_MODULATOR_WAVEFORM, ringmodulatorwaveform)
GET_ENUM_FUNC(GetFlangerWaveform, AL_FLANGER_WAVEFORM, flangerwaveform)
GET_ENUM_FUNC(GetCompressorOnoff, AL_COMPRESSOR_ONOFF, compressoronoff)
#undef GET_ENUM_FUNC



#define SET_ENUM_FUNC(FuncName, param, enumtype)        \
static int FuncName(lua_State *L, effect_t effect)      \
    {                                                   \
    ud_t *ud = userdata(effect);                        \
    ALenum val = check##enumtype(L, 3);                 \
    CheckDevicePfn(L, ud, Effecti);                     \
    ud->ddt->Effecti(effect->name, param, val);         \
    CheckErrorAl(L);                                    \
    return 0;                                           \
    }

SET_ENUM_FUNC(SetChorusWaveform, AL_CHORUS_WAVEFORM, choruswaveform)
SET_ENUM_FUNC(SetRingModulatorWaveform, AL_RING_MODULATOR_WAVEFORM, ringmodulatorwaveform)
SET_ENUM_FUNC(SetFlangerWaveform, AL_FLANGER_WAVEFORM, flangerwaveform)
SET_ENUM_FUNC(SetCompressorOnoff, AL_COMPRESSOR_ONOFF, compressoronoff)
#undef GETSET_ENUM_FUNC


static int GetEffectChorus(lua_State *L, effect_t effect)
    {
    ALenum param = checkchorusparam(L, 2);
    switch(param)
        {
        case AL_CHORUS_WAVEFORM: return GetChorusWaveform(L, effect);
        case AL_CHORUS_RATE:
        case AL_CHORUS_DEPTH:
        case AL_CHORUS_FEEDBACK:
        case AL_CHORUS_DELAY:
                            return GetFloat(L, effect, param);
        case AL_CHORUS_PHASE:
                            return GetInteger(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectChorus(lua_State *L, effect_t effect)
    {
    ALenum param = checkchorusparam(L, 2);
    switch(param)
        {
        case AL_CHORUS_WAVEFORM: return SetChorusWaveform(L, effect);
        case AL_CHORUS_RATE:
        case AL_CHORUS_DEPTH:
        case AL_CHORUS_FEEDBACK:
        case AL_CHORUS_DELAY:
                            return SetFloat(L, effect, param);
        case AL_CHORUS_PHASE:
                            return SetInteger(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetEffectReverb(lua_State *L, effect_t effect)
    {
    ALenum param = checkreverbparam(L, 2);
    switch(param)
        {
        case AL_REVERB_DENSITY:
        case AL_REVERB_DIFFUSION:
        case AL_REVERB_GAIN:
        case AL_REVERB_GAINHF:
        case AL_REVERB_DECAY_TIME:
        case AL_REVERB_DECAY_HFRATIO:
        case AL_REVERB_REFLECTIONS_GAIN:
        case AL_REVERB_REFLECTIONS_DELAY:
        case AL_REVERB_LATE_REVERB_GAIN:
        case AL_REVERB_LATE_REVERB_DELAY:
        case AL_REVERB_AIR_ABSORPTION_GAINHF:
        case AL_REVERB_ROOM_ROLLOFF_FACTOR:
                            return GetFloat(L, effect, param);
        case AL_REVERB_DECAY_HFLIMIT:
                            return GetInteger(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectReverb(lua_State *L, effect_t effect)
    {
    ALenum param = checkreverbparam(L, 2);
    switch(param)
        {
        case AL_REVERB_DENSITY:
        case AL_REVERB_DIFFUSION:
        case AL_REVERB_GAIN:
        case AL_REVERB_GAINHF:
        case AL_REVERB_DECAY_TIME:
        case AL_REVERB_DECAY_HFRATIO:
        case AL_REVERB_REFLECTIONS_GAIN:
        case AL_REVERB_REFLECTIONS_DELAY:
        case AL_REVERB_LATE_REVERB_GAIN:
        case AL_REVERB_LATE_REVERB_DELAY:
        case AL_REVERB_AIR_ABSORPTION_GAINHF:
        case AL_REVERB_ROOM_ROLLOFF_FACTOR:
                            return SetFloat(L, effect, param);
        case AL_REVERB_DECAY_HFLIMIT:
                            return SetInteger(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetEffectDistortion(lua_State *L, effect_t effect)
    {
    ALenum param = checkdistortionparam(L, 2);
    switch(param)
        {
        case AL_DISTORTION_EDGE:
        case AL_DISTORTION_GAIN:
        case AL_DISTORTION_LOWPASS_CUTOFF:
        case AL_DISTORTION_EQCENTER:
        case AL_DISTORTION_EQBANDWIDTH:
                            return GetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectDistortion(lua_State *L, effect_t effect)
    {
    ALenum param = checkdistortionparam(L, 2);
    switch(param)
        {
        case AL_DISTORTION_EDGE:
        case AL_DISTORTION_GAIN:
        case AL_DISTORTION_LOWPASS_CUTOFF:
        case AL_DISTORTION_EQCENTER:
        case AL_DISTORTION_EQBANDWIDTH:
                            return SetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int GetEffectEcho(lua_State *L, effect_t effect)
    {
    ALenum param = checkechoparam(L, 2);
    switch(param)
        {
        case AL_ECHO_DELAY:
        case AL_ECHO_LRDELAY:
        case AL_ECHO_DAMPING:
        case AL_ECHO_FEEDBACK:
        case AL_ECHO_SPREAD:
                            return GetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectEcho(lua_State *L, effect_t effect)
    {
    ALenum param = checkechoparam(L, 2);
    switch(param)
        {
        case AL_ECHO_DELAY:
        case AL_ECHO_LRDELAY:
        case AL_ECHO_DAMPING:
        case AL_ECHO_FEEDBACK:
        case AL_ECHO_SPREAD:
                            return SetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetEffectFlanger(lua_State *L, effect_t effect)
    {
    ALenum param = checkflangerparam(L, 2);
    switch(param)
        {
        case AL_FLANGER_WAVEFORM: return GetFlangerWaveform(L, effect);
        case AL_FLANGER_RATE:
        case AL_FLANGER_DEPTH:
        case AL_FLANGER_FEEDBACK:
        case AL_FLANGER_DELAY:
                            return GetFloat(L, effect, param);
        case AL_FLANGER_PHASE:
                            return GetInteger(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectFlanger(lua_State *L, effect_t effect)
    {
    ALenum param = checkflangerparam(L, 2);
    switch(param)
        {
        case AL_FLANGER_WAVEFORM: return SetFlangerWaveform(L, effect);
        case AL_FLANGER_RATE:
        case AL_FLANGER_DEPTH:
        case AL_FLANGER_FEEDBACK:
        case AL_FLANGER_DELAY:
                            return SetFloat(L, effect, param);
        case AL_FLANGER_PHASE:
                            return SetInteger(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetEffectRingModulator(lua_State *L, effect_t effect)
    {
    ALenum param = checkringmodulatorparam(L, 2);
    switch(param)
        {
        case AL_RING_MODULATOR_WAVEFORM: return GetRingModulatorWaveform(L, effect);
        case AL_RING_MODULATOR_FREQUENCY:
        case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
                            return GetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectRingModulator(lua_State *L, effect_t effect)
    {
    ALenum param = checkringmodulatorparam(L, 2);
    switch(param)
        {
        case AL_RING_MODULATOR_WAVEFORM: return SetRingModulatorWaveform(L, effect);
        case AL_RING_MODULATOR_FREQUENCY:
        case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
                            return SetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetEffectCompressor(lua_State *L, effect_t effect)
    {
    ALenum param = checkcompressorparam(L, 2);
    switch(param)
        {
        case AL_COMPRESSOR_ONOFF: return GetCompressorOnoff(L, effect);
                            return GetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectCompressor(lua_State *L, effect_t effect)
    {
    ALenum param = checkcompressorparam(L, 2);
    switch(param)
        {
        case AL_COMPRESSOR_ONOFF: return SetCompressorOnoff(L, effect);
                            return SetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetEffectEqualizer(lua_State *L, effect_t effect)
    {
    ALenum param = checkequalizerparam(L, 2);
    switch(param)
        {
        case AL_EQUALIZER_LOW_GAIN:
        case AL_EQUALIZER_LOW_CUTOFF:
        case AL_EQUALIZER_MID1_GAIN:
        case AL_EQUALIZER_MID1_CENTER:
        case AL_EQUALIZER_MID1_WIDTH:
        case AL_EQUALIZER_MID2_GAIN:
        case AL_EQUALIZER_MID2_CENTER:
        case AL_EQUALIZER_MID2_WIDTH:
        case AL_EQUALIZER_HIGH_GAIN:
        case AL_EQUALIZER_HIGH_CUTOFF:
                            return GetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectEqualizer(lua_State *L, effect_t effect)
    {
    ALenum param = checkequalizerparam(L, 2);
    switch(param)
        {
        case AL_EQUALIZER_LOW_GAIN:
        case AL_EQUALIZER_LOW_CUTOFF:
        case AL_EQUALIZER_MID1_GAIN:
        case AL_EQUALIZER_MID1_CENTER:
        case AL_EQUALIZER_MID1_WIDTH:
        case AL_EQUALIZER_MID2_GAIN:
        case AL_EQUALIZER_MID2_CENTER:
        case AL_EQUALIZER_MID2_WIDTH:
        case AL_EQUALIZER_HIGH_GAIN:
        case AL_EQUALIZER_HIGH_CUTOFF:
                            return SetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetEffectEaxreverb(lua_State *L, effect_t effect)
    {
    ALenum param = checkeaxreverbparam(L, 2);
    switch(param)
        {
        case AL_EAXREVERB_DENSITY:
        case AL_EAXREVERB_DIFFUSION:
        case AL_EAXREVERB_GAIN:
        case AL_EAXREVERB_GAINHF:
        case AL_EAXREVERB_GAINLF:
        case AL_EAXREVERB_DECAY_TIME:
        case AL_EAXREVERB_DECAY_HFRATIO:
        case AL_EAXREVERB_DECAY_LFRATIO:
        case AL_EAXREVERB_DECAY_HFLIMIT:
        case AL_EAXREVERB_REFLECTIONS_GAIN:
        case AL_EAXREVERB_REFLECTIONS_DELAY:
        case AL_EAXREVERB_LATE_REVERB_GAIN:
        case AL_EAXREVERB_LATE_REVERB_DELAY:
        case AL_EAXREVERB_AIR_ABSORPTION_GAINHF:
        case AL_EAXREVERB_ECHO_TIME:
        case AL_EAXREVERB_ECHO_DEPTH:
        case AL_EAXREVERB_MODULATION_TIME:
        case AL_EAXREVERB_MODULATION_DEPTH:
        case AL_EAXREVERB_HFREFERENCE:
        case AL_EAXREVERB_LFREFERENCE:
        case AL_EAXREVERB_ROOM_ROLLOFF_FACTOR:
        case AL_EAXREVERB_LATE_REVERB_PAN:
                            return GetFloat(L, effect, param);
        case AL_EAXREVERB_REFLECTIONS_PAN:
                            return GetFloat3(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectEaxreverb(lua_State *L, effect_t effect)
    {
    ALenum param = checkeaxreverbparam(L, 2);
    switch(param)
        {
        case AL_EAXREVERB_DENSITY:
        case AL_EAXREVERB_DIFFUSION:
        case AL_EAXREVERB_GAIN:
        case AL_EAXREVERB_GAINHF:
        case AL_EAXREVERB_GAINLF:
        case AL_EAXREVERB_DECAY_TIME:
        case AL_EAXREVERB_DECAY_HFRATIO:
        case AL_EAXREVERB_DECAY_LFRATIO:
        case AL_EAXREVERB_DECAY_HFLIMIT:
        case AL_EAXREVERB_REFLECTIONS_GAIN:
        case AL_EAXREVERB_REFLECTIONS_DELAY:
        case AL_EAXREVERB_LATE_REVERB_GAIN:
        case AL_EAXREVERB_LATE_REVERB_DELAY:
        case AL_EAXREVERB_AIR_ABSORPTION_GAINHF:
        case AL_EAXREVERB_ECHO_TIME:
        case AL_EAXREVERB_ECHO_DEPTH:
        case AL_EAXREVERB_MODULATION_TIME:
        case AL_EAXREVERB_MODULATION_DEPTH:
        case AL_EAXREVERB_HFREFERENCE:
        case AL_EAXREVERB_LFREFERENCE:
        case AL_EAXREVERB_ROOM_ROLLOFF_FACTOR:
        case AL_EAXREVERB_LATE_REVERB_PAN:
                            return SetFloat(L, effect, param);
        case AL_EAXREVERB_REFLECTIONS_PAN:
                            return SetFloat3(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetEffectDedicated(lua_State *L, effect_t effect)
    {
    ALenum param = checkdedicatedparam(L, 2);
    switch(param)
        {
        case AL_DEDICATED_GAIN:
                            return GetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetEffectDedicated(lua_State *L, effect_t effect)
    {
    ALenum param = checkdedicatedparam(L, 2);
    switch(param)
        {
        case AL_DEDICATED_GAIN:
                            return SetFloat(L, effect, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static ALenum EffectType(lua_State *L, effect_t effect)
    {
    ALenum val;
    ud_t *ud = userdata(effect);
    CheckDevicePfn(L, ud, GetEffecti);
    ud->ddt->GetEffecti(effect->name, AL_EFFECT_TYPE, &val);
    CheckErrorAl(L);
    return val;
    }


static int GetEffectType(lua_State *L)
    {
    ALenum val;
    ud_t *ud;
    effect_t effect = checkeffect(L, 1, &ud);
    CheckDevicePfn(L, ud, GetEffecti);
    ud->ddt->GetEffecti(effect->name, AL_EFFECT_TYPE, &val);
    CheckErrorAl(L);
    pusheffecttype(L, val);
    return 1;
    }

static int SetEffectType(lua_State *L)
    {
    ud_t *ud;
    effect_t effect = checkeffect(L, 1, &ud);
    ALenum val = checkeffecttype(L, 2);
    CheckDevicePfn(L, ud, Effecti);
    ud->ddt->Effecti(effect->name, AL_EFFECT_TYPE, val);
    CheckErrorAl(L);
    return 0;
    }


static int GetEffect(lua_State *L)
    {
    effect_t effect = checkeffect(L, 1, NULL);
    ALenum effect_type = EffectType(L, effect);

    switch(effect_type)
        {
        case AL_EFFECT_REVERB:  return GetEffectReverb(L, effect);
        case AL_EFFECT_CHORUS:  return GetEffectChorus(L, effect);
        case AL_EFFECT_DISTORTION:  return GetEffectDistortion(L, effect);
        case AL_EFFECT_ECHO:    return GetEffectEcho(L, effect);
        case AL_EFFECT_FLANGER: return GetEffectFlanger(L, effect);
//      case AL_EFFECT_FREQUENCY_SHIFTER:   return GetEffectFrequencyShifter(L, effect);
//      case AL_EFFECT_VOCAL_MORPHER:   return GetEffectVocalMorpher(L, effect);
//      case AL_EFFECT_PITCH_SHIFTER:   return GetEffectPitchShifter(L, effect);
        case AL_EFFECT_RING_MODULATOR:  return GetEffectRingModulator(L, effect);
//      case AL_EFFECT_AUTOWAH: return GetEffectAutowah(L, effect);
        case AL_EFFECT_COMPRESSOR:  return GetEffectCompressor(L, effect);
        case AL_EFFECT_EQUALIZER:   return GetEffectEqualizer(L, effect);
        case AL_EFFECT_EAXREVERB:   return GetEffectEaxreverb(L, effect);
        case AL_EFFECT_DEDICATED_DIALOGUE:
        case AL_EFFECT_DEDICATED_LOW_FREQUENCY_EFFECT:  return GetEffectDedicated(L, effect);
        case AL_EFFECT_NULL:
        default:
            return luaL_argerror(L, 1, "undefined effect type");
        }
    return 0;
    }

static int SetEffect(lua_State *L)
    {
    effect_t effect = checkeffect(L, 1, NULL);
    ALenum effect_type = EffectType(L, effect);

    switch(effect_type)
        {
        case AL_EFFECT_REVERB:  return SetEffectReverb(L, effect);
        case AL_EFFECT_CHORUS:  return SetEffectChorus(L, effect);
        case AL_EFFECT_DISTORTION:  return SetEffectDistortion(L, effect);
        case AL_EFFECT_ECHO:    return SetEffectEcho(L, effect);
        case AL_EFFECT_FLANGER: return SetEffectFlanger(L, effect);
//      case AL_EFFECT_FREQUENCY_SHIFTER:   return SetEffectFrequencyShifter(L, effect);
//      case AL_EFFECT_VOCAL_MORPHER:   return SetEffectVocalMorpher(L, effect);
//      case AL_EFFECT_PITCH_SHIFTER:   return SetEffectPitchShifter(L, effect);
        case AL_EFFECT_RING_MODULATOR:  return SetEffectRingModulator(L, effect);
//      case AL_EFFECT_AUTOWAH: return SetEffectAutowah(L, effect);
        case AL_EFFECT_COMPRESSOR:  return SetEffectCompressor(L, effect);
        case AL_EFFECT_EQUALIZER:   return SetEffectEqualizer(L, effect);
        case AL_EFFECT_EAXREVERB:   return SetEffectEaxreverb(L, effect);
        case AL_EFFECT_DEDICATED_DIALOGUE:
        case AL_EFFECT_DEDICATED_LOW_FREQUENCY_EFFECT:  return SetEffectDedicated(L, effect);
        case AL_EFFECT_NULL:
        default:
            return luaL_argerror(L, 1, "undefined effect type");
        }

    return 0;
    }



#if 0

The following effects are not (yet?) supported by OpenAL-SOFT (1.18.1):
AL_EFFECT_FREQUENCY_SHIFTER
AL_EFFECT_VOCAL_MORPHER
AL_EFFECT_PITCH_SHIFTER
AL_EFFECT_AUTOWAH

/* Effect properties. */
/* Frequency shifter effect parameters */
#define AL_FREQUENCY_SHIFTER_FREQUENCY           0x0001
#define AL_FREQUENCY_SHIFTER_LEFT_DIRECTION      0x0002
#define AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION     0x0003

/* Vocal morpher effect parameters */
#define AL_VOCAL_MORPHER_PHONEMEA                0x0001
#define AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING  0x0002
#define AL_VOCAL_MORPHER_PHONEMEB                0x0003
#define AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING  0x0004
#define AL_VOCAL_MORPHER_WAVEFORM                0x0005
#define AL_VOCAL_MORPHER_RATE                    0x0006

/* Pitchshifter effect parameters */
#define AL_PITCH_SHIFTER_COARSE_TUNE             0x0001
#define AL_PITCH_SHIFTER_FINE_TUNE               0x0002

/* Autowah effect parameters */
#define AL_AUTOWAH_ATTACK_TIME                   0x0001
#define AL_AUTOWAH_RELEASE_TIME                  0x0002
#define AL_AUTOWAH_RESONANCE                     0x0003
#define AL_AUTOWAH_PEAK_GAIN                     0x0004

#endif


RAW_FUNC(effect)
TYPE_FUNC(effect)
PARENT_FUNC(effect)
DELETE_FUNC(effect)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "delete", Delete },
        { "get", GetEffect },
        { "set", SetEffect },
        { "get_type", GetEffectType },
        { "set_type", SetEffectType },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_effect", Create},
        { "delete_effect", Delete },
        { "effect_get", GetEffect },
        { "effect_set", SetEffect },
        { "get_effect_type", GetEffectType },
        { "set_effect_type", SetEffectType },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_effect(lua_State *L)
    {
    udata_define(L, EFFECT_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

