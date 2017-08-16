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

static int freebuffer(lua_State *L, ud_t *ud)
    {
    buffer_t buffer = (buffer_t)ud->handle;
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(buffer, "buffer");
    al.DeleteBuffers(1, &buffer->name);
    Free(L, buffer);
    CheckErrorAl(L);
    return 0;
    }

static int Create(lua_State *L)
    {
    ALuint name;
    buffer_t buffer;
    ud_t *ud, *context_ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &context_ud);
    
    al.GenBuffers(1, &name);
    CheckErrorRestoreAl(L, old_context);

    buffer = (object_t*)MallocNoErr(L, sizeof(object_t));
    if(!buffer)
        {
        make_context_current(L, old_context);
        al.DeleteBuffers(1, &name);
        return luaL_error(L, errstring(ERR_MEMORY));
        }
    
    buffer->name = name;
    
    ud = newuserdata(L, buffer, BUFFER_MT);
    ud->context = context;
    ud->device = context_ud->device;
    ud->parent_ud = context_ud;
    ud->ddt = context_ud->ddt;
    ud->cdt = context_ud->cdt;
    ud->destructor = freebuffer;
    TRACE_CREATE(buffer, "buffer");
    make_context_current(L, old_context);
    return 1;
    }

static int BufferData(lua_State *L)
    {
    size_t size;
    ud_t *ud;
    buffer_t buffer = checkbuffer(L, 1, &ud);
    ALenum format = checkformat(L, 2);
    const char* data = luaL_checklstring(L, 3, &size);
    ALsizei freq = luaL_checkinteger(L, 4);
    al.BufferData(buffer->name, format, data, size, freq);
    CheckErrorAl(L);
    return 0;
    }


#if 0 
/* Note: AL_SOFT_buffer_samples and AL_SOFT_buffer_sub_data are removed since v 1.17.2 */
//ALvoid al.BufferSubDataSOFT(ALuint buffer,ALenum format,const ALvoid *data,ALsizei offset,ALsizei length);
static int BufferSubData(lua_State *L) //NO: extension removed
    {
    ud_t *ud;
    buffer_t buffer = checkbuffer(L, 1, &ud);
    (void)buffer;
    CheckContextPfn(L, ud, BufferSubDataSOFT);
    //ud->cdt->BufferSubDataSOFT()
    CheckErrorAl(L);
    return 0;
    }

//void al.BufferSamplesSOFT(ALuint buffer, ALuint samplerate, ALenum internalformat, ALsizei samples, ALenum channels, ALenum type, const ALvoid *data);
static int BufferSamples(lua_State *L) //NO: extension removed
    {
    ud_t *ud;
    buffer_t buffer = checkbuffer(L, 1, &ud);
    (void)buffer;
    CheckContextPfn(L, ud, BufferSamplesSOFT);
    //ud->cdt->BufferSamplesSOFT()
    CheckErrorAl(L);
    return 0;
    }

//void alBufferSubSamplesSOFT(ALuint buffer, ALsizei offset, ALsizei samples, ALenum channels, ALenum type, const ALvoid *data);
static int BufferSubSamples(lua_State *L) //NO: extension removed
    {
    ud_t *ud;
    buffer_t buffer = checkbuffer(L, 1, &ud);
    (void)buffer;
    CheckContextPfn(L, ud, BufferSubSamplesSOFT);
    ud->cdt->BufferSubSamplesSOFT()
    CheckErrorAl(L);
    return 0;
    }

//void alGetBufferSamplesSOFT(ALuint buffer, ALsizei offset, ALsizei samples, ALenum channels, ALenum type, ALvoid *data);
static int GetBufferSamples(lua_State *L) //NO: extension removed
    {
    ud_t *ud;
    buffer_t buffer = checkbuffer(L, 1, &ud);
    (void)buffer;
    CheckContextPfn(L, ud, GetBufferSamplesSOFT);
    ud->cdt->GetBufferSamplesSOFT()
    CheckErrorAl(L);
    return 0;
    }

static int IsBufferFormatSupported(lua_State *L) //NO: extension removed
    {
    ALboolean res;
    context_t context = current_context(L);
    ud_t *ud = userdata(context);
    ALenum format = checkformat(L, 1);
    CheckContextPfn(L, ud, IsBufferFormatSupportedSOFT);
    res = ud->cdt->IsBufferFormatSupportedSOFT(format);
    CheckErrorAl(L);
    lua_pushboolean(L, res);
    return 1;
    }

ALvoid alBufferDataStatic(const ALint buffer, ALenum format, ALvoid *data, ALsizei len, ALsizei freq);
static int BufferDataStatic(lua_State *L) //NO: AL_EXT_STATIC_BUFFER not supported by OpenAL Soft
    {
    return 1;
    }

#endif

static int GetInt(lua_State *L, buffer_t buffer, ALenum param)
    {
    ALint val;
    al.GetBufferi(buffer->name, param, &val);
    CheckErrorAl(L);
    lua_pushinteger(L, val);
    return 1;
    }

