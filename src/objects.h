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

#ifndef objectsDEFINED
#define objectsDEFINED

#include "tree.h"
#include "udata.h"


/*------------------------------------------------------*/

/* Objects' metatable names */
#define CONTEXT_MT "moonal_context"
#define DEVICE_MT "moonal_device"
#define BUFFER_MT "moonal_buffer"
#define LISTENER_MT "moonal_listener"
#define SOURCE_MT "moonal_source"
#define EFFECT_MT "moonal_effect"
#define FILTER_MT "moonal_filter"
#define AUXSLOT_MT "moonal_auxslot"

/* Userdata memory associated with objects */
#define ud_t moonal_ud_t
typedef struct moonal_ud_s ud_t;

struct moonal_ud_s {
    void *handle; /* the object handle bound to this userdata */
    int (*destructor)(lua_State *L, ud_t *ud);  /* self destructor */
    device_t device;
    context_t context;
    ud_t *parent_ud; /* the ud of the parent object */
    device_dt_t *ddt; /* dispatch table */
    context_dt_t *cdt; /* dispatch table */
    listener_t listener; /* context only */
    uint32_t marks;
    void *info; /* object specific info (ud_info_t, subject to Free() at destruction, if not NULL) */
};
    
/* Marks.  m_ = marks word (uint32_t) , i_ = bit number (0 .. 31)  */
#define MarkGet(m_,i_)  (((m_) & ((uint32_t)1<<(i_))) == ((uint32_t)1<<(i_)))
#define MarkSet(m_,i_)  do { (m_) = ((m_) | ((uint32_t)1<<(i_))); } while(0)
#define MarkReset(m_,i_) do { (m_) = ((m_) & (~((uint32_t)1<<(i_)))); } while(0)

#define IsValid(ud)             MarkGet((ud)->marks, 0)
#define MarkValid(ud)           MarkSet((ud)->marks, 0) 
#define CancelValid(ud)         MarkReset((ud)->marks, 0)

#define IsCaptureDevice(ud)     MarkGet((ud)->marks, 1)
#define MarkCaptureDevice(ud)   MarkSet((ud)->marks, 1) 
#define CancelCaptureDevice(ud) MarkReset((ud)->marks, 1)

#define IsLoopbackDevice(ud)     MarkGet((ud)->marks, 2)
#define MarkLoopbackDevice(ud)   MarkSet((ud)->marks, 2) 
#define CancelLoopbackDevice(ud) MarkReset((ud)->marks, 2)

#if 0
/* .c */
#define  moonal_
#endif

#define newuserdata moonal_newuserdata
ud_t *newuserdata(lua_State *L, void *handle, const char *mt);
#define freeuserdata moonal_freeuserdata
int freeuserdata(lua_State *L, ud_t *ud);
#define pushuserdata moonal_pushuserdata 
int pushuserdata(lua_State *L, ud_t *ud);

#define userdata_unref(L, handle) udata_unref((L),(handle))

#define UD(handle) userdata((handle)) /* dispatchable objects only */
#define userdata moonal_userdata
ud_t *userdata(void *handle);
#define testxxx moonal_testxxx
void *testxxx(lua_State *L, int arg, ud_t **udp, const char *mt);
#define checkxxx moonal_checkxxx
void *checkxxx(lua_State *L, int arg, ud_t **udp, const char *mt);
#define pushxxx moonal_pushxxx
int pushxxx(lua_State *L, void *handle);
#define checkxxxlist moonal_checkxxxlist
void** checkxxxlist(lua_State *L, int arg, uint32_t *count, int *err, const char *mt);
#define objectnamelist moonal_objectnamelist
ALuint *objectnamelist(lua_State *L, object_t **list, uint32_t count, int *err);
#define objectsearchxxx moonal_objectsearchxxx
void *objectsearchxxx(lua_State *L, ALuint name, ud_t **udp, const char *mt);

#define freechildren moonal_freechildren
int freechildren(lua_State *L,  const char *mt, ud_t *parent_ud);

/* device.c */
#define checkdevice(L, arg, udp) (device_t)checkxxx((L), (arg), (udp), DEVICE_MT)
#define testdevice(L, arg, udp) (device_t)testxxx((L), (arg), (udp), DEVICE_MT)
#define pushdevice(L, handle) pushxxx((L), (handle))
#define checkdevicelist(L, arg, count, err) (device_t*)checkxxxlist((L), (arg), (count), (err), DEVICE_MT)

/* context.c */
#define checkcontext(L, arg, udp) (context_t)checkxxx((L), (arg), (udp), CONTEXT_MT)
#define testcontext(L, arg, udp) (context_t)testxxx((L), (arg), (udp), CONTEXT_MT)
#define pushcontext(L, handle) pushxxx((L), (handle))

/* buffer.c */
#define checkbuffer(L, arg, udp) (buffer_t)checkxxx((L), (arg), (udp), BUFFER_MT)
#define testbuffer(L, arg, udp) (buffer_t)testxxx((L), (arg), (udp), BUFFER_MT)
#define pushbuffer(L, handle) pushxxx((L), (handle))
#define checkbufferlist(L, arg, count, err) (buffer_t*)checkxxxlist((L), (arg), (count), (err), BUFFER_MT)
#define searchbuffer(L, name, udp) (buffer_t)objectsearchxxx((L), (name), (udp), BUFFER_MT)

