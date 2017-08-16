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
 | Misc utilities                                                               |
 *------------------------------------------------------------------------------*/

int noprintf(const char *fmt, ...) 
    { (void)fmt; return 0; }

int notavailable(lua_State *L, ...) 
    { 
    return luaL_error(L, "function not available in this CL version");
    }

int tablelen(lua_State *L, int arg)
/* counts the elements of the table at arg */
    {
    int n = 0;
    if(!lua_istable(L, arg))
        return luaL_error(L, errstring(ERR_TABLE));
    lua_pushnil(L);
    while(lua_next(L, arg) != 0)
        { lua_pop(L, 1); n++; }
    return n;
    }


/* Utilities to scan lists of strings ---------------------------------------*/

/* List of strings separated by a space and ending with a NUL */

char *firststringS(const char *pp, size_t *len)
    {
    char *p = (char*)pp;
    *len = 0;
    while((*p != ' ')&&(*p != '\0')) { p++; (*len)++; }
    return *len > 0 ?  (char*)pp : NULL;
    }

char *nextstringS(const char *pp, size_t *len)
    {
    char *p = (char*)pp;
    while((*p != ' ')&&(*p != '\0')) p++;
    p++;
    return firststringS(p, len);
    }

/* List of strings separated by a NUL and ending with a double NUL */

char *firststringN(const char *pp)
    {
    char *p = (char*)pp;
    return *p!='\0' ? p : NULL;
    }


char *nextstringN(const char *pp)
    {
    char *p = (char*)pp;
    while(*p!='\0') p++;
    p++;
    return *p!='\0' ? p : NULL;
    }

/*------------------------------------------------------------------------------*
 | Time utilities                                                               |
 *------------------------------------------------------------------------------*/

#if defined(LINUX)

#if 0
static double tstosec(const struct timespec *ts)
    {
    return ts->tv_sec*1.0+ts->tv_nsec*1.0e-9;
    }
#endif

static void sectots(struct timespec *ts, double seconds)
    {
    ts->tv_sec=(time_t)seconds;
    ts->tv_nsec=(long)((seconds-((double)ts->tv_sec))*1.0e9);
    }

double now(void)
    {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC,&ts)!=0)
        { printf("clock_gettime error\n"); return -1; }
    return ts.tv_sec + ts.tv_nsec*1.0e-9;
#else
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != 0)
        { printf("gettimeofday error\n"); return -1; }
    return tv.tv_sec + tv.tv_usec*1.0e-6;
#endif
    }

void sleeep(double seconds)
    {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts, ts1;
    struct timespec *req, *rem, *tmp;
    sectots(&ts, seconds);
    req = &ts;
    rem = &ts1; 
    while(1)
        {   
        if(nanosleep(req, rem) == 0) 
            return;
        tmp = req;
        req = rem;
        rem = tmp;
        }
#else
    usleep((useconds_t)(seconds*1.0e6));
#endif
    }

#define time_init(L) do { /* do nothing */ } while(0)

#elif defined(MINGW)

#include <windows.h>

static LARGE_INTEGER Frequency;
double now(void)
    {
    LARGE_INTEGER ts;
    QueryPerformanceCounter(&ts);
    return ((double)(ts.QuadPart))/Frequency.QuadPart;
    }

void sleeep(double seconds)
    {
    DWORD msec = (DWORD)seconds * 1000;
    //if(msec < 0) return;  DWORD seems to be unsigned
    Sleep(msec);
    }

static void time_init(lua_State *L)
    {
    (void)L;
    QueryPerformanceFrequency(&Frequency);
    }

#endif



/*------------------------------------------------------------------------------*
 | Malloc                                                                       |
 *------------------------------------------------------------------------------*/

/* We do not use malloc(), free() etc directly. Instead, we inherit the memory 
 * allocator from the main Lua state instead (see lua_getallocf in the Lua manual)
 * and use that.
 *
 * By doing so, we can use an alternative malloc() implementation without recompiling
 * this library (we have needs to recompile lua only, or execute it with LD_PRELOAD
 * set to the path to the malloc library we want to use).
 */
