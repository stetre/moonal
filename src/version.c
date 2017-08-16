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

static int AddVersions(lua_State *L)
    {
    lua_pushstring(L, "_VERSION");
    lua_pushstring(L, "MoonAL "MOONAL_VERSION);
    lua_settable(L, -3);
    return 0;
    }

static int GetAlcVersion(lua_State *L)
    {
    ALCint major, minor;
    context_t context = current_context(L);
    device_t device = userdata(context)->device;
    alc.GetIntegerv(device, ALC_MAJOR_VERSION, 1, &major);
    CheckErrorAlc(L, device);
    alc.GetIntegerv(device, ALC_MINOR_VERSION, 1, &minor);
    CheckErrorAlc(L, device);
    lua_pushfstring(L, "ALC %d.%d", major, minor);
    return 1;
    }

static int GetEfxVersion(lua_State *L)
    {
    ALCint major, minor;
    context_t context = current_context(L);
    device_t device = userdata(context)->device;
    alc.GetIntegerv(device, ALC_EFX_MAJOR_VERSION, 1, &major);
    CheckErrorAlc(L, device);
    alc.GetIntegerv(device, ALC_EFX_MINOR_VERSION, 1, &minor);
    CheckErrorAlc(L, device);
    lua_pushfstring(L, "EFX %d.%d", major, minor);
    return 1;
    }

static int AlGetString(lua_State *L, ALenum pname)
    {
    const ALchar* val;
    (void)current_context(L);
    val = al.GetString(pname);
    CheckErrorAl(L);
    lua_pushstring(L, val);
    return 1;
    }

static int GetVendor(lua_State *L)
    { 
    return AlGetString(L, AL_VENDOR);
    }

static int GetVersion(lua_State *L)
    { 
    return AlGetString(L, AL_VERSION); 
    }

static int GetRenderer(lua_State *L)
    { 
    return AlGetString(L, AL_RENDERER);
    }

static const struct luaL_Reg Functions[] = 
    {
        { "get_alc_version", GetAlcVersion },
        { "get_efx_version", GetEfxVersion },
        { "get_vendor", GetVendor },
        { "get_version", GetVersion },
        { "get_renderer", GetRenderer },
        { NULL, NULL } /* sentinel */
    };

void moonal_open_version(lua_State *L)
    {
    AddVersions(L);
    luaL_setfuncs(L, Functions, 0);
    }

