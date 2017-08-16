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

/********************************************************************************
 * Internal common header                                                       *
 ********************************************************************************/

#ifndef internalDEFINED
#define internalDEFINED

#define _ISOC11_SOURCE /* see man aligned_alloc(3) */
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "moonal.h"

/* AL TYPES ADAPTATION (for internal use) ------------- */
#define device_t ALCdevice*
#define context_t ALCcontext*
typedef struct { ALuint name; } object_t;
#define buffer_t object_t*
#define listener_t object_t*
#define source_t object_t*
#define effect_t object_t*
#define filter_t object_t*
#define auxslot_t object_t*

#define TOSTR_(x) #x
#define TOSTR(x) TOSTR_(x)

#include "tree.h"
#include "getproc.h"
#include "objects.h"
#include "enums.h"

/* Note: all the dynamic symbols of this library (should) start with 'moonal_' .
 * The only exception is the luaopen_moonal() function, which is searched for
 * with that name by Lua.
 * MoonAL's string references on the Lua registry also start with 'moonal_'.
 */


#if 0
/* .c */
#define  moonal_
#endif

/* utils.c */
#define noprintf moonal_noprintf
int noprintf(const char *fmt, ...); 
#define now moonal_now
double now(void);
#define since(t) (now() - (t))
#define sleeep moonal_sleeep
void sleeep(double seconds);
#define notavailable moonal_notavailable
int notavailable(lua_State *L, ...);
#define tablelen moonal_tablelen
int tablelen(lua_State *L, int arg);
#define firststringS moonal_firststringS
char *firststringS(const char *pp, size_t *len);
#define nextstringS moonal_nextstringS
char *nextstringS(const char *pp, size_t *len);
#define firststringN moonal_firststringN
char *firststringN(const char *pp);
#define nextstringN moonal_nextstringN
char *nextstringN(const char *pp);
#define Malloc moonal_Malloc
void *Malloc(lua_State *L, size_t size);
#define MallocNoErr moonal_MallocNoErr
void *MallocNoErr(lua_State *L, size_t size);
#define Strdup moonal_Strdup
char *Strdup(lua_State *L, const char *s);
#define Free moonal_Free
void Free(lua_State *L, void *ptr);
#define checkboolean moonal_checkboolean
int checkboolean(lua_State *L, int arg);
#define testboolean moonal_testboolean
int testboolean(lua_State *L, int arg, int *err);
#define optboolean moonal_optboolean
int optboolean(lua_State *L, int arg, int d);
#define checklightuserdata moonal_checklightuserdata
void *checklightuserdata(lua_State *L, int arg);
#define optlightuserdata moonal_optlightuserdata
void *optlightuserdata(lua_State *L, int arg);
#if 0
#define testindex moonal_testindex
int testindex(lua_State *L, int arg, int *err);
#define checkindex moonal_checkindex
int checkindex(lua_State *L, int arg);
#define optindex moonal_optindex
int optindex(lua_State *L, int arg, int optval);
#define pushindex moonal_pushindex
void pushindex(lua_State *L, int val);
#endif
#define pushalerror moonal_pushalerror
int pushalerror(lua_State *L, ALenum ec);
#define pushalcerror moonal_pushalcerror
int pushalcerror(lua_State *L, ALCenum ec);
#define formatchannels moonal_formatchannels
size_t formatchannels(lua_State *L, ALenum fmt);
#define formatframesize moonal_formatframesize
size_t formatframesize(lua_State *L, ALenum fmt, ALsizei align);

/* Internal error codes */
#define ERR_NOTPRESENT       1
#define ERR_SUCCESS          0
#define ERR_GENERIC         -1
#define ERR_TYPE            -2
#define ERR_VALUE           -3
#define ERR_TABLE           -4
#define ERR_EMPTY           -5
#define ERR_MEMORY          -6
#define ERR_LENGTH          -7
#define ERR_POOL            -8
#define ERR_BOUNDARIES      -9
#define ERR_UNKNOWN         -10
#define errstring moonal_errstring
const char* errstring(int err);

/* context.c */
#define make_context_current moonal_make_context_current
int make_context_current(lua_State *L, context_t context);
#define current_context moonal_current_context
context_t current_context(lua_State *L);
#define current_device moonal_current_device
device_t current_device(lua_State *L);

