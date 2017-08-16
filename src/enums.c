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

/*------------------------------------------------------------------------------*
 | Code<->string map for enumerations                                           |
 *------------------------------------------------------------------------------*/


/* code <-> string record */
#define rec_t struct rec_s
struct rec_s {
    RB_ENTRY(rec_s) CodeEntry;
    RB_ENTRY(rec_s) StringEntry;
    uint32_t domain;
    uint32_t code;  /* (domain, code) = search key in code tree */
    char     *str;  /* (domain, str) = search key in string tree */
};

/* compare functions */
static int cmp_code(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->domain != rec2->domain)
        return (rec1->domain < rec2->domain ? -1 : rec1->domain > rec2->domain);
    return (rec1->code < rec2->code ? -1 : rec1->code > rec2->code);
    } 

static int cmp_str(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->domain != rec2->domain)
        return (rec1->domain < rec2->domain ? -1 : rec1->domain > rec2->domain);
    return strcmp(rec1->str, rec2->str);
    } 

static RB_HEAD(CodeTree, rec_s) CodeHead = RB_INITIALIZER(&CodeHead);
RB_PROTOTYPE_STATIC(CodeTree, rec_s, CodeEntry, cmp_code) 
RB_GENERATE_STATIC(CodeTree, rec_s, CodeEntry, cmp_code) 

static RB_HEAD(StringTree, rec_s) StringHead = RB_INITIALIZER(&StringHead);
RB_PROTOTYPE_STATIC(StringTree, rec_s, StringEntry, cmp_str) 
RB_GENERATE_STATIC(StringTree, rec_s, StringEntry, cmp_str) 
 
static rec_t *code_remove(rec_t *rec) 
    { return RB_REMOVE(CodeTree, &CodeHead, rec); }
static rec_t *code_insert(rec_t *rec) 
    { return RB_INSERT(CodeTree, &CodeHead, rec); }
static rec_t *code_search(uint32_t domain, uint32_t code) 
    { rec_t tmp; tmp.domain = domain; tmp.code = code; return RB_FIND(CodeTree, &CodeHead, &tmp); }
static rec_t *code_first(uint32_t domain, uint32_t code) 
    { rec_t tmp; tmp.domain = domain; tmp.code = code; return RB_NFIND(CodeTree, &CodeHead, &tmp); }
static rec_t *code_next(rec_t *rec)
    { return RB_NEXT(CodeTree, &CodeHead, rec); }
#if 0
static rec_t *code_prev(rec_t *rec)
    { return RB_PREV(CodeTree, &CodeHead, rec); }
static rec_t *code_min(void)
    { return RB_MIN(CodeTree, &CodeHead); }
static rec_t *code_max(void)
    { return RB_MAX(CodeTree, &CodeHead); }
static rec_t *code_root(void)
    { return RB_ROOT(&CodeHead); }
#endif
 
static rec_t *str_remove(rec_t *rec) 
    { return RB_REMOVE(StringTree, &StringHead, rec); }
static rec_t *str_insert(rec_t *rec) 
    { return RB_INSERT(StringTree, &StringHead, rec); }
static rec_t *str_search(uint32_t domain, const char* str) 
    { rec_t tmp; tmp.domain = domain; tmp.str = (char*)str; return RB_FIND(StringTree, &StringHead, &tmp); }
#if 0
static rec_t *str_first(uint32_t domain, const char* str ) 
    { rec_t tmp; tmp.domain = domain; tmp.str = str; return RB_NFIND(StringTree, &StringHead, &tmp); }
static rec_t *str_next(rec_t *rec)
    { return RB_NEXT(StringTree, &StringHead, rec); }
static rec_t *str_prev(rec_t *rec)
    { return RB_PREV(StringTree, &StringHead, rec); }
static rec_t *str_min(void)
    { return RB_MIN(StringTree, &StringHead); }
static rec_t *str_max(void)
    { return RB_MAX(StringTree, &StringHead); }
static rec_t *str_root(void)
    { return RB_ROOT(&StringHead); }
#endif


static int enums_new(lua_State *L, uint32_t domain, uint32_t code, const char *str)
    {
    rec_t *rec;
    if((rec = (rec_t*)Malloc(L, sizeof(rec_t))) == NULL) 
        return luaL_error(L, errstring(ERR_MEMORY));

    memset(rec, 0, sizeof(rec_t));
    rec->domain = domain;
    rec->code = code;
    rec->str = Strdup(L, str);
    if(code_search(domain, code) || str_search(domain, str))
        { 
        Free(L, rec->str);
        Free(L, rec); 
        return unexpected(L); /* duplicate value */
        }
    code_insert(rec);
    str_insert(rec);
    return 0;
    }

