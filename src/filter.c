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

static int freefilter(lua_State *L, ud_t *ud)
    {
    filter_t filter = (filter_t)ud->handle;
    LPALDELETEFILTERS DeleteFilters = ud->ddt->DeleteFilters;
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(filter, "filter");
    DeleteFilters(1, &filter->name);
    Free(L, filter);
    CheckErrorAl(L); 
    return 0;
    }


static int Create(lua_State *L)
    {
    ALuint name;
    filter_t filter;
    ud_t *ud, *context_ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &context_ud);
    
    CheckDevicePfn(L, context_ud, GenFilters);

    context_ud->ddt->GenFilters(1, &name);
    CheckErrorRestoreAl(L, old_context);

    filter = (object_t*)MallocNoErr(L, sizeof(object_t));
    if(!filter)
        {
        make_context_current(L, old_context);
        context_ud->ddt->DeleteFilters(1, &name);
        return luaL_error(L, errstring(ERR_MEMORY));
        }
    
    filter->name = name;
    ud = newuserdata(L, filter, FILTER_MT);
    ud->context = context;
    ud->device = context_ud->device;
    ud->parent_ud = context_ud;
    ud->destructor = freefilter;
    ud->ddt = context_ud->ddt;
    ud->cdt = context_ud->cdt;
    TRACE_CREATE(filter, "filter");
    make_context_current(L, old_context);
    return 1;
    }
    
static int GetFloat(lua_State *L, filter_t filter, ALenum param)
    {
    ALfloat val;
    ud_t *ud = userdata(filter);
    CheckDevicePfn(L, ud, GetFilterf);
    ud->ddt->GetFilterf(filter->name, param, &val);
    CheckErrorAl(L);
    lua_pushnumber(L, val);
    return 1;
    }

static int SetFloat(lua_State *L, filter_t filter, ALenum param)
    {
    ALfloat val;
    ud_t *ud = userdata(filter);
    CheckDevicePfn(L, ud, Filterf);
    val = luaL_checknumber(L, 3);
    ud->ddt->Filterf(filter->name, param, val);
    CheckErrorAl(L);
    return 0;
    }


static ALenum FilterType(lua_State *L, filter_t filter)
    {
    ALenum val;
    ud_t *ud = userdata(filter);
    CheckDevicePfn(L, ud, GetFilteri);
    ud->ddt->GetFilteri(filter->name, AL_FILTER_TYPE, &val);
    CheckErrorAl(L);
    return val;
    }


static int GetFilterType(lua_State *L)
    {
    ALenum val;
    ud_t *ud;
    filter_t filter = checkfilter(L, 1, &ud);
    CheckDevicePfn(L, ud, GetFilteri);
    ud->ddt->GetFilteri(filter->name, AL_FILTER_TYPE, &val);
    CheckErrorAl(L);
    pushfiltertype(L, val);
    return 1;
    }

static int SetFilterType(lua_State *L)
    {
    ud_t *ud;
    filter_t filter = checkfilter(L, 1, &ud);
    ALenum val = checkfiltertype(L, 2);
    CheckDevicePfn(L, ud, Filteri);
    ud->ddt->Filteri(filter->name, AL_FILTER_TYPE, val);
    CheckErrorAl(L);
    return 0;
    }


static int GetLowpass(lua_State *L, filter_t filter)
    {
    ALenum param = checklowpassparam(L, 2);
    switch(param)
        {
        case AL_LOWPASS_GAIN:
        case AL_LOWPASS_GAINHF:
                            return GetFloat(L, filter, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetLowpass(lua_State *L, filter_t filter)
    {
    ALenum param = checklowpassparam(L, 2);
    switch(param)
        {
        case AL_LOWPASS_GAIN:
        case AL_LOWPASS_GAINHF:
                            return SetFloat(L, filter, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int GetHighpass(lua_State *L, filter_t filter)
    {
    ALenum param = checkhighpassparam(L, 2);
    switch(param)
        {
        case AL_HIGHPASS_GAIN:
        case AL_HIGHPASS_GAINLF:
                            return GetFloat(L, filter, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetHighpass(lua_State *L, filter_t filter)
    {
    ALenum param = checkhighpassparam(L, 2);
    switch(param)
        {
        case AL_HIGHPASS_GAIN:
        case AL_HIGHPASS_GAINLF:
                            return SetFloat(L, filter, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetBandpass(lua_State *L, filter_t filter)
    {
    ALenum param = checkbandpassparam(L, 2);
    switch(param)
        {
        case AL_BANDPASS_GAIN:
        case AL_BANDPASS_GAINHF:
        case AL_BANDPASS_GAINLF:
                            return GetFloat(L, filter, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetBandpass(lua_State *L, filter_t filter)
    {
    ALenum param = checkbandpassparam(L, 2);
    switch(param)
        {
        case AL_BANDPASS_GAIN:
        case AL_BANDPASS_GAINHF:
        case AL_BANDPASS_GAINLF:
                            return SetFloat(L, filter, param);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }


static int GetFilter(lua_State *L)
    {
    filter_t filter = checkfilter(L, 1, NULL);
    ALenum filter_type = FilterType(L, filter);

    switch(filter_type)
        {
        case AL_FILTER_LOWPASS:     return GetLowpass(L, filter);
        case AL_FILTER_HIGHPASS:    return GetHighpass(L, filter);
        case AL_FILTER_BANDPASS:    return GetBandpass(L, filter);
        case AL_FILTER_NULL:
        default:
            return luaL_argerror(L, 1, "undefined filter type");
        }
    return 0;
    }

static int SetFilter(lua_State *L)
    {
    filter_t filter = checkfilter(L, 1, NULL);
    ALenum filter_type = FilterType(L, filter);

    switch(filter_type)
        {
        case AL_FILTER_LOWPASS:     return SetLowpass(L, filter);
        case AL_FILTER_HIGHPASS:    return SetHighpass(L, filter);
        case AL_FILTER_BANDPASS:    return SetBandpass(L, filter);
        case AL_FILTER_NULL:
        default:
            return luaL_argerror(L, 1, "undefined filter type");
        }

    return 0;
    }


RAW_FUNC(filter)
TYPE_FUNC(filter)
PARENT_FUNC(filter)
DELETE_FUNC(filter)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "delete", Delete },
        { "get", GetFilter },
        { "set", SetFilter },
        { "get_type", GetFilterType },
        { "set_type", SetFilterType },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_filter", Create},
        { "delete_filter", Delete },
        { "filter_get", GetFilter },
        { "filter_set", SetFilter },
        { "get_filter_type", GetFilterType },
        { "set_filter_type", SetFilterType },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_filter(lua_State *L)
    {
    udata_define(L, FILTER_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

