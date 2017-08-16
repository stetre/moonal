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

#ifndef enumsDEFINED
#define enumsDEFINED

/* enums.c */
#define enums_free_all moonal_enums_free_all
void enums_free_all(lua_State *L);
#define enums_test moonal_enums_test
uint32_t enums_test(lua_State *L, uint32_t domain, int arg, int *err);
#define enums_check moonal_enums_check
uint32_t enums_check(lua_State *L, uint32_t domain, int arg);
#define enums_push moonal_enums_push
int enums_push(lua_State *L, uint32_t domain, uint32_t code);
#define enums_values moonal_enums_values
int enums_values(lua_State *L, uint32_t domain);
#define enums_checklist moonal_enums_checklist
uint32_t* enums_checklist(lua_State *L, uint32_t domain, int arg, uint32_t *count, int *err);
#define enums_freelist moonal_enums_freelist
void enums_freelist(lua_State *L, uint32_t *list);


/* Enum domains */
#define DOMAIN_RESULT                       0
#define DOMAIN_AL_PARAM                     1
#define DOMAIN_ALC_PARAM                    2
#define DOMAIN_ALC_CHANNELS_SOFT            3
#define DOMAIN_ALC_TYPE_SOFT                4
#define DOMAIN_AL_CAPABILITY                5
#define DOMAIN_AL_FORMAT                    6
//#define DOMAIN_AL_INTERNAL_FORMAT             7
#define DOMAIN_AL_DISTANCE_MODEL            8
#define DOMAIN_AL_RESAMPLER                 9
#define DOMAIN_AL_SPATIALIZE_MODE           10
#define DOMAIN_AL_SOURCE_TYPE               11
#define DOMAIN_AL_SOURCE_STATE              12
#define DOMAIN_AL_EFFECT_TYPE               13
#define DOMAIN_AL_CHORUS_WAVEFORM           14
#define DOMAIN_AL_FLANGER_WAVEFORM          15
#define DOMAIN_AL_RING_MODULATOR_WAVEFORM   16
#define DOMAIN_AL_COMPRESSOR_ONOFF          17
#define DOMAIN_AL_FILTER_TYPE               18
#define DOMAIN_ALC_HRTF_STATUS              19
#define DOMAIN_AL_CHORUS_PARAM              30
#define DOMAIN_AL_REVERB_PARAM              31
#define DOMAIN_AL_DISTORTION_PARAM          32
#define DOMAIN_AL_ECHO_PARAM                33
#define DOMAIN_AL_FLANGER_PARAM             34
#define DOMAIN_AL_RING_MODULATOR_PARAM      35
#define DOMAIN_AL_COMPRESSOR_PARAM          36
#define DOMAIN_AL_EQUALIZER_PARAM           37
#define DOMAIN_AL_EAXREVERB_PARAM           38
#define DOMAIN_AL_DEDICATED_PARAM           39
#define DOMAIN_AL_LOWPASS_PARAM             51
#define DOMAIN_AL_HIGHPASS_PARAM            52
#define DOMAIN_AL_BANDPASS_PARAM            53
#define DOMAIN_AL_EFFECTSLOT_PARAM          71
/* NONAL additions */
#define DOMAIN_NONAL_TYPE                  101

/* Types for al.sizeof() & friends */
#define NONAL_TYPE_CHAR         1
#define NONAL_TYPE_UCHAR        2
#define NONAL_TYPE_BYTE         3
#define NONAL_TYPE_UBYTE        4
#define NONAL_TYPE_SHORT        5
#define NONAL_TYPE_USHORT       6
#define NONAL_TYPE_INT          7
#define NONAL_TYPE_UINT         8
#define NONAL_TYPE_LONG         9
#define NONAL_TYPE_ULONG        10
#define NONAL_TYPE_FLOAT        11
#define NONAL_TYPE_DOUBLE       12


#define testtype(L, arg, err) (ALenum)enums_test((L), DOMAIN_NONAL_TYPE, (arg), (err))
#define checktype(L, arg) (ALenum)enums_check((L), DOMAIN_NONAL_TYPE, (arg))
#define pushtype(L, val) enums_push((L), DOMAIN_NONAL_TYPE, (uint32_t)(val))
#define valuestype(L) enums_values((L), DOMAIN_NONAL_TYPE)

#define testchannels(L, arg, err) (ALCenum)enums_test((L), DOMAIN_ALC_CHANNELS_SOFT, (arg), (err))
#define checkchannels(L, arg) (ALCenum)enums_check((L), DOMAIN_ALC_CHANNELS_SOFT, (arg))
#define pushchannels(L, val) enums_push((L), DOMAIN_ALC_CHANNELS_SOFT, (uint32_t)(val))
#define valueschannels(L) enums_values((L), DOMAIN_ALC_CHANNELS_SOFT)