static void enums_free(lua_State *L, rec_t* rec)
    {
    if(code_search(rec->domain, rec->code) == rec)
        code_remove(rec);
    if(str_search(rec->domain, rec->str) == rec)
        str_remove(rec);
    Free(L, rec->str);
    Free(L, rec);   
    }

void enums_free_all(lua_State *L)
    {
    rec_t *rec;
    while((rec = code_first(0, 0)))
        enums_free(L, rec);
    }

#if 0
uint32_t enums_code(uint32_t domain, const char *str, int* found)
    {
    rec_t *rec = str_search(domain, str);
    if(!rec)
        { *found = 0; return 0; }
    *found = 1; 
    return rec->code;
    }

const char* enums_string(uint32_t domain, uint32_t code)
    {
    rec_t *rec = code_search(domain, code);
    if(!rec)
        return NULL;
    return rec->str;
    }

#endif


uint32_t enums_test(lua_State *L, uint32_t domain, int arg, int *err)
    {
    rec_t *rec;
    const char *s = luaL_optstring(L, arg, NULL);

    if(!s)
        { *err = ERR_NOTPRESENT; return 0; }

    rec = str_search(domain, s);
    if(!rec)
        { *err = ERR_VALUE; return 0; }
    
    *err = ERR_SUCCESS;
    return rec->code;
    }

uint32_t enums_check(lua_State *L, uint32_t domain, int arg)
    {
    rec_t *rec;
    const char *s = luaL_checkstring(L, arg);

    rec = str_search(domain, s);
    if(!rec)
        return luaL_argerror(L, arg, badvalue(L, s));
    
    return rec->code;
    }

int enums_push(lua_State *L, uint32_t domain, uint32_t code)
    {
    rec_t *rec = code_search(domain, code);

    if(!rec)
        return unexpected(L);

    lua_pushstring(L, rec->str);
    return 1;
    }

int enums_values(lua_State *L, uint32_t domain)
    {
    int i;
    rec_t *rec;

    lua_newtable(L);
    i = 1;
    rec = code_first(domain, 0);
    while(rec)
        {
        if(rec->domain == domain)
            {
            lua_pushstring(L, rec->str);
            lua_rawseti(L, -2, i++);
            }
        rec = code_next(rec);
        }

    return 1;
    }


uint32_t* enums_checklist(lua_State *L, uint32_t domain, int arg, uint32_t *count, int *err)
    {
    uint32_t* list;
    uint32_t i;

    *count = 0;
    *err = 0;
    if(lua_isnoneornil(L, arg))
        { *err = ERR_NOTPRESENT; return NULL; }
    if(lua_type(L, arg) != LUA_TTABLE)
        { *err = ERR_TABLE; return NULL; }

    *count = luaL_len(L, arg);
    if(*count == 0)
        { *err = ERR_NOTPRESENT; return NULL; }

    list = (uint32_t*)MallocNoErr(L, sizeof(uint32_t) * (*count));
    if(!list)
        { *count = 0; *err = ERR_MEMORY; return NULL; }

    for(i=0; i<*count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        list[i] = enums_test(L, domain, -1, err);
        lua_pop(L, 1);
        if(*err)
            { Free(L, list); *count = 0; return NULL; }
        }
    return list;
    }

void enums_freelist(lua_State *L, uint32_t *list)
    {
    if(!list)
        return;
    Free(L, list);
    }

/*------------------------------------------------------------------------------*
 |                                                                              |
 *------------------------------------------------------------------------------*/