static lua_Alloc Alloc = NULL;
static void* AllocUd = NULL;

static void malloc_init(lua_State *L)
    {
    if(Alloc) unexpected(L);
    Alloc = lua_getallocf(L, &AllocUd);
    }

static void* Malloc_(size_t size)
    { return Alloc ? Alloc(AllocUd, NULL, 0, size) : NULL; }

static void Free_(void *ptr)
    { if(Alloc) Alloc(AllocUd, ptr, 0, 0); }

void *Malloc(lua_State *L, size_t size)
    {
    void *ptr = Malloc_(size);
    if(ptr==NULL)
        { luaL_error(L, errstring(ERR_MEMORY)); return NULL; }
    memset(ptr, 0, size);
    //DBG("Malloc %p\n", ptr);
    return ptr;
    }

void *MallocNoErr(lua_State *L, size_t size) /* do not raise errors (check the retval) */
    {
    void *ptr = Malloc_(size);
    (void)L;
    if(ptr==NULL)
        return NULL;
    memset(ptr, 0, size);
    //DBG("MallocNoErr %p\n", ptr);
    return ptr;
    }

char *Strdup(lua_State *L, const char *s)
    {
    size_t len = strnlen(s, 256);
    char *ptr = (char*)Malloc(L, len + 1);
    if(len>0)
        memcpy(ptr, s, len);
    ptr[len]='\0';
    return ptr;
    }


void Free(lua_State *L, void *ptr)
    {
    (void)L;
    //DBG("Free %p\n", ptr);
    if(ptr) Free_(ptr);
    }

/*------------------------------------------------------------------------------*
 | Light userdata                                                               |
 *------------------------------------------------------------------------------*/

void *checklightuserdata(lua_State *L, int arg)
    {
    if(lua_type(L, arg) != LUA_TLIGHTUSERDATA)
        { luaL_argerror(L, arg, "expected lightuserdata"); return NULL; }
    return lua_touserdata(L, arg);
    }
    
void *optlightuserdata(lua_State *L, int arg)
    {
    if(lua_isnoneornil(L, arg))
        return NULL;
    return checklightuserdata(L, arg);
    }

/*------------------------------------------------------------------------------*
 | Custom luaL_checkxxx() style functions                                       |
 *------------------------------------------------------------------------------*/

int checkboolean(lua_State *L, int arg)
    {
    if(!lua_isboolean(L, arg))
        return (int)luaL_argerror(L, arg, "boolean expected");
    return lua_toboolean(L, arg); // ? AL_TRUE : AL_FALSE;
    }


int testboolean(lua_State *L, int arg, int *err)
    {
    if(!lua_isboolean(L, arg))
        { *err = ERR_TYPE; return 0; }
    *err = 0;
    return lua_toboolean(L, arg); // ? AL_TRUE : AL_FALSE;
    }


int optboolean(lua_State *L, int arg, int d)
    {
    if(!lua_isboolean(L, arg))
        return d;
    return lua_toboolean(L, arg);
    }

#if 0
/* 1-based index to 0-based ------------------------------------------*/

int testindex(lua_State *L, int arg, int *err)
    {
    int val;
    if(!lua_isinteger(L, arg))
        { *err = ERR_TYPE; return 0; }
    val = lua_tointeger(L, arg);
    if(val < 1)
        { *err = ERR_GENERIC; return 0; }
    *err = 0;
    return val - 1;
    }

int checkindex(lua_State *L, int arg)
    {
    int val = luaL_checkinteger(L, arg);
    if(val < 1)
        return luaL_argerror(L, arg, "positive integer expected");
    return val - 1;
    }

int optindex(lua_State *L, int arg, int optval /* 0-based */)
    {
    int val = luaL_optinteger(L, arg, optval + 1);
    if(val < 1)
        return luaL_argerror(L, arg, "positive integer expected");
    return val - 1;
    }

void pushindex(lua_State *L, int val)
    { lua_pushinteger((L), (val) + 1); }