#define testtypesoft(L, arg, err) (ALCenum)enums_test((L), DOMAIN_ALC_TYPE_SOFT, (arg), (err))
#define checktypesoft(L, arg) (ALCenum)enums_check((L), DOMAIN_ALC_TYPE_SOFT, (arg))
#define pushtypesoft(L, val) enums_push((L), DOMAIN_ALC_TYPE_SOFT, (uint32_t)(val))
#define valuestypesoft(L) enums_values((L), DOMAIN_ALC_TYPE_SOFT)

#define testcapability(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_CAPABILITY, (arg), (err))
#define checkcapability(L, arg) (ALenum)enums_check((L), DOMAIN_AL_CAPABILITY, (arg))
#define pushcapability(L, val) enums_push((L), DOMAIN_AL_CAPABILITY, (uint32_t)(val))
#define valuescapability(L) enums_values((L), DOMAIN_AL_CAPABILITY)

#define testformat(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_FORMAT, (arg), (err))
#define checkformat(L, arg) (ALenum)enums_check((L), DOMAIN_AL_FORMAT, (arg))
#define pushformat(L, val) enums_push((L), DOMAIN_AL_FORMAT, (uint32_t)(val))
#define valuesformat(L) enums_values((L), DOMAIN_AL_FORMAT)

#if 0
#define testinternalformat(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_INTERNAL_FORMAT, (arg), (err))
#define checkinternalformat(L, arg) (ALenum)enums_check((L), DOMAIN_AL_INTERNAL_FORMAT, (arg))
#define pushinternalformat(L, val) enums_push((L), DOMAIN_AL_INTERNAL_FORMAT, (uint32_t)(val))
#define valuesinternalformat(L) enums_values((L), DOMAIN_AL_INTERNAL_FORMAT)
#endif

#define testdistancemodel(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_DISTANCE_MODEL, (arg), (err))
#define checkdistancemodel(L, arg) (ALenum)enums_check((L), DOMAIN_AL_DISTANCE_MODEL, (arg))
#define pushdistancemodel(L, val) enums_push((L), DOMAIN_AL_DISTANCE_MODEL, (uint32_t)(val))
#define valuesdistancemodel(L) enums_values((L), DOMAIN_AL_DISTANCE_MODEL)

#define testresampler(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_RESAMPLER, (arg), (err))
#define checkresampler(L, arg) (ALenum)enums_check((L), DOMAIN_AL_RESAMPLER, (arg))
#define pushresampler(L, val) enums_push((L), DOMAIN_AL_RESAMPLER, (uint32_t)(val))
#define valuesresampler(L) enums_values((L), DOMAIN_AL_RESAMPLER)

#define testspatializemode(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_SPATIALIZE_MODE, (arg), (err))
#define checkspatializemode(L, arg) (ALenum)enums_check((L), DOMAIN_AL_SPATIALIZE_MODE, (arg))
#define pushspatializemode(L, val) enums_push((L), DOMAIN_AL_SPATIALIZE_MODE, (uint32_t)(val))
#define valuesspatializemode(L) enums_values((L), DOMAIN_AL_SPATIALIZE_MODE)

#define testsourcetype(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_SOURCE_TYPE, (arg), (err))
#define checksourcetype(L, arg) (ALenum)enums_check((L), DOMAIN_AL_SOURCE_TYPE, (arg))
#define pushsourcetype(L, val) enums_push((L), DOMAIN_AL_SOURCE_TYPE, (uint32_t)(val))
#define valuessourcetype(L) enums_values((L), DOMAIN_AL_SOURCE_TYPE)

#define testsourcestate(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_SOURCE_STATE, (arg), (err))
#define checksourcestate(L, arg) (ALenum)enums_check((L), DOMAIN_AL_SOURCE_STATE, (arg))
#define pushsourcestate(L, val) enums_push((L), DOMAIN_AL_SOURCE_STATE, (uint32_t)(val))
#define valuessourcestate(L) enums_values((L), DOMAIN_AL_SOURCE_STATE)

#define testeffecttype(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_EFFECT_TYPE, (arg), (err))
#define checkeffecttype(L, arg) (ALenum)enums_check((L), DOMAIN_AL_EFFECT_TYPE, (arg))
#define pusheffecttype(L, val) enums_push((L), DOMAIN_AL_EFFECT_TYPE, (uint32_t)(val))
#define valueseffecttype(L) enums_values((L), DOMAIN_AL_EFFECT_TYPE)

#define testchoruswaveform(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_CHORUS_WAVEFORM, (arg), (err))
#define checkchoruswaveform(L, arg) (ALenum)enums_check((L), DOMAIN_AL_CHORUS_WAVEFORM, (arg))
#define pushchoruswaveform(L, val) enums_push((L), DOMAIN_AL_CHORUS_WAVEFORM, (uint32_t)(val))
#define valueschoruswaveform(L) enums_values((L), DOMAIN_AL_CHORUS_WAVEFORM)