/* listener.c */
#define checklistener(L, arg, udp) (listener_t)checkxxx((L), (arg), (udp), LISTENER_MT)
#define testlistener(L, arg, udp) (listener_t)testxxx((L), (arg), (udp), LISTENER_MT)
#define pushlistener moonal_pushlistener
int pushlistener(lua_State *L, context_t context);
#define checklistenerlist(L, arg, count, err) (listener_t*)checkxxxlist((L), (arg), (count), (err), LISTENER_MT)

/* source.c */
#define checksource(L, arg, udp) (source_t)checkxxx((L), (arg), (udp), SOURCE_MT)
#define testsource(L, arg, udp) (source_t)testxxx((L), (arg), (udp), SOURCE_MT)
#define pushsource(L, handle) pushxxx((L), (handle))
#define checksourcelist(L, arg, count, err) (source_t*)checkxxxlist((L), (arg), (count), (err), SOURCE_MT)

/* effect.c */
#define checkeffect(L, arg, udp) (effect_t)checkxxx((L), (arg), (udp), EFFECT_MT)
#define testeffect(L, arg, udp) (effect_t)testxxx((L), (arg), (udp), EFFECT_MT)
#define pusheffect(L, handle) pushxxx((L), (handle))
#define checkeffectlist(L, arg, count, err) (effect_t*)checkxxxlist((L), (arg), (count), (err), EFFECT_MT)
#define searcheffect(L, name, udp) (effect_t)objectsearchxxx((L), (name), (udp), EFFECT_MT)

/* filter.c */
#define checkfilter(L, arg, udp) (filter_t)checkxxx((L), (arg), (udp), FILTER_MT)
#define testfilter(L, arg, udp) (filter_t)testxxx((L), (arg), (udp), FILTER_MT)
#define pushfilter(L, handle) pushxxx((L), (handle))
#define searchfilter(L, name, udp) (filter_t)objectsearchxxx((L), (name), (udp), FILTER_MT)
#define checkfilterlist(L, arg, count, err) (filter_t*)checkxxxlist((L), (arg), (count), (err), FILTER_MT)

/* auxslot.c */
#define checkauxslot(L, arg, udp) (auxslot_t)checkxxx((L), (arg), (udp), AUXSLOT_MT)
#define testauxslot(L, arg, udp) (auxslot_t)testxxx((L), (arg), (udp), AUXSLOT_MT)
#define pushauxslot(L, handle) pushxxx((L), (handle))
#define checkauxslotlist(L, arg, count, err) (auxslot_t*)checkxxxlist((L), (arg), (count), (err), AUXSLOT_MT)
#define searchauxslot(L, name, udp) (auxslot_t)objectsearchxxx((L), (name), (udp), AUXSLOT_MT)

#if 0 /* scaffolding 6yy */
/* zzz.c */
#define checkzzz(L, arg, udp) (zzz_t)checkxxx((L), (arg), (udp), ZZZ_MT)
#define testzzz(L, arg, udp) (zzz_t)testxxx((L), (arg), (udp), ZZZ_MT)
#define pushzzz(L, handle) pushxxx((L), (handle))
#define checkzzzlist(L, arg, count, err) (zzz_t*)checkxxxlist((L), (arg), (count), (err), ZZZ_MT)
#define searchzzz(L, name, udp) (zzz_t)objectsearchxxx((L), (name), (udp), ZZZ_MT)

#endif

/* used in main.c */
void moonal_open_version(lua_State *L);
void moonal_open_listener(lua_State *L);
void moonal_open_context(lua_State *L);
void moonal_open_device(lua_State *L);
void moonal_open_buffer(lua_State *L);
void moonal_open_source(lua_State *L);
void moonal_open_effect(lua_State *L);
void moonal_open_filter(lua_State *L);
void moonal_open_auxslot(lua_State *L);
void moonal_open_datahandling(lua_State *L);
void moonal_open_ranges(lua_State *L);

#define RAW_FUNC(xxx)                       \
static int Raw(lua_State *L)                \
    {                                       \
    lua_pushinteger(L, (uintptr_t)check##xxx(L, 1, NULL));  \
    return 1;                               \
    }

#define TYPE_FUNC(xxx) /* NONCL */          \
static int Type(lua_State *L)               \
    {                                       \
    (void)check##xxx(L, 1, NULL);           \
    lua_pushstring(L, ""#xxx);              \
    return 1;                               \
    }

#define DELETE_FUNC(xxx)                    \
static int Delete(lua_State *L)             \
    {                                       \
    ud_t *ud;                               \
    (void)test##xxx(L, 1, &ud);             \
    if(!ud) return 0; /* already deleted */ \
    return ud->destructor(L, ud);           \
    }

#define PARENT_FUNC(xxx)                    \
static int Parent(lua_State *L)             \
    {                                       \
    ud_t *ud;                               \
    (void)check##xxx(L, 1, &ud);            \
    if(!ud->parent_ud) return 0;            \
    return pushuserdata(L, ud->parent_ud);  \
    }

#define CONTEXT_FUNC(xxx)                   \
static int Context(lua_State *L)            \
    {                                       \
    ud_t *ud;                               \
    (void)check##xxx(L, 1, &ud);            \
    if(!ud->context) return 0;              \
    return pushcontext(L, ud->context);     \
    }

#define DEVICE_FUNC(xxx)                    \
static int Device(lua_State *L)             \
    {                                       \
    ud_t *ud;                               \
    (void)check##xxx(L, 1, &ud);            \
    if(!ud->device) return 0;               \
    return pushdevice(L, ud->device);       \
    }

#endif /* objectsDEFINED */
