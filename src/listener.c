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

static int freelistener(lua_State *L, ud_t *ud)
    {
    listener_t listener = (listener_t)ud->handle;
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(listener, "listener");
    Free(L, listener);
    return 0;
    }

int pushlistener(lua_State *L, context_t context)
    {
    ud_t *ud;
    listener_t listener;
    ud_t *context_ud = userdata(context);

    if(context_ud->listener) 
        return pushxxx(L, context_ud->listener);

    /* create listener */
    listener = (object_t*)MallocNoErr(L, sizeof(object_t));
    if(!listener)
        return luaL_error(L, errstring(ERR_MEMORY));
    
    listener->name = 0;
    ud = newuserdata(L, listener, LISTENER_MT);
    ud->context = context;
    ud->device = context_ud->device;
    ud->parent_ud = context_ud;
    ud->destructor = freelistener;
    ud->ddt = context_ud->ddt;
    ud->cdt = context_ud->cdt;
    context_ud->listener = listener;
    TRACE_CREATE(listener, "listener");
    return 1;
    }
    

static int GetFloat(lua_State *L, ALenum param)
    {
    ALfloat val;
    al.GetListenerf(param, &val);
    lua_pushnumber(L, val);
    return 1;
    }

static int SetFloat(lua_State *L, ALenum param)
    {
    ALfloat val = luaL_checknumber(L, 3);
    al.Listenerf(param, val);
    return 0;
    }

static int GetFloat3(lua_State *L, ALenum param)
    {
    ALfloat val[3];
    al.GetListenerfv(param, val);
    pushfloat3(L, val);
    return 1;
    }

static int SetFloat3(lua_State *L, ALenum param)
    {
    ALfloat val[3];
    checkfloat3(L, 3, val);
    al.Listenerfv(param, val);
    return 0;
    }

static int GetOrientation(lua_State *L)
    {
    ALfloat val[6];
    al.GetListenerfv(AL_ORIENTATION, val);
    pushfloat3(L, val);  /* "at" */
    pushfloat3(L, &val[3]); /* "up" */
    return 2;
    }


static int SetOrientation(lua_State *L)
    {
    ALfloat val[6];
    checkfloat3(L, 3, val); /* "at" */
    checkfloat3(L, 4, &val[3]); /* "up" */
    al.Listenerfv(AL_ORIENTATION, val);
    return 0;
    }

static int SetListener(lua_State *L)
    {
    int res;
    ud_t *ud;
    context_t old_context = current_context(L);
    listener_t listener = checklistener(L, 1, &ud);
    ALenum param = checkalparam(L, 2);
    (void)listener;
    make_context_current(L, ud->context);
    switch(param)
        {
        case AL_GAIN:
        case AL_METERS_PER_UNIT: res = SetFloat(L, param); break;
        case AL_POSITION:
        case AL_VELOCITY: res = SetFloat3(L, param); break;
        case AL_ORIENTATION: res = SetOrientation(L); break;
        default:
            make_context_current(L, old_context);
            return luaL_argerror(L, 1, "invalid AL parameter");
        }
    make_context_current(L, old_context);
    CheckErrorAl(L);
    return res;
    }

static int GetListener(lua_State *L)
    {
    int res;
    ud_t *ud;
    context_t old_context = current_context(L);
    listener_t listener = checklistener(L, 1, &ud);
    ALenum param = checkalparam(L, 2);
    (void)listener;
    make_context_current(L, ud->context);
    switch(param)
        {
        case AL_GAIN:
        case AL_METERS_PER_UNIT: res = GetFloat(L, param); break;
        case AL_POSITION:
        case AL_VELOCITY: res = GetFloat3(L, param); break;
        case AL_ORIENTATION: res = GetOrientation(L); break;
        default:
            make_context_current(L, old_context);
            return luaL_argerror(L, 1, "invalid AL parameter");
        }
    make_context_current(L, old_context);
    CheckErrorAl(L);
    return res;
    }


RAW_FUNC(listener)
TYPE_FUNC(listener)
PARENT_FUNC(listener)
DELETE_FUNC(listener)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "delete", Delete },
        { "get", GetListener },
        { "set", SetListener },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "listener_get", GetListener },
        { "listener_set", SetListener },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_listener(lua_State *L)
    {
    udata_define(L, LISTENER_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