#define testflangerwaveform(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_FLANGER_WAVEFORM, (arg), (err))
#define checkflangerwaveform(L, arg) (ALenum)enums_check((L), DOMAIN_AL_FLANGER_WAVEFORM, (arg))
#define pushflangerwaveform(L, val) enums_push((L), DOMAIN_AL_FLANGER_WAVEFORM, (uint32_t)(val))
#define valuesflangerwaveform(L) enums_values((L), DOMAIN_AL_FLANGER_WAVEFORM)

#define testringmodulatorwaveform(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_RING_MODULATOR_WAVEFORM, (arg), (err))
#define checkringmodulatorwaveform(L, arg) (ALenum)enums_check((L), DOMAIN_AL_RING_MODULATOR_WAVEFORM, (arg))
#define pushringmodulatorwaveform(L, val) enums_push((L), DOMAIN_AL_RING_MODULATOR_WAVEFORM, (uint32_t)(val))
#define valuesringmodulatorwaveform(L) enums_values((L), DOMAIN_AL_RING_MODULATOR_WAVEFORM)

#define testcompressoronoff(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_COMPRESSOR_ONOFF, (arg), (err))
#define checkcompressoronoff(L, arg) (ALenum)enums_check((L), DOMAIN_AL_COMPRESSOR_ONOFF, (arg))
#define pushcompressoronoff(L, val) enums_push((L), DOMAIN_AL_COMPRESSOR_ONOFF, (uint32_t)(val))
#define valuescompressoronoff(L) enums_values((L), DOMAIN_AL_COMPRESSOR_ONOFF)

#define testfiltertype(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_FILTER_TYPE, (arg), (err))
#define checkfiltertype(L, arg) (ALenum)enums_check((L), DOMAIN_AL_FILTER_TYPE, (arg))
#define pushfiltertype(L, val) enums_push((L), DOMAIN_AL_FILTER_TYPE, (uint32_t)(val))
#define valuesfiltertype(L) enums_values((L), DOMAIN_AL_FILTER_TYPE)

#define testalparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_PARAM, (arg), (err))
#define checkalparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_PARAM, (arg))
#define pushalparam(L, val) enums_push((L), DOMAIN_AL_PARAM, (uint32_t)(val))
#define valuesalparam(L) enums_values((L), DOMAIN_AL_PARAM)

#define testalcparam(L, arg, err) (ALCenum)enums_test((L), DOMAIN_ALC_PARAM, (arg), (err))
#define checkalcparam(L, arg) (ALCenum)enums_check((L), DOMAIN_ALC_PARAM, (arg))
#define pushalcparam(L, val) enums_push((L), DOMAIN_ALC_PARAM, (uint32_t)(val))
#define valuesalcparam(L) enums_values((L), DOMAIN_ALC_PARAM)

#define testchorusparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_CHORUS_PARAM, (arg), (err))
#define checkchorusparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_CHORUS_PARAM, (arg))
#define pushchorusparam(L, val) enums_push((L), DOMAIN_AL_CHORUS_PARAM, (uint32_t)(val))
#define valueschorusparam(L) enums_values((L), DOMAIN_AL_CHORUS_PARAM)

#define testreverbparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_REVERB_PARAM, (arg), (err))
#define checkreverbparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_REVERB_PARAM, (arg))
#define pushreverbparam(L, val) enums_push((L), DOMAIN_AL_REVERB_PARAM, (uint32_t)(val))
#define valuesreverbparam(L) enums_values((L), DOMAIN_AL_REVERB_PARAM)

#define testdistortionparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_DISTORTION_PARAM, (arg), (err))
#define checkdistortionparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_DISTORTION_PARAM, (arg))
#define pushdistortionparam(L, val) enums_push((L), DOMAIN_AL_DISTORTION_PARAM, (uint32_t)(val))
#define valuesdistortionparam(L) enums_values((L), DOMAIN_AL_DISTORTION_PARAM)

#define testechoparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_ECHO_PARAM, (arg), (err))
#define checkechoparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_ECHO_PARAM, (arg))
#define pushechoparam(L, val) enums_push((L), DOMAIN_AL_ECHO_PARAM, (uint32_t)(val))
#define valuesechoparam(L) enums_values((L), DOMAIN_AL_ECHO_PARAM)

#define testflangerparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_FLANGER_PARAM, (arg), (err))
#define checkflangerparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_FLANGER_PARAM, (arg))
#define pushflangerparam(L, val) enums_push((L), DOMAIN_AL_FLANGER_PARAM, (uint32_t)(val))
#define valuesflangerparam(L) enums_values((L), DOMAIN_AL_FLANGER_PARAM)

