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

int checkfloat3(lua_State *L, int arg, ALfloat dst[3])
    {
    int i, isnum;
    if(lua_type(L, arg) != LUA_TTABLE)
        return luaL_argerror(L, arg, errstring(ERR_TABLE));
    for(i = 0; i < 3; i++)
        {
        lua_rawgeti(L, arg, i+1);
        dst[i] = lua_tonumberx(L, -1, &isnum),
        lua_pop(L, 1);
        if(!isnum) 
            {
            lua_pushfstring(L, "invalid element #%d", i+1);
            return luaL_argerror(L, arg, lua_tostring(L, -1));
            }
        }
    return 0;
    }

int pushfloat3(lua_State *L, const ALfloat dst[3])
    {
    int i;
    lua_newtable(L);
    for(i = 0; i < 3; i++)
        {
        lua_pushnumber(L, dst[i]);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }




ALCint *echeckdeviceattributeslist(lua_State *L, int arg, int *err)
    {
    int i, isnum, t, nattr;
    ALCint *attr;
    *err = ERR_SUCCESS;
    if(lua_isnoneornil(L, arg)) 
        {
        *err = ERR_NOTPRESENT;
        lua_pushstring(L, errstring(*err));
        return NULL;
        }
    if(lua_type(L, arg) != LUA_TTABLE)
        {
        *err = ERR_TABLE;
        lua_pushstring(L, errstring(*err));
        return NULL;
        }

    nattr = tablelen(L, arg);
    
    attr = (ALCint*)Malloc(L, ((nattr*2)+1)*sizeof(ALCint));
    i = 0;

#define GetInteger(c, s) do {                                   \
    t = lua_getfield(L, arg, (s));                              \
    if(t != LUA_TNIL && t !=LUA_TNONE)                          \
        {                                                       \
        attr[i++] = (c);                                        \
        attr[i++] = lua_tointegerx(L, -1, &isnum);              \
        if(!isnum)                                              \
            {                                                   \
            lua_pop(L, 1);                                      \
            lua_pushfstring(L, "field '%s'", (s));              \
            *err = ERR_TYPE;                                    \
            Free(L, attr);                                      \
            return NULL;                                        \
            }                                                   \
        }                                                       \
    lua_pop(L, 1);                                              \
} while(0)

#define GetBoolean(c, s) do {                                   \
    t = lua_getfield(L, arg, (s));                              \
    if(t != LUA_TNIL && t !=LUA_TNONE)                          \
        {                                                       \
        if(t != LUA_TBOOLEAN)                                   \
            {                                                   \
            lua_pop(L, 1);                                      \
            lua_pushfstring(L, "field '%s'", (s));              \
            *err = ERR_TYPE;                                    \
            Free(L, attr);                                      \
            return NULL;                                        \
            }                                                   \
        attr[i++] = (c);                                        \
        attr[i++] = lua_toboolean(L, -1);                       \
        }                                                       \
    lua_pop(L, 1);                                              \
} while(0)

#define GetEnum(c, s, testfunc) do {                            \
    t = lua_getfield(L, arg, (s));                              \
    if(t != LUA_TNIL && t !=LUA_TNONE)                          \
        {                                                       \
        attr[i++] = (c);                                        \
        attr[i++] = testfunc(L, -1, err);                       \
        if(*err)                                                \
            {                                                   \
            lua_pop(L, 1);                                      \
            lua_pushfstring(L, "field '%s'", (s));              \
            *err = ERR_TYPE;                                    \
            Free(L, attr);                                      \
            return NULL;                                        \
            }                                                   \
        }                                                       \
    lua_pop(L, 1);                                              \
} while(0)


    GetEnum(ALC_FORMAT_TYPE_SOFT, "format_type", testtypesoft);
    GetEnum(ALC_FORMAT_CHANNELS_SOFT, "format_channels", testchannels);
    GetInteger(ALC_FREQUENCY, "frequency");
    GetInteger(ALC_REFRESH, "refresh");
    GetInteger(ALC_MONO_SOURCES, "mono_sources");
    GetInteger(ALC_STEREO_SOURCES, "stereo_sources");
    GetInteger(ALC_MAX_AUXILIARY_SENDS, "max_auxiliary_sends");
    GetBoolean(ALC_SYNC, "sync");
    GetBoolean(ALC_HRTF_SOFT, "hrtf");
    GetInteger(ALC_HRTF_ID_SOFT, "hrtf_id");
    GetBoolean(ALC_OUTPUT_LIMITER_SOFT, "output_limiter");
#if 0 
/* AMBISONIC is not defined in public headers (see Alc/ALc.c, OpenAL32/Include/alMain.h) */
ALC_AMBISONIC_LAYOUT_SOFT  "ambisonic layout"
ALC_AMBISONIC_SCALING_SOFT "ambisonic scaling"
ALC_AMBISONIC_ORDER_SOFT   "ambisonic order"
#endif

    attr[i] = 0;    
    return attr;
#undef GetInteger
#undef GetBoolean
#undef GetEnum
    }

#if 0
int pushdeviceattributeslist(lua_State *L, ALCint *attr)
    {
    int i = 0;
    lua_newtable(L);
    while(attr[i]!=0)
        {
        switch(attr[i])
            {
#define SetInteger(s) do {  lua_pushinteger(L, attr[i+1]); lua_setfield(L, -2, s); } while(0)
#define SetBoolean(s) do {  lua_pushboolean(L, attr[i+1]); lua_setfield(L, -2, s); } while(0)
#define SetEnum(s, pushfunc) do {  pushfunc(L, attr[i+1]); lua_setfield(L, -2, s); } while(0)
            case ALC_FREQUENCY: SetInteger("frequency"); break;
            case ALC_REFRESH: SetInteger("refresh"); break;
            case ALC_MONO_SOURCES: SetInteger("mono_sources"); break;
            case ALC_STEREO_SOURCES: SetInteger("stereo_sources"); break;
            case ALC_MAX_AUXILIARY_SENDS: SetInteger("max_auxiliary_sends"); break;
            case ALC_SYNC: SetBoolean("sync"); break;
            case ALC_HRTF_SOFT: SetBoolean("hrtf"); break;
            case ALC_HRTF_ID_SOFT: SetBoolean("hrtf_id"); break;
            case ALC_OUTPUT_LIMITER_SOFT: SetBoolean("output_limiter"); break;
            case ALC_FORMAT_TYPE_SOFT: SetEnum("format_type", pushtypesoft);
            case ALC_FORMAT_CHANNELS_SOFT: SetEnum("format_channels", pushchannels);
            default:
                //printf("unknown attribute 0x%.4x, val = %d\n", attr[i], attr[i+1]);
                lua_pushinteger(L, attr[i+1]); lua_rawseti(L, -2, attr[i]); // extension attributes
#undef SetInteger
#undef SetBoolean
#undef SetEnum
            }
        i+=2;
        }
    return 1;
    }
#endif