/* tracing.c */
#define trace_objects moonal_trace_objects
extern int trace_objects;

/* structs.c */
#define checkfloat3 moonal_checkfloat3
int checkfloat3(lua_State *L, int arg, ALfloat dst[3]);
#define pushfloat3 moonal_pushfloat3
int pushfloat3(lua_State *L, const ALfloat dst[3]);
#define echeckdeviceattributeslist moonal_echeckdeviceattributeslist
ALCint *echeckdeviceattributeslist(lua_State *L, int arg, int *err);

/* main.c */
int luaopen_moonal(lua_State *L);
void moonal_utils_init(lua_State *L);
void moonal_open_tracing(lua_State *L);
void moonal_open_enums(lua_State *L);
int moonal_open_getproc(lua_State *L);
void moonal_atexit_getproc(void);

/*------------------------------------------------------------------------------*
 | Debug and other utilities                                                    |
 *------------------------------------------------------------------------------*/

#define TRACE_CREATE(p, ttt) do {                                               \
    if(trace_objects) { printf("create "ttt" %p\n", (void*)(uintptr_t)(p)); }   \
} while(0)

#define TRACE_DELETE(p, ttt) do {                                               \
    if(trace_objects) { printf("delete "ttt" %px\n", (void*)(uintptr_t)(p)); }  \
} while(0)


#define CheckErrorAlc(L, device_) do {                                      \
    ALCenum ec_ = alc.GetError(device_);                                    \
    if(ec_ != ALC_NO_ERROR) { pushalcerror(L, ec_); return lua_error(L); }  \
} while(0)

#define CheckErrorAl(L) do {                                                \
    ALenum ec_ = al.GetError();                                             \
    if(ec_ != AL_NO_ERROR) { pushalerror(L, ec_); return lua_error(L); }    \
} while(0)

#define CheckErrorRestoreAlc(L, device_, old_context_) do {                 \
    /* restores old_context_ before raising an error */                     \
    ALCenum ec_ = alc.GetError(device_);                                    \
    if(ec_ != ALC_NO_ERROR) {                                               \
        alc.MakeContextCurrent((old_context_));                             \
        pushalcerror(L, ec_); return lua_error(L);                          \
    }                                                                       \
} while(0)

#define CheckErrorRestoreAl(L, old_context_) do {                           \
    /* restores old_context_ before raising an error */                     \
    ALenum ec_ = al.GetError();                                             \
    if(ec_ != AL_NO_ERROR) {                                                \
        alc.MakeContextCurrent((old_context_));                             \
        pushalerror(L, ec_); return lua_error(L);                           \
    }                                                                       \
} while(0)


#define FlushErrorAlc(L, device_) do { (void)alc.GetError(device_);} while(0)
#define FlushErrorAl(L) do { (void)al.GetError();   } while(0)

/* If this is printed, it denotes a suspect bug: */
#define UNEXPECTED_ERROR "unexpected error (%s, %d)", __FILE__, __LINE__
#define unexpected(L) luaL_error((L), UNEXPECTED_ERROR)

#define notsupported(L) luaL_error((L), "operation not supported")

#define badvalue(L,s)   lua_pushfstring((L), "invalid value '%s'", (s))

#define erralparam(L, arg) luaL_argerror((L), (arg), "invalid AL param")
#define erralcparam(L, arg) luaL_argerror((L), (arg), "invalid ALC param")
#define errreadonlyparam(L, arg) luaL_argerror((L), (arg), "parameter is read only")

/* DEBUG -------------------------------------------------------- */
#if defined(DEBUG)

#define DBG printf
#define TR() do { printf("trace %s %d\n",__FILE__,__LINE__); } while(0)
#define BK() do { printf("break %s %d\n",__FILE__,__LINE__); getchar(); } while(0)
#define TSTART double ts = now();
#define TSTOP do {                                          \
    ts = since(ts); ts = ts*1e6;                            \
    printf("%s %d %.3f us\n", __FILE__, __LINE__, ts);      \
    ts = now();                                             \
} while(0);

#else 

#define DBG noprintf
#define TR()
#define BK()
#define TSTART do {} while(0) 
#define TSTOP do {} while(0)    

#endif /* DEBUG ------------------------------------------------- */


#endif /* internalDEFINED */