#endif

/*------------------------------------------------------------------------------*
 | Internal error codes                                                         |
 *------------------------------------------------------------------------------*/

const char* errstring(int err)
    {
    switch(err)
        {
        case 0: return "success";
        case ERR_GENERIC: return "generic error";
        case ERR_TABLE: return "not a table";
        case ERR_EMPTY: return "empty list";
        case ERR_TYPE: return "invalid type";
        case ERR_VALUE: return "invalid value";
        case ERR_NOTPRESENT: return "missing";
        case ERR_MEMORY: return "out of memory";
        case ERR_LENGTH: return "invalid length";
        case ERR_POOL: return "elements are not from the same pool";
        case ERR_BOUNDARIES: return "invalid boundaries";
        case ERR_UNKNOWN: return "unknown field name";
        default:
            return "???";
        }
    return NULL; /* unreachable */
    }

/*------------------------------------------------------------------------------*
 | OpenAL error codes                                                           |
 *------------------------------------------------------------------------------*/

int pushalerror(lua_State *L, ALenum ec)
    {
    switch(ec)
        {
#define CASE(c, s) case c: lua_pushstring(L, s); return 1
        CASE(AL_NO_ERROR, "no error");
        CASE(AL_INVALID_NAME, "invalid name");
        CASE(AL_INVALID_ENUM, "invalid enum");
        CASE(AL_INVALID_VALUE, "invalid value");
        CASE(AL_INVALID_OPERATION, "invalid operation");
        CASE(AL_OUT_OF_MEMORY, "out of memory");
#undef CASE
        default:
            lua_pushfstring(L, "unknown AL error code %d", ec); 
            return 1;
            //return unexpected(L);
        }
    return 0;
    }

int pushalcerror(lua_State *L, ALCenum ec)
    {
    switch(ec)
        {
#define CASE(c, s) case c: lua_pushstring(L, s); return 1
        CASE(ALC_NO_ERROR, "no error");
        CASE(ALC_INVALID_DEVICE, "invalid device");
        CASE(ALC_INVALID_CONTEXT, "invalid context");
        CASE(ALC_INVALID_ENUM, "invalid enum");
        CASE(ALC_INVALID_VALUE, "invalid value");
        CASE(ALC_OUT_OF_MEMORY, "out of memory");
#undef CASE
        default:
            lua_pushfstring(L, "unknown ALC error code %d", ec); 
            return 1;
            //return unexpected(L);
        }
    return 0;
    }