static int Enum(lua_State *L)
/* { strings } = cl.enum('type') lists all the values for a given enum type */
    { 
    const char *s = luaL_checkstring(L, 1); 
#define CASE(xxx) if(strcmp(s, ""#xxx) == 0) return values##xxx(L)
    CASE(type);
    CASE(alparam);
    CASE(alcparam);
    CASE(channels);
    CASE(typesoft);
    CASE(capability);
    CASE(format);
//  CASE(internalformat);
    CASE(distancemodel);
    CASE(resampler);
    CASE(spatializemode);
    CASE(sourcetype);
    CASE(sourcestate);
    CASE(effecttype);
    CASE(choruswaveform);
    CASE(flangerwaveform);
    CASE(ringmodulatorwaveform);
    CASE(compressoronoff);
    CASE(filtertype);
    CASE(chorusparam);
    CASE(reverbparam);
    CASE(distortionparam);
    CASE(echoparam);
    CASE(flangerparam);
    CASE(ringmodulatorparam);
    CASE(compressorparam);
    CASE(equalizerparam);
    CASE(eaxreverbparam);
    CASE(dedicatedparam);
    CASE(lowpassparam);
    CASE(highpassparam);
    CASE(bandpassparam);
    CASE(effectslotparam);
    CASE(hrtfstatus);
#undef CASE
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "enum", Enum },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_enums(lua_State *L)
    {
    uint32_t domain;

    luaL_setfuncs(L, Functions, 0);

    /* Add all the code<->string mappings and the cl.XXX constant strings */
#define ADD(what, s) do { enums_new(L, domain, NONAL_##what, s); } while(0)
    domain = DOMAIN_NONAL_TYPE; 
    ADD(TYPE_CHAR, "char");
    ADD(TYPE_UCHAR, "uchar");
    ADD(TYPE_BYTE, "byte");
    ADD(TYPE_UBYTE, "ubyte");
    ADD(TYPE_SHORT, "short");
    ADD(TYPE_USHORT, "ushort");
    ADD(TYPE_INT, "int");
    ADD(TYPE_UINT, "uint");
    ADD(TYPE_LONG, "long");
    ADD(TYPE_ULONG, "ulong");
    ADD(TYPE_FLOAT, "float");
    ADD(TYPE_DOUBLE, "double");
#undef ADD

#define ADD(what, s) do {                               \
    lua_pushstring(L, s); lua_setfield(L, -2, #what);   \
    enums_new(L, domain, AL_##what, s);                 \
} while(0)
    //ADD(, "");
#undef ADD

#define ADD_AL(what, s) do {                            \
    lua_pushstring(L, s); lua_setfield(L, -2, #what);   \
    enums_new(L, domain, AL_##what, s);                 \
} while(0)

#define ADD_ALC(what, s) do {                           \
    lua_pushstring(L, s); lua_setfield(L, -2, "ALC_"#what);   \
    enums_new(L, domain, ALC_##what, s);                \
} while(0)

    domain = DOMAIN_ALC_CHANNELS_SOFT;
    ADD_ALC(MONO_SOFT, "mono");
    ADD_ALC(STEREO_SOFT, "stereo");
    ADD_ALC(QUAD_SOFT, "quad");
    ADD_ALC(5POINT1_SOFT, "5point1");
    ADD_ALC(6POINT1_SOFT, "6point1");
    ADD_ALC(7POINT1_SOFT, "7point1");
//@@   ADD_ALC(BFORMAT3D_SOFT, "bformat3d");

//@@ rimuovere e usare checktype rimappandoli? o semplicemente documentare come 'type'
    domain = DOMAIN_ALC_TYPE_SOFT;
    ADD_ALC(BYTE_SOFT, "byte");
    ADD_ALC(UNSIGNED_BYTE_SOFT, "ubyte");
    ADD_ALC(SHORT_SOFT, "short");
    ADD_ALC(UNSIGNED_SHORT_SOFT, "ushort");
    ADD_ALC(INT_SOFT, "int");
    ADD_ALC(UNSIGNED_INT_SOFT, "uint");
    ADD_ALC(FLOAT_SOFT, "float");

    domain = DOMAIN_AL_CAPABILITY;
    ADD_AL(SOURCE_DISTANCE_MODEL, "source distance model");

    domain = DOMAIN_AL_FORMAT;
//  ADD_AL(FORMAT_IMA_ADPCM_MONO16_EXT, "ima adpcm mono16");
//  ADD_AL(FORMAT_IMA_ADPCM_STEREO16_EXT, "ima adpcm stereo16");
//  ADD_AL(FORMAT_WAVE_EXT, "wave");
//  ADD_AL(FORMAT_VORBIS_EXT, "vorbis");
    ADD_AL(FORMAT_QUAD8_LOKI, "quad8 loki");
    ADD_AL(FORMAT_QUAD16_LOKI, "quad16 loki");
    ADD_AL(FORMAT_MONO_FLOAT32, "mono float32");
    ADD_AL(FORMAT_STEREO_FLOAT32, "stereo float32");
    ADD_AL(FORMAT_MONO_DOUBLE_EXT, "mono double");
    ADD_AL(FORMAT_STEREO_DOUBLE_EXT, "stereo double");
    ADD_AL(FORMAT_MONO_MULAW_EXT, "mono mulaw");
    ADD_AL(FORMAT_STEREO_MULAW_EXT, "stereo mulaw");
    ADD_AL(FORMAT_MONO_ALAW_EXT, "mono alaw");
    ADD_AL(FORMAT_STEREO_ALAW_EXT, "stereo alaw");
    ADD_AL(FORMAT_QUAD8, "quad8");
    ADD_AL(FORMAT_QUAD16, "quad16");
    ADD_AL(FORMAT_QUAD32, "quad32");
    ADD_AL(FORMAT_REAR8, "rear8");
    ADD_AL(FORMAT_REAR16, "rear16");
    ADD_AL(FORMAT_REAR32, "rear32");
    ADD_AL(FORMAT_51CHN8, "51chn8");
    ADD_AL(FORMAT_51CHN16, "51chn16");
    ADD_AL(FORMAT_51CHN32, "51chn32");
    ADD_AL(FORMAT_61CHN8, "61chn8");
    ADD_AL(FORMAT_61CHN16, "61chn16");
    ADD_AL(FORMAT_61CHN32, "61chn32");
    ADD_AL(FORMAT_71CHN8, "71chn8");
    ADD_AL(FORMAT_71CHN16, "71chn16");
    ADD_AL(FORMAT_71CHN32, "71chn32");
    //ADD_AL(FORMAT_MONO_MULAW, "mono mulaw");
    //ADD_AL(FORMAT_STEREO_MULAW, "stereo mulaw");
    ADD_AL(FORMAT_QUAD_MULAW, "quad mulaw");
    ADD_AL(FORMAT_REAR_MULAW, "rear mulaw");
    ADD_AL(FORMAT_51CHN_MULAW, "51chn mulaw");
    ADD_AL(FORMAT_61CHN_MULAW, "61chn mulaw");
    ADD_AL(FORMAT_71CHN_MULAW, "71chn mulaw");
    ADD_AL(FORMAT_MONO_IMA4, "mono ima4");
    ADD_AL(FORMAT_STEREO_IMA4, "stereo ima4");
    ADD_AL(FORMAT_MONO_MSADPCM_SOFT, "mono msadpcm");
    ADD_AL(FORMAT_STEREO_MSADPCM_SOFT, "stereo msadpcm");
    ADD_AL(FORMAT_BFORMAT2D_8, "bformat2d 8");
    ADD_AL(FORMAT_BFORMAT2D_16, "bformat2d 16");
    ADD_AL(FORMAT_BFORMAT2D_FLOAT32, "bformat2d float32");
    ADD_AL(FORMAT_BFORMAT3D_8, "bformat3d 8");
    ADD_AL(FORMAT_BFORMAT3D_16, "bformat3d 16");
    ADD_AL(FORMAT_BFORMAT3D_FLOAT32, "bformat3d float32");
    ADD_AL(FORMAT_BFORMAT2D_MULAW, "bformat2d mulaw");
    ADD_AL(FORMAT_BFORMAT3D_MULAW, "bformat3d mulaw");
    ADD_AL(FORMAT_MONO8, "mono8");
    ADD_AL(FORMAT_MONO16, "mono16");
    ADD_AL(FORMAT_STEREO8, "stereo8");
    ADD_AL(FORMAT_STEREO16, "stereo16");

#if 0 // AL_SOFT_buffer_samples extension
    domain = DOMAIN_AL_INTERNAL_FORMAT;
    ADD_AL(MONO8_SOFT, "mono8");
    ADD_AL(MONO16_SOFT, "mono16");
    ADD_AL(MONO32F_SOFT, "mono32f");
    ADD_AL(STEREO8_SOFT, "stereo8");
    ADD_AL(STEREO16_SOFT, "stereo16");
    ADD_AL(STEREO32F_SOFT, "stereo32f");
    ADD_AL(REAR8_SOFT, "rear8");
    ADD_AL(REAR16_SOFT, "rear16");
    ADD_AL(REAR32F_SOFT, "rear32f");
    ADD_AL(FORMAT_QUAD8_LOKI, "quad8 loki");
    ADD_AL(FORMAT_QUAD16_LOKI, "quad16 loki");
    ADD_AL(QUAD8_SOFT, "quad8");
    ADD_AL(QUAD16_SOFT, "quad16");
    ADD_AL(QUAD32F_SOFT, "quad32f");
    ADD_AL(5POINT1_8_SOFT, "5point1 8");
    ADD_AL(5POINT1_16_SOFT, "5point1 16");
    ADD_AL(5POINT1_32F_SOFT, "5point1 32f");
    ADD_AL(6POINT1_8_SOFT, "6point1 8");
    ADD_AL(6POINT1_16_SOFT, "6point1 16");
    ADD_AL(6POINT1_32F_SOFT, "6point1 32f");
    ADD_AL(7POINT1_8_SOFT, "7point1 8");
    ADD_AL(7POINT1_16_SOFT, "7point1 16");
    ADD_AL(7POINT1_32F_SOFT, "7point1 32f");
    ADD_AL(BFORMAT2D_8_SOFT, "bformat2d 8");
    ADD_AL(BFORMAT2D_16_SOFT, "bformat2d 16");
    ADD_AL(BFORMAT2D_32F_SOFT, "bformat2d 32f");
    ADD_AL(BFORMAT3D_8_SOFT, "bformat3d 8");
    ADD_AL(BFORMAT3D_16_SOFT, "bformat3d 16");
    ADD_AL(BFORMAT3D_32F_SOFT, "bformat3d 32f");
#endif

    domain = DOMAIN_AL_DISTANCE_MODEL;
    ADD_AL(NONE, "none");
    ADD_AL(INVERSE_DISTANCE, "inverse");
    ADD_AL(INVERSE_DISTANCE_CLAMPED, "inverse clamped");
    ADD_AL(LINEAR_DISTANCE, "linear");
    ADD_AL(LINEAR_DISTANCE_CLAMPED, "linear clamped");
    ADD_AL(EXPONENT_DISTANCE, "exponent");
    ADD_AL(EXPONENT_DISTANCE_CLAMPED, "exponent clamped");

    domain = DOMAIN_AL_RESAMPLER;
    /* These defines must match the 'enum Resampler' in openal-soft/OpenAL32/Include/alu.h: */
#define AL_POINT_RESAMPLER  0 /* PointResampler */
#define AL_LINEAR_RESAMPLER 1 /* LinearResampler */
#define AL_FIR4_RESAMPLER   2 /* FIR4Resampler */
#define AL_BSINC_RESAMPLER  3 /* BSincResampler */
    ADD_AL(POINT_RESAMPLER, "point");
    ADD_AL(LINEAR_RESAMPLER, "linear");
    ADD_AL(FIR4_RESAMPLER, "fir4");
    ADD_AL(BSINC_RESAMPLER, "bsinc");

    domain = DOMAIN_AL_SPATIALIZE_MODE;
    /* These defines must match the 'enum SpatializeMode' in openal-soft/OpenAL32/Include/alu.h: */
#define AL_SPATIALIZE_MODE_OFF  0 /* SpatializeOff */
#define AL_SPATIALIZE_MODE_ON   1 /* SpatializeOn */
#define AL_SPATIALIZE_MODE_AUTO 2 /* SpatializeAuto */
    ADD_AL(SPATIALIZE_MODE_OFF, "off");
    ADD_AL(SPATIALIZE_MODE_ON, "on");
    ADD_AL(SPATIALIZE_MODE_AUTO, "auto");

    domain = DOMAIN_AL_SOURCE_TYPE;
    ADD_AL(STATIC, "static");
    ADD_AL(STREAMING, "streaming");
    ADD_AL(UNDETERMINED, "undetermined");

    domain = DOMAIN_AL_SOURCE_STATE;
    ADD_AL(INITIAL, "initial");
    ADD_AL(PLAYING, "playing");
    ADD_AL(PAUSED, "paused");
    ADD_AL(STOPPED, "stopped");

    domain = DOMAIN_AL_EFFECT_TYPE;
    ADD_AL(EFFECT_NULL, "null");
    ADD_AL(EFFECT_REVERB, "reverb");
    ADD_AL(EFFECT_CHORUS, "chorus");
    ADD_AL(EFFECT_DISTORTION, "distortion");
    ADD_AL(EFFECT_ECHO, "echo");
    ADD_AL(EFFECT_FLANGER, "flanger");
    ADD_AL(EFFECT_FREQUENCY_SHIFTER, "frequency shifter");
    ADD_AL(EFFECT_VOCAL_MORPHER, "vocal morpher");
    ADD_AL(EFFECT_PITCH_SHIFTER, "pitch shifter");
    ADD_AL(EFFECT_RING_MODULATOR, "ring modulator");
    ADD_AL(EFFECT_AUTOWAH, "autowah");
    ADD_AL(EFFECT_COMPRESSOR, "compressor");
    ADD_AL(EFFECT_EQUALIZER, "equalizer");
    ADD_AL(EFFECT_EAXREVERB, "eaxreverb");
    ADD_AL(EFFECT_DEDICATED_DIALOGUE, "dedicated dialogue");
    ADD_AL(EFFECT_DEDICATED_LOW_FREQUENCY_EFFECT, "dedicated low frequency effect");

    domain = DOMAIN_AL_CHORUS_WAVEFORM;
    ADD_AL(CHORUS_WAVEFORM_SINUSOID, "sinusoid");
    ADD_AL(CHORUS_WAVEFORM_TRIANGLE, "triangle");
    
    domain = DOMAIN_AL_FLANGER_WAVEFORM;
    ADD_AL(FLANGER_WAVEFORM_SINUSOID, "sinusoid");
    ADD_AL(FLANGER_WAVEFORM_TRIANGLE, "triangle");

    domain = DOMAIN_AL_RING_MODULATOR_WAVEFORM;
    ADD_AL(RING_MODULATOR_SINUSOID, "sinusoid");
    ADD_AL(RING_MODULATOR_SAWTOOTH, "sawtooth");
    ADD_AL(RING_MODULATOR_SQUARE, "square");

    domain = DOMAIN_AL_COMPRESSOR_ONOFF;
#define AL_COMPRESSOR_OFF AL_COMPRESSOR_MIN_ONOFF
#define AL_COMPRESSOR_ON AL_COMPRESSOR_MAX_ONOFF
    ADD_AL(COMPRESSOR_OFF, "off");
    ADD_AL(COMPRESSOR_ON, "on");

    domain = DOMAIN_AL_FILTER_TYPE;
    ADD_AL(FILTER_NULL, "null");
    ADD_AL(FILTER_LOWPASS, "lowpass");
    ADD_AL(FILTER_HIGHPASS, "highpass");
    ADD_AL(FILTER_BANDPASS, "bandpass");

    domain = DOMAIN_ALC_PARAM;
    ADD_ALC(SYNC, "sync");
    ADD_ALC(HRTF_SOFT, "hrtf");
    ADD_ALC(HRTF_SPECIFIER_SOFT, "hrtf specifier");
    ADD_ALC(HRTF_STATUS_SOFT, "hrtf status");
    ADD_ALC(OUTPUT_LIMITER_SOFT, "output limiter");
    ADD_ALC(FREQUENCY, "frequency");
    ADD_ALC(REFRESH, "refresh");
    ADD_ALC(MONO_SOURCES, "mono sources");
    ADD_ALC(STEREO_SOURCES, "stereo sources");
    ADD_ALC(MAX_AUXILIARY_SENDS, "max auxiliary sends");
    ADD_ALC(HRTF_ID_SOFT, "hrtf id");
    ADD_ALC(FORMAT_TYPE_SOFT, "format type");
    ADD_ALC(FORMAT_CHANNELS_SOFT, "format channels");

    domain = DOMAIN_AL_PARAM;
    ADD_AL(GAIN, "gain");
    ADD_AL(METERS_PER_UNIT, "meters per unit");
    ADD_AL(POSITION, "position");
    ADD_AL(VELOCITY, "velocity");
    ADD_AL(ORIENTATION, "orientation");
    ADD_AL(PITCH, "pitch");
    ADD_AL(CONE_INNER_ANGLE, "cone inner angle");
    ADD_AL(CONE_OUTER_ANGLE, "cone outer angle");
    ADD_AL(MAX_DISTANCE, "max distance");
    ADD_AL(ROLLOFF_FACTOR, "rolloff factor");
    ADD_AL(REFERENCE_DISTANCE, "reference distance");
    ADD_AL(MIN_GAIN, "min gain");
    ADD_AL(MAX_GAIN, "max gain");
    ADD_AL(GAIN_LIMIT_SOFT, "gain limit");
    ADD_AL(CONE_OUTER_GAIN, "cone outer gain");
    ADD_AL(CONE_OUTER_GAINHF, "cone outer gainhf");
    ADD_AL(AIR_ABSORPTION_FACTOR, "air absorption factor");
    ADD_AL(ROOM_ROLLOFF_FACTOR, "room rolloff factor");
    ADD_AL(DOPPLER_FACTOR, "doppler factor");
    ADD_AL(DOPPLER_VELOCITY, "doppler velocity");
    ADD_AL(SPEED_OF_SOUND, "speed of sound");
    ADD_AL(SEC_OFFSET, "sec offset");
    ADD_AL(SAMPLE_OFFSET, "sample offset");
    ADD_AL(BYTE_OFFSET, "byte offset");
    ADD_AL(SOURCE_RADIUS, "radius");
    ADD_AL(SEC_LENGTH_SOFT, "sec length");
    ADD_AL(DIRECTION, "direction");
    ADD_AL(SOURCE_RELATIVE, "relative");
    ADD_AL(LOOPING, "looping");
    ADD_AL(DIRECT_FILTER_GAINHF_AUTO, "direct filter gainhf auto");
    ADD_AL(AUXILIARY_SEND_FILTER_GAIN_AUTO, "auxiliary send filter gain auto");
    ADD_AL(AUXILIARY_SEND_FILTER_GAINHF_AUTO, "auxiliary send filter gainhf auto");
    ADD_AL(DIRECT_CHANNELS_SOFT, "direct channels");
    ADD_AL(BUFFERS_QUEUED, "buffers queued");
    ADD_AL(BUFFERS_PROCESSED, "buffers processed");
    ADD_AL(BYTE_LENGTH_SOFT, "byte length");
    ADD_AL(SAMPLE_LENGTH_SOFT, "sample length");
    ADD_AL(SEC_OFFSET_LATENCY_SOFT, "sec offset latency");
    ADD_AL(SAMPLE_OFFSET_LATENCY_SOFT, "sample offset latency");
    ADD_AL(STEREO_ANGLES, "stereo angles");
    ADD_AL(SOURCE_TYPE, "type");
    ADD_AL(SOURCE_STATE, "state");
    ADD_AL(DISTANCE_MODEL, "distance model");
    ADD_AL(SOURCE_RESAMPLER_SOFT, "resampler");
    ADD_AL(SOURCE_SPATIALIZE_SOFT, "spatialize");
    ADD_AL(BUFFER, "buffer");
    ADD_AL(DIRECT_FILTER, "direct filter");
    ADD_AL(AUXILIARY_SEND_FILTER, "auxiliary send filter");
    ADD_AL(DEFERRED_UPDATES_SOFT, "deferred updates");
    ADD_AL(DEFAULT_RESAMPLER_SOFT, "default resampler");
    ADD_AL(RESAMPLER_NAME_SOFT, "resampler names");
    ADD_AL(UNPACK_BLOCK_ALIGNMENT_SOFT, "unpack block alignment");
    ADD_AL(PACK_BLOCK_ALIGNMENT_SOFT, "pack block alignment");
    ADD_AL(LOOP_POINTS_SOFT, "loop points");
    ADD_AL(FREQUENCY, "frequency");
    ADD_AL(BITS, "bits");
    ADD_AL(CHANNELS, "channels");
    ADD_AL(SIZE, "size");
    ADD_AL(INTERNAL_FORMAT_SOFT, "internal format");

    domain = DOMAIN_AL_CHORUS_PARAM;
    ADD_AL(CHORUS_WAVEFORM, "waveform");
    ADD_AL(CHORUS_RATE, "rate");
    ADD_AL(CHORUS_DEPTH, "depth");
    ADD_AL(CHORUS_FEEDBACK, "feedback");
    ADD_AL(CHORUS_DELAY, "delay");
    ADD_AL(CHORUS_PHASE, "phase");

    domain = DOMAIN_AL_REVERB_PARAM;
    ADD_AL(REVERB_DENSITY, "density");
    ADD_AL(REVERB_DIFFUSION, "diffusion");
    ADD_AL(REVERB_GAIN, "gain");
    ADD_AL(REVERB_GAINHF, "gainhf");
    ADD_AL(REVERB_DECAY_TIME, "decay time");
    ADD_AL(REVERB_DECAY_HFRATIO, "decay hfratio");
    ADD_AL(REVERB_REFLECTIONS_GAIN, "reflections gain");
    ADD_AL(REVERB_REFLECTIONS_DELAY, "reflections delay");
    ADD_AL(REVERB_LATE_REVERB_GAIN, "late gain");
    ADD_AL(REVERB_LATE_REVERB_DELAY, "late delay");
    ADD_AL(REVERB_AIR_ABSORPTION_GAINHF, "air absorption gainhf");
    ADD_AL(REVERB_ROOM_ROLLOFF_FACTOR, "room rolloff factor");
    ADD_AL(REVERB_DECAY_HFLIMIT, "decay hflimit");

    domain = DOMAIN_AL_DISTORTION_PARAM;
    ADD_AL(DISTORTION_EDGE, "edge");
    ADD_AL(DISTORTION_GAIN, "gain");
    ADD_AL(DISTORTION_LOWPASS_CUTOFF, "lowpass cutoff");
    ADD_AL(DISTORTION_EQCENTER, "eqcenter");
    ADD_AL(DISTORTION_EQBANDWIDTH, "eqbandwidth");

    domain = DOMAIN_AL_ECHO_PARAM;
    ADD_AL(ECHO_DELAY, "delay");
    ADD_AL(ECHO_LRDELAY, "lrdelay");
    ADD_AL(ECHO_DAMPING, "damping");
    ADD_AL(ECHO_FEEDBACK, "feedback");
    ADD_AL(ECHO_SPREAD, "spread");

    domain = DOMAIN_AL_FLANGER_PARAM;
    ADD_AL(FLANGER_WAVEFORM, "waveform");
    ADD_AL(FLANGER_RATE, "rate");
    ADD_AL(FLANGER_DEPTH, "depth");
    ADD_AL(FLANGER_FEEDBACK, "feedback");
    ADD_AL(FLANGER_DELAY, "delay");
    ADD_AL(FLANGER_PHASE, "phase");

    domain = DOMAIN_AL_RING_MODULATOR_PARAM;
    ADD_AL(RING_MODULATOR_WAVEFORM, "waveform");
    ADD_AL(RING_MODULATOR_FREQUENCY, "frequency");
    ADD_AL(RING_MODULATOR_HIGHPASS_CUTOFF, "highpass cutoff");

    domain = DOMAIN_AL_COMPRESSOR_PARAM;
    ADD_AL(COMPRESSOR_ONOFF, "onoff");

    domain = DOMAIN_AL_EQUALIZER_PARAM;
    ADD_AL(EQUALIZER_LOW_GAIN, "low gain");
    ADD_AL(EQUALIZER_LOW_CUTOFF, "low cutoff");
    ADD_AL(EQUALIZER_MID1_GAIN, "mid1 gain");
    ADD_AL(EQUALIZER_MID1_CENTER, "mid1 center");
    ADD_AL(EQUALIZER_MID1_WIDTH, "mid1 width");
    ADD_AL(EQUALIZER_MID2_GAIN, "mid2 gain");
    ADD_AL(EQUALIZER_MID2_CENTER, "mid2 center");
    ADD_AL(EQUALIZER_MID2_WIDTH, "mid2 width");
    ADD_AL(EQUALIZER_HIGH_GAIN, "high gain");
    ADD_AL(EQUALIZER_HIGH_CUTOFF, "high cutoff");

    domain = DOMAIN_AL_EAXREVERB_PARAM;
    ADD_AL(EAXREVERB_DENSITY, "density");
    ADD_AL(EAXREVERB_DIFFUSION, "diffusion");
    ADD_AL(EAXREVERB_GAIN, "gain");
    ADD_AL(EAXREVERB_GAINHF, "gainhf");
    ADD_AL(EAXREVERB_GAINLF, "gainlf");
    ADD_AL(EAXREVERB_DECAY_TIME, "decay time");
    ADD_AL(EAXREVERB_DECAY_HFRATIO, "decay hfratio");
    ADD_AL(EAXREVERB_DECAY_LFRATIO, "decay lfratio");
    ADD_AL(EAXREVERB_DECAY_HFLIMIT, "decay hflimit");
    ADD_AL(EAXREVERB_REFLECTIONS_GAIN, "reflections gain");
    ADD_AL(EAXREVERB_REFLECTIONS_DELAY, "reflections delay");
    ADD_AL(EAXREVERB_LATE_REVERB_GAIN, "late reverb gain");
    ADD_AL(EAXREVERB_LATE_REVERB_DELAY, "late reverb delay");
    ADD_AL(EAXREVERB_AIR_ABSORPTION_GAINHF, "air absorption gainhf");
    ADD_AL(EAXREVERB_ECHO_TIME, "echo time");
    ADD_AL(EAXREVERB_ECHO_DEPTH, "echo depth");
    ADD_AL(EAXREVERB_MODULATION_TIME, "modulation time");
    ADD_AL(EAXREVERB_MODULATION_DEPTH, "modulation depth");
    ADD_AL(EAXREVERB_HFREFERENCE, "hfreference");
    ADD_AL(EAXREVERB_LFREFERENCE, "lfreference");
    ADD_AL(EAXREVERB_ROOM_ROLLOFF_FACTOR, "room rolloff factor");
    ADD_AL(EAXREVERB_LATE_REVERB_PAN, "late reverb pan");
    ADD_AL(EAXREVERB_REFLECTIONS_PAN, "reflections pan");

    domain = DOMAIN_AL_DEDICATED_PARAM;
    ADD_AL(DEDICATED_GAIN, "gain");

    domain = DOMAIN_AL_LOWPASS_PARAM;
    ADD_AL(LOWPASS_GAIN, "gain");
    ADD_AL(LOWPASS_GAINHF, "gainhf");

    domain = DOMAIN_AL_HIGHPASS_PARAM;
    ADD_AL(HIGHPASS_GAIN, "gain");
    ADD_AL(HIGHPASS_GAINLF, "gainlf");

    domain = DOMAIN_AL_BANDPASS_PARAM;
    ADD_AL(BANDPASS_GAIN, "gain");
    ADD_AL(BANDPASS_GAINHF, "gainhf");
    ADD_AL(BANDPASS_GAINLF, "gainlf");

    domain = DOMAIN_AL_EFFECTSLOT_PARAM;
    ADD_AL(EFFECTSLOT_GAIN, "gain");
    ADD_AL(EFFECTSLOT_AUXILIARY_SEND_AUTO, "auxiliary send auto");
    ADD_AL(EFFECTSLOT_EFFECT, "effect");

    domain = DOMAIN_ALC_HRTF_STATUS;
    ADD_ALC(HRTF_DISABLED_SOFT, "disabled");
    ADD_ALC(HRTF_ENABLED_SOFT, "enabled");
    ADD_ALC(HRTF_DENIED_SOFT, "denied");
    ADD_ALC(HRTF_REQUIRED_SOFT, "required");
    ADD_ALC(HRTF_HEADPHONES_DETECTED_SOFT, "headphones detected");
    ADD_ALC(HRTF_UNSUPPORTED_FORMAT_SOFT, "unsupported format");


#if 0 /* scaffolding */
4yy
#define DOMAIN_AL_
    domain = DOMAIN_AL_;
    ADD_AL(, "");

#endif

#undef ADD_AL
#undef ADD_ALC
    }