static int SetInt(lua_State *L, buffer_t buffer, ALenum param)
    {
    ALint val = luaL_checkinteger(L, 3);
    al.Bufferi(buffer->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

#if 0
static int GetFloat(lua_State *L, buffer_t buffer, ALenum param)
    {
    ALfloat val;
    al.GetBufferf(buffer->name, param, &val);
    CheckErrorAl(L);
    lua_pushnumber(L, val);
    return 1;
    }
#endif

static int GetInt2(lua_State *L, buffer_t buffer, ALenum param)
    {
    ALint val[2];
    al.GetBufferiv(buffer->name, param, val);
    CheckErrorAl(L);
    lua_pushinteger(L, val[0]);
    lua_pushinteger(L, val[1]);
    return 2;
    }

static int SetInt2(lua_State *L, buffer_t buffer, ALenum param)
    {
    ALint val[2];
    val[0] = luaL_checkinteger(L, 3);
    val[1] = luaL_checkinteger(L, 4);
    al.Bufferiv(buffer->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

#if 0
#define GET_ENUM_FUNC(FuncName, param, pushfunc)    \
static int FuncName(lua_State *L, buffer_t buffer)  \
    {                                               \
    ALint val;                                      \
    al.GetBufferi(buffer->name, (param), &val);     \
    CheckErrorAl(L);                                \
    pushfunc(L, val);                               \
    return 1;                                       \
    }

GET_ENUM_FUNC(GetInternalFormat, AL_INTERNAL_FORMAT_SOFT, pushinternalformat)
#endif


static int GetBuffer(lua_State *L)
    {
    buffer_t buffer = checkbuffer(L, 1, NULL);
    ALenum param = checkalparam(L, 2);
    switch(param)
        {
        case AL_FREQUENCY: return GetInt(L, buffer, param);
        case AL_BITS: return GetInt(L, buffer, param);
        case AL_CHANNELS: return GetInt(L, buffer, param);
        case AL_SIZE: return GetInt(L, buffer, param);
        case AL_UNPACK_BLOCK_ALIGNMENT_SOFT: return GetInt(L, buffer, param);
        case AL_PACK_BLOCK_ALIGNMENT_SOFT: return GetInt(L, buffer, param);
        case AL_LOOP_POINTS_SOFT: return GetInt2(L, buffer, param);
#if 0 // AL_SOFT_buffer_samples
        case AL_BYTE_LENGTH_SOFT: return GetInt(L, buffer, param);
        case AL_SAMPLE_LENGTH_SOFT: return GetInt(L, buffer, param);
        case AL_INTERNAL_FORMAT_SOFT: return GetInternalFormat(L, buffer);
        case AL_SEC_LENGTH_SOFT: return GetFloat(L, buffer, param);
#endif
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetBuffer(lua_State *L)
    {
    buffer_t buffer = checkbuffer(L, 1, NULL);
    ALenum param = checkalparam(L, 2);
    switch(param)
        {
        case AL_UNPACK_BLOCK_ALIGNMENT_SOFT:
        case AL_PACK_BLOCK_ALIGNMENT_SOFT:
                            return SetInt(L, buffer, param);
        case AL_LOOP_POINTS_SOFT:
                            return SetInt2(L, buffer, param);
        case AL_FREQUENCY:
        case AL_BITS:
        case AL_CHANNELS:
        case AL_SIZE:
                            return errreadonlyparam(L, 2);
#if 0 // AL_SOFT_buffer_samples
        case AL_SEC_LENGTH_SOFT:    return errreadonlyparam(L, 2);
        case AL_BYTE_LENGTH_SOFT:   return errreadonlyparam(L, 2);
        case AL_SAMPLE_LENGTH_SOFT: return errreadonlyparam(L, 2);
        case AL_INTERNAL_FORMAT_SOFT:   return errreadonlyparam(L, 2);
#endif
        default:
            return erralparam(L, 2);
        }
    return 0;
    }



RAW_FUNC(buffer)
TYPE_FUNC(buffer)
PARENT_FUNC(buffer)
DELETE_FUNC(buffer)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "delete", Delete },
        { "data", BufferData },
        { "get", GetBuffer },
        { "set", SetBuffer },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_buffer", Create},
        { "delete_buffer", Delete },
        { "buffer_data", BufferData },
//      { "buffer_sub_data", BufferSubData },
//      { "buffer_samples", BufferSamples },
//      { "buffer_sub_samples", BufferSubSamples },
//      { "get_buffer_samples", GetBufferSamples },
//      { "is_buffer_format_supported", IsBufferFormatSupported },
//      { "buffer_data_static", BufferDataStatic },
        { "buffer_get", GetBuffer },
        { "buffer_set", SetBuffer },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_buffer(lua_State *L)
    {
    udata_define(L, BUFFER_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