size_t formatchannels(lua_State *L, ALenum fmt)
    {
    switch(fmt)
        {
    // case format: return channels*sizeof(type); 
        case AL_FORMAT_MONO8: return 1;
        case AL_FORMAT_MONO16: return 1;
        case AL_FORMAT_MONO_FLOAT32: return 1;
        case AL_FORMAT_MONO_DOUBLE_EXT: return 1;
        case AL_FORMAT_MONO_IMA4: return 1;
        case AL_FORMAT_MONO_MSADPCM_SOFT: return 1;
    //  case AL_FORMAT_MONO_MULAW_EXT:
        case AL_FORMAT_MONO_MULAW: return 1;
        case AL_FORMAT_MONO_ALAW_EXT: return 1;
        case AL_FORMAT_STEREO8: return 2;
        case AL_FORMAT_STEREO16: return 2;
        case AL_FORMAT_STEREO_FLOAT32: return 2;
        case AL_FORMAT_STEREO_DOUBLE_EXT: return 2;
        case AL_FORMAT_STEREO_IMA4: return 2;
        case AL_FORMAT_STEREO_MSADPCM_SOFT: return 2;
    //  case AL_FORMAT_STEREO_MULAW_EXT:
        case AL_FORMAT_STEREO_MULAW: return 2;
        case AL_FORMAT_STEREO_ALAW_EXT: return 2;
        case AL_FORMAT_REAR8: return 2;
        case AL_FORMAT_REAR16: return 2;
        case AL_FORMAT_REAR32: return 2;
        case AL_FORMAT_REAR_MULAW: return 2;
        case AL_FORMAT_QUAD8_LOKI: return 4;
        case AL_FORMAT_QUAD16_LOKI: return 4;
        case AL_FORMAT_QUAD8: return 4;
        case AL_FORMAT_QUAD16: return 4;
        case AL_FORMAT_QUAD32: return 4;
        case AL_FORMAT_QUAD_MULAW: return 4;
        case AL_FORMAT_51CHN8: return 6;
        case AL_FORMAT_51CHN16: return 6;
        case AL_FORMAT_51CHN32: return 6;
        case AL_FORMAT_51CHN_MULAW: return 6;
        case AL_FORMAT_61CHN8: return 7;
        case AL_FORMAT_61CHN16: return 7;
        case AL_FORMAT_61CHN32: return 7;
        case AL_FORMAT_61CHN_MULAW: return 7;
        case AL_FORMAT_71CHN8: return 8;
        case AL_FORMAT_71CHN16: return 8;
        case AL_FORMAT_71CHN32: return 8;
        case AL_FORMAT_71CHN_MULAW: return 8;
        case AL_FORMAT_BFORMAT2D_8: return 3;
        case AL_FORMAT_BFORMAT2D_16: return 3;
        case AL_FORMAT_BFORMAT2D_FLOAT32: return 3;
        case AL_FORMAT_BFORMAT2D_MULAW: return 3;
        case AL_FORMAT_BFORMAT3D_8: return 4;
        case AL_FORMAT_BFORMAT3D_16: return 4;
        case AL_FORMAT_BFORMAT3D_FLOAT32: return 4;
        case AL_FORMAT_BFORMAT3D_MULAW: return 4;
//      case AL_FORMAT_IMA_ADPCM_MONO16 _EXT:
//      case AL_FORMAT_IMA_ADPCM_STEREO16 _EXT:
//      case AL_FORMAT_WAVE _EXT:
//      case AL_FORMAT_VORBIS _EXT:
        default:
            return unexpected(L);
        }
    return 0;
    }

static size_t vframesize(lua_State *L, ALenum fmt, size_t channels, ALsizei align)
/* Blindly copied from OpenAL32/alBuffer.c (OpenAL-SOFT sources) */
    {
    if(align < 0)
        return luaL_error(L, "block alignment must non-negative");

    switch(fmt)
        {
        case AL_FORMAT_MONO_IMA4: 
        case AL_FORMAT_STEREO_IMA4:
                align = (align == 0) ? 65 : align;
                if((align&7) != 1)
                    return luaL_error(L, "IMA4 block alignment must be a multiple of 8, plus 1");
                return ((align-1)/2 + 4) * channels;

        case AL_FORMAT_MONO_MSADPCM_SOFT:
        case AL_FORMAT_STEREO_MSADPCM_SOFT:
                align = (align == 0) ? 64 : align;
                if((align&1) != 0)
                    return luaL_error(L, "MSADPCM block alignment must be a multiple of 2");
                return ((align-2)/2 + 7) * channels;
        default: 
            return unexpected(L);
        }

    return 0;
    }