#define testringmodulatorparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_RING_MODULATOR_PARAM, (arg), (err))
#define checkringmodulatorparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_RING_MODULATOR_PARAM, (arg))
#define pushringmodulatorparam(L, val) enums_push((L), DOMAIN_AL_RING_MODULATOR_PARAM, (uint32_t)(val))
#define valuesringmodulatorparam(L) enums_values((L), DOMAIN_AL_RING_MODULATOR_PARAM)

#define testcompressorparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_COMPRESSOR_PARAM, (arg), (err))
#define checkcompressorparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_COMPRESSOR_PARAM, (arg))
#define pushcompressorparam(L, val) enums_push((L), DOMAIN_AL_COMPRESSOR_PARAM, (uint32_t)(val))
#define valuescompressorparam(L) enums_values((L), DOMAIN_AL_COMPRESSOR_PARAM)

#define testequalizerparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_EQUALIZER_PARAM, (arg), (err))
#define checkequalizerparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_EQUALIZER_PARAM, (arg))
#define pushequalizerparam(L, val) enums_push((L), DOMAIN_AL_EQUALIZER_PARAM, (uint32_t)(val))
#define valuesequalizerparam(L) enums_values((L), DOMAIN_AL_EQUALIZER_PARAM)

#define testeaxreverbparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_EAXREVERB_PARAM, (arg), (err))
#define checkeaxreverbparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_EAXREVERB_PARAM, (arg))
#define pusheaxreverbparam(L, val) enums_push((L), DOMAIN_AL_EAXREVERB_PARAM, (uint32_t)(val))
#define valueseaxreverbparam(L) enums_values((L), DOMAIN_AL_EAXREVERB_PARAM)

#define testdedicatedparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_DEDICATED_PARAM, (arg), (err))
#define checkdedicatedparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_DEDICATED_PARAM, (arg))
#define pushdedicatedparam(L, val) enums_push((L), DOMAIN_AL_DEDICATED_PARAM, (uint32_t)(val))
#define valuesdedicatedparam(L) enums_values((L), DOMAIN_AL_DEDICATED_PARAM)

#define testlowpassparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_LOWPASS_PARAM, (arg), (err))
#define checklowpassparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_LOWPASS_PARAM, (arg))
#define pushlowpassparam(L, val) enums_push((L), DOMAIN_AL_LOWPASS_PARAM, (uint32_t)(val))
#define valueslowpassparam(L) enums_values((L), DOMAIN_AL_LOWPASS_PARAM)

#define testhighpassparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_HIGHPASS_PARAM, (arg), (err))
#define checkhighpassparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_HIGHPASS_PARAM, (arg))
#define pushhighpassparam(L, val) enums_push((L), DOMAIN_AL_HIGHPASS_PARAM, (uint32_t)(val))
#define valueshighpassparam(L) enums_values((L), DOMAIN_AL_HIGHPASS_PARAM)

#define testbandpassparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_BANDPASS_PARAM, (arg), (err))
#define checkbandpassparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_BANDPASS_PARAM, (arg))
#define pushbandpassparam(L, val) enums_push((L), DOMAIN_AL_BANDPASS_PARAM, (uint32_t)(val))
#define valuesbandpassparam(L) enums_values((L), DOMAIN_AL_BANDPASS_PARAM)

#define testeffectslotparam(L, arg, err) (ALenum)enums_test((L), DOMAIN_AL_EFFECTSLOT_PARAM, (arg), (err))
#define checkeffectslotparam(L, arg) (ALenum)enums_check((L), DOMAIN_AL_EFFECTSLOT_PARAM, (arg))
#define pusheffectslotparam(L, val) enums_push((L), DOMAIN_AL_EFFECTSLOT_PARAM, (uint32_t)(val))
#define valueseffectslotparam(L) enums_values((L), DOMAIN_AL_EFFECTSLOT_PARAM)

#define testhrtfstatus(L, arg, err) (ALenum)enums_test((L), DOMAIN_ALC_HRTF_STATUS, (arg), (err))
#define checkhrtfstatus(L, arg) (ALenum)enums_check((L), DOMAIN_ALC_HRTF_STATUS, (arg))
#define pushhrtfstatus(L, val) enums_push((L), DOMAIN_ALC_HRTF_STATUS, (uint32_t)(val))
#define valueshrtfstatus(L) enums_values((L), DOMAIN_ALC_HRTF_STATUS)

#if 0 /* scaffolding 6yy */
#define testxxx(L, arg, err) (Xxx)enums_test((L), DOMAIN_XXX, (arg), (err))
#define checkxxx(L, arg) (Xxx)enums_check((L), DOMAIN_XXX, (arg))
#define pushxxx(L, val) enums_push((L), DOMAIN_XXX, (uint32_t)(val))
#define valuesxxx(L) enums_values((L), DOMAIN_XXX)
    CASE(xxx);

#endif

#endif /* enumsDEFINED */