size_t formatframesize(lua_State *L, ALenum fmt, ALsizei align)
    {
    switch(fmt)
        {
        case AL_FORMAT_MONO_IMA4: 
        case AL_FORMAT_MONO_MSADPCM_SOFT: return vframesize(L, fmt, 1, align);
        case AL_FORMAT_STEREO_IMA4:
        case AL_FORMAT_STEREO_MSADPCM_SOFT: return vframesize(L, fmt, 2, align);
        /* case format: return channels*sizeof(type);  */
        case AL_FORMAT_MONO8: return 1*sizeof(uint8_t);
        case AL_FORMAT_MONO16: return 1*sizeof(uint16_t);
        case AL_FORMAT_MONO_FLOAT32: return 1*sizeof(float);
        case AL_FORMAT_MONO_DOUBLE_EXT: return 1*sizeof(double);
//      case AL_FORMAT_MONO_MULAW_EXT:
        case AL_FORMAT_MONO_MULAW: return 1*sizeof(uint8_t);
        case AL_FORMAT_MONO_ALAW_EXT: return 1*sizeof(uint8_t);
        case AL_FORMAT_STEREO8: return 2*sizeof(uint8_t);
        case AL_FORMAT_STEREO16: return 2*sizeof(uint16_t);
        case AL_FORMAT_STEREO_FLOAT32: return 2*sizeof(float);
        case AL_FORMAT_STEREO_DOUBLE_EXT: return 2*sizeof(double);
//      case AL_FORMAT_STEREO_MULAW_EXT:
        case AL_FORMAT_STEREO_MULAW: return 2*sizeof(uint8_t);
        case AL_FORMAT_STEREO_ALAW_EXT: return 2*sizeof(uint8_t);
        case AL_FORMAT_REAR8: return 2*sizeof(uint8_t);
        case AL_FORMAT_REAR16: return 2*sizeof(uint16_t);
        case AL_FORMAT_REAR32: return 2*sizeof(float);
        case AL_FORMAT_REAR_MULAW: return 2*sizeof(uint8_t);
        case AL_FORMAT_QUAD8_LOKI: return 4*sizeof(uint8_t);
        case AL_FORMAT_QUAD16_LOKI: return 4*sizeof(uint16_t);
        case AL_FORMAT_QUAD8: return 4*sizeof(uint8_t);
        case AL_FORMAT_QUAD16: return 4*sizeof(uint16_t);
        case AL_FORMAT_QUAD32: return 4*sizeof(float);
        case AL_FORMAT_QUAD_MULAW: return 4*sizeof(uint8_t);
        case AL_FORMAT_51CHN8: return 6*sizeof(uint8_t);
        case AL_FORMAT_51CHN16: return 6*sizeof(uint16_t);
        case AL_FORMAT_51CHN32: return 6*sizeof(float);
        case AL_FORMAT_51CHN_MULAW: return 6*sizeof(uint8_t);
        case AL_FORMAT_61CHN8: return 7*sizeof(uint8_t);
        case AL_FORMAT_61CHN16: return 7*sizeof(uint16_t);
        case AL_FORMAT_61CHN32: return 7*sizeof(float);
        case AL_FORMAT_61CHN_MULAW: return 7*sizeof(uint8_t);
        case AL_FORMAT_71CHN8: return 8*sizeof(uint8_t);
        case AL_FORMAT_71CHN16: return 8*sizeof(uint16_t);
        case AL_FORMAT_71CHN32: return 8*sizeof(float);
        case AL_FORMAT_71CHN_MULAW: return 8*sizeof(uint8_t);
        case AL_FORMAT_BFORMAT2D_8: return 3*sizeof(uint8_t);
        case AL_FORMAT_BFORMAT2D_16: return 3*sizeof(uint16_t);
        case AL_FORMAT_BFORMAT2D_FLOAT32: return 3*sizeof(float);
        case AL_FORMAT_BFORMAT2D_MULAW: return 3*sizeof(uint8_t);
        case AL_FORMAT_BFORMAT3D_8: return 4*sizeof(uint8_t);
        case AL_FORMAT_BFORMAT3D_16: return 4*sizeof(uint16_t);
        case AL_FORMAT_BFORMAT3D_FLOAT32: return 4*sizeof(float);
        case AL_FORMAT_BFORMAT3D_MULAW: return 4*sizeof(uint8_t);
//      case AL_FORMAT_IMA_ADPCM_MONO16 _EXT:
//      case AL_FORMAT_IMA_ADPCM_STEREO16 _EXT:
//      case AL_FORMAT_WAVE _EXT:
//      case AL_FORMAT_VORBIS _EXT:
        default:
            return unexpected(L);
        }
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Inits                                                                        |
 *------------------------------------------------------------------------------*/

void moonal_utils_init(lua_State *L)
    {
    malloc_init(L);
    time_init(L);
    }

