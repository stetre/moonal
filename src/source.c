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

static int freesource(lua_State *L, ud_t *ud)
    {
    source_t source = (source_t)ud->handle;
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(source, "source");
    al.DeleteSources(1, &source->name);
    Free(L, source);
    CheckErrorAl(L); 
    return 0;
    }

static int Create(lua_State *L)
    {
    ALuint name;
    ud_t *ud, *context_ud;
    source_t source;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &context_ud);
    
    al.GenSources(1, &name);
    CheckErrorRestoreAl(L, old_context);

    source = (object_t*)MallocNoErr(L, sizeof(object_t));
    if(!source)
        {
        make_context_current(L, old_context);
        al.DeleteSources(1, &name);
        return luaL_error(L, errstring(ERR_MEMORY));
        }
    
    source->name = name;
    ud = newuserdata(L, source, SOURCE_MT);
    ud->context = context;
    ud->device = context_ud->device;
    ud->parent_ud = context_ud;
    ud->destructor = freesource;
    ud->ddt = context_ud->ddt;
    ud->cdt = context_ud->cdt;
    TRACE_CREATE(source, "source");
    make_context_current(L, old_context);
    return 1;
    }

static ALuint *CheckSources(lua_State *L, int arg, uint32_t *count, ud_t **ud)
    {
    int err;
    ALuint *names;
    source_t *sources = checksourcelist(L, arg, count, &err);
    if(err)
        { luaL_argerror(L, arg, errstring(err)); return NULL; }
    names = objectnamelist(L, sources, *count, &err);
    if(err)
        { Free(L, sources); luaL_argerror(L, arg, errstring(err)); return NULL; }
    if(ud) *ud = userdata(sources[1]);
    return names;
    }

static ALuint *CheckBuffers(lua_State *L, int arg, uint32_t *count, ud_t **ud)
    {
    int err;
    ALuint *names;
    buffer_t *buffers = checkbufferlist(L, arg, count, &err);
    if(err)
        { luaL_argerror(L, arg, errstring(err)); return NULL; }
    names = objectnamelist(L, buffers, *count, &err);
    if(err)
        { Free(L, buffers); luaL_argerror(L, arg, errstring(err)); return NULL; }
    if(ud) *ud = userdata(buffers[1]);
    return names;
    }

#define SOURCE_FUNC(what)                           \
static int Source##what(lua_State *L)               \
    {                                               \
    ud_t *ud;                                       \
    uint32_t count;                                 \
    ALuint *sources;                                \
    source_t source;                                \
    if(lua_istable(L, 1))                           \
        {                                           \
        sources = CheckSources(L, 1, &count, &ud);  \
        al.Source##what##v(count, sources);         \
        Free(L, sources);                           \
        CheckErrorAl(L);                            \
        return 0;                                   \
        }                                           \
    source = checksource(L, 1, &ud);                \
    al.Source##what(source->name);                  \
    CheckErrorAl(L);                                \
    return 0;                                       \
    }

SOURCE_FUNC(Play)
SOURCE_FUNC(Stop)
SOURCE_FUNC(Pause)
SOURCE_FUNC(Rewind)

#undef SOURCE_FUNC

static int SourceQueueBuffers(lua_State *L)
    {
    ud_t *ud;
    uint32_t count;
    source_t source = checksource(L, 1, &ud);
    ALuint *buffers = CheckBuffers(L, 2, &count, NULL);
    al.SourceQueueBuffers(source->name, count, buffers);
    CheckErrorAl(L);
    return 0;
    }

static int SourceUnqueueBuffers(lua_State *L)
    {
    ALenum ec;
    ud_t *ud;
    ALuint *buffers;
    buffer_t buffer;
    source_t source = checksource(L, 1, &ud);
    uint32_t i, count = luaL_checkinteger(L, 2);
    if(count == 0)
        return luaL_argerror(L, 2, errstring(ERR_VALUE));
    buffers = (ALuint*)Malloc(L, count * sizeof(ALuint));
    al.SourceUnqueueBuffers(source->name, count, buffers);
    ec = al.GetError(); 
    if(ec)
        {
        Free(L, buffers);
        lua_pushnil(L);
        pushalerror(L, ec);
        return 2;
        }
    else
        {
        lua_newtable(L);
        for(i=0; i< count; i++)
            { 
            /* search buffer from its name (this involves a scan of the whole udata
             * tree; we do not use the name as udata handle because due to the lack
             * of OpenAL documentation we don't know if names are unique across all
             * object types).
             */
            buffer = searchbuffer(L, buffers[i], NULL); 
            if(!buffer)
                {
                Free(L, buffers);
                return unexpected(L);
                }
            pushbuffer(L, buffer);
            lua_rawseti(L, -2, i+1);
            }
        Free(L, buffers);
        return 1;
        }
    CheckErrorAl(L);
    return 0;
    }

/*----- get/set -------------------------------------------------------------*/

static int GetInteger(lua_State *L, source_t source, ALenum param)
    {
    ALint val;
    al.GetSourcei(source->name, param, &val);
    CheckErrorAl(L);
    lua_pushinteger(L, val);
    return 1;
    }

static int GetBoolean(lua_State *L, source_t source, ALenum param)
    {
    ALint val;
    al.GetSourcei(source->name, param, &val);
    CheckErrorAl(L);
    lua_pushboolean(L, val);
    return 1;
    }

static int SetBoolean(lua_State *L, source_t source, ALenum param)
    {
    ALint val;
    val = checkboolean(L, 3);
    al.Sourcei(source->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

static int GetFloat(lua_State *L, source_t source, ALenum param)
    {
    ALfloat val;
    al.GetSourcef(source->name, param, &val);
    CheckErrorAl(L);
    lua_pushnumber(L, val);
    return 1;
    }

static int SetFloat(lua_State *L, source_t source, ALenum param)
    {
    ALfloat val;
    val = luaL_checknumber(L, 3);
    al.Sourcef(source->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

static int GetFloat3(lua_State *L, source_t source, ALenum param)
    {
    ALfloat val[3];
    al.GetSourcefv(source->name, param, val);
    CheckErrorAl(L);
    pushfloat3(L, val);
    return 1;
    }

static int SetFloat3(lua_State *L, source_t source, ALenum param)
    {
    ALfloat val[3];
    checkfloat3(L, 3, val);
    al.Sourcefv(source->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

static int GetDouble2(lua_State *L, source_t source, ALenum param)
    {
    ALdouble val[2];
    ud_t *ud = userdata(source);
    CheckContextPfn(L, ud, GetSourcedvSOFT);
    ud->cdt->GetSourcedvSOFT(source->name, param, val);
    CheckErrorAl(L);
    lua_pushnumber(L, val[0]);
    lua_pushnumber(L, val[1]);
    return 2;
    }

static int GetI64_2(lua_State *L, source_t source, ALenum param)
    {
    ALint64SOFT val[2];
    ud_t *ud = userdata(source);
    CheckContextPfn(L, ud, GetSourcei64vSOFT);
    ud->cdt->GetSourcei64vSOFT(source->name, param, val);
    CheckErrorAl(L);
    lua_pushinteger(L, val[0]);
    lua_pushinteger(L, val[1]);
    return 2;
    }

static int GetOrientation(lua_State *L, source_t source)
    {
    ALfloat val[6];
    al.GetSourcefv(source->name, AL_ORIENTATION, val);
    CheckErrorAl(L);
    pushfloat3(L, val);  /* "at" */
    pushfloat3(L, &val[3]); /* "up" */
    return 2;
    }

static int SetOrientation(lua_State *L, source_t source)
    {
    ALfloat val[6];
    checkfloat3(L, 3, val); /* "at" */
    checkfloat3(L, 4, &val[3]); /* "up" */
    al.Sourcefv(source->name, AL_ORIENTATION, val);
    CheckErrorAl(L);
    return 0;
    }


static int GetStereoAngles(lua_State *L, source_t source)
    {
    ALfloat val[2];
    al.GetSourcefv(source->name, AL_STEREO_ANGLES, val);
    CheckErrorAl(L);
    lua_pushnumber(L, val[0]);
    lua_pushnumber(L, val[1]);
    return 2;
    }

static int SetStereoAngles(lua_State *L, source_t source)
    {
    ALfloat val[2];
    val[0] = luaL_checknumber(L, 3);
    val[1] = luaL_checknumber(L, 4);
    al.Sourcefv(source->name, AL_STEREO_ANGLES, val);
    CheckErrorAl(L);
    return 0;
    }


#define GET_ENUM_FUNC(FuncName, param, enumtype)        \
static int FuncName(lua_State *L, source_t source)      \
    {                                                   \
    ALenum val;                                         \
    al.GetSourcei(source->name, param, &val);           \
    CheckErrorAl(L);                                    \
    push##enumtype(L, val);                             \
    return 1;                                           \
    }

GET_ENUM_FUNC(GetType, AL_SOURCE_TYPE, sourcetype)
GET_ENUM_FUNC(GetState, AL_SOURCE_STATE, sourcestate)
GET_ENUM_FUNC(GetDistanceModel, AL_DISTANCE_MODEL, distancemodel)
GET_ENUM_FUNC(GetResampler, AL_SOURCE_RESAMPLER_SOFT, resampler)
GET_ENUM_FUNC(GetSpatializeMode, AL_SOURCE_SPATIALIZE_SOFT, spatializemode )
#undef GET_ENUM_FUNC


#define SET_ENUM_FUNC(FuncName, param, enumtype)        \
static int FuncName(lua_State *L, source_t source)      \
    {                                                   \
    ALenum val;                                         \
    val = check##enumtype(L, 3);                        \
    al.Sourcei(source->name, param, val);               \
    CheckErrorAl(L);                                    \
    return 0;                                           \
    }

SET_ENUM_FUNC(SetDistanceModel, AL_DISTANCE_MODEL, distancemodel)
SET_ENUM_FUNC(SetResampler, AL_SOURCE_RESAMPLER_SOFT, resampler)
SET_ENUM_FUNC(SetSpatializeMode, AL_SOURCE_SPATIALIZE_SOFT, spatializemode )
#undef GETSET_ENUM_FUNC


#define GET_OBJECT_FUNC(FuncName, param, what)          \
static int FuncName(lua_State *L, source_t source)      \
    {                                                   \
    ALint name;                                         \
    what##_t obj;                                       \
    al.GetSourcei(source->name, param, &name);          \
    CheckErrorAl(L);                                    \
    obj = search##what(L, name, NULL);                  \
    if(!obj) return 0;                                  \
    push##what(L, obj);                                 \
    return 1;                                           \
    }                                                   \

GET_OBJECT_FUNC(GetBuffer, AL_BUFFER, buffer)
GET_OBJECT_FUNC(GetDirectFilter, AL_DIRECT_FILTER, filter)
#undef GETSET_OBJECT_FUNC


#define SET_OBJECT_FUNC(FuncName, param, what)          \
static int FuncName(lua_State *L, source_t source)      \
    {                                                   \
    ALint name;                                         \
    what##_t obj;                                       \
    obj = test##what(L, 3, NULL);                       \
    name = obj ? obj->name : 0;                         \
    al.Sourcei(source->name, param, name);              \
    CheckErrorAl(L);                                    \
    return 0;                                           \
    }

SET_OBJECT_FUNC(SetBuffer, AL_BUFFER, buffer)
SET_OBJECT_FUNC(SetDirectFilter, AL_DIRECT_FILTER, filter)
#undef GETSET_OBJECT_FUNC


static int GetAuxiliarySendFilter(lua_State *L, source_t source) 
    {
    filter_t filter;
    auxslot_t auxslot;
    ALint val[3]; /* 0=auxslot name, 1=aux send number, 2=filter name (opt) */
    al.GetSourceiv(source->name, AL_AUXILIARY_SEND_FILTER, val);
    CheckErrorAl(L);
    auxslot = searchauxslot(L, val[0], NULL);
    if(auxslot) pushauxslot(L, auxslot); else lua_pushnil(L);
    lua_pushinteger(L, val[1]);
    filter = searchfilter(L, val[2], NULL);
    if(filter) pushfilter(L, filter); else lua_pushnil(L);
    return 3;
    }

static int SetAuxiliarySendFilter(lua_State *L, source_t source) 
    {
    filter_t filter;
    auxslot_t auxslot;
    ALint val[3]; /* 0=auxslot name, 1=aux send number, 2=filter name (opt) */
    auxslot = testauxslot(L, 3, NULL);
    val[0] = auxslot ? auxslot->name : 0;
    val[1] = luaL_checkinteger(L, 4);
    filter = testfilter(L, 5, NULL);
    val[2] = filter ? filter->name : 0;
    al.Sourceiv(source->name, AL_AUXILIARY_SEND_FILTER, val);
    CheckErrorAl(L);
    return 0;
    }

static int GetSource(lua_State *L)
    {
    source_t source = checksource(L, 1, NULL);
    ALenum param = checkalparam(L, 2);
    switch(param)
        {
        case AL_PITCH:
        case AL_CONE_INNER_ANGLE:
        case AL_CONE_OUTER_ANGLE:
        case AL_GAIN: 
        case AL_MAX_DISTANCE:
        case AL_ROLLOFF_FACTOR:
        case AL_REFERENCE_DISTANCE:
        case AL_MIN_GAIN:
        case AL_MAX_GAIN:
        case AL_CONE_OUTER_GAIN:
        case AL_CONE_OUTER_GAINHF:
        case AL_AIR_ABSORPTION_FACTOR:
        case AL_ROOM_ROLLOFF_FACTOR:
        case AL_DOPPLER_FACTOR:
        case AL_SEC_OFFSET:
        case AL_SAMPLE_OFFSET:
        case AL_BYTE_OFFSET:
        case AL_SOURCE_RADIUS:
        case AL_SEC_LENGTH_SOFT:
                            return GetFloat(L, source, param);
        case AL_POSITION:
        case AL_VELOCITY:
        case AL_DIRECTION:
                            return GetFloat3(L, source, param);
        case AL_SOURCE_RELATIVE:
        case AL_LOOPING:
        case AL_DIRECT_FILTER_GAINHF_AUTO:
        case AL_AUXILIARY_SEND_FILTER_GAIN_AUTO:
        case AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO:
        case AL_DIRECT_CHANNELS_SOFT:
                            return GetBoolean(L, source, param);
        case AL_BUFFERS_QUEUED:
        case AL_BUFFERS_PROCESSED:
        case AL_BYTE_LENGTH_SOFT:
        case AL_SAMPLE_LENGTH_SOFT:
                            return GetInteger(L, source, param);
        case AL_SEC_OFFSET_LATENCY_SOFT:
                            return GetDouble2(L, source, param);
        case AL_SAMPLE_OFFSET_LATENCY_SOFT:
                            return GetI64_2(L, source, param);
        case AL_ORIENTATION: return GetOrientation(L, source);
        case AL_STEREO_ANGLES: return GetStereoAngles(L, source);
        case AL_SOURCE_TYPE:    return GetType(L, source);
        case AL_SOURCE_STATE:   return GetState(L, source);
        case AL_DISTANCE_MODEL: return GetDistanceModel(L, source);
        case AL_SOURCE_RESAMPLER_SOFT:  return GetResampler(L, source);
        case AL_SOURCE_SPATIALIZE_SOFT: return GetSpatializeMode(L, source);
        case AL_BUFFER: return GetBuffer(L, source);
        case AL_DIRECT_FILTER: // not supported by OpenAL-SOFT
            return GetDirectFilter(L, source);
        case AL_AUXILIARY_SEND_FILTER: // not supported by OpenAL-SOFT
            return GetAuxiliarySendFilter(L, source);
        default: return erralparam(L, 2);
        }
    return 0;
    }

static int SetSource(lua_State *L)
    {
    source_t source = checksource(L, 1, NULL);
    ALenum param = checkalparam(L, 2);
    switch(param)
        {
        case AL_PITCH:
        case AL_CONE_INNER_ANGLE:
        case AL_CONE_OUTER_ANGLE: 
        case AL_GAIN: 
        case AL_MAX_DISTANCE:
        case AL_ROLLOFF_FACTOR:
        case AL_REFERENCE_DISTANCE:
        case AL_MIN_GAIN:
        case AL_MAX_GAIN:
        case AL_CONE_OUTER_GAIN:
        case AL_CONE_OUTER_GAINHF:
        case AL_AIR_ABSORPTION_FACTOR:
        case AL_ROOM_ROLLOFF_FACTOR:
        case AL_DOPPLER_FACTOR:
        case AL_SEC_OFFSET:
        case AL_SAMPLE_OFFSET:
        case AL_BYTE_OFFSET:
        case AL_SOURCE_RADIUS:
                            return SetFloat(L, source, param);
        case AL_POSITION:
        case AL_VELOCITY:
        case AL_DIRECTION:
                            return SetFloat3(L, source, param);
        case AL_SOURCE_RELATIVE:
        case AL_LOOPING:
        case AL_DIRECT_FILTER_GAINHF_AUTO:
        case AL_AUXILIARY_SEND_FILTER_GAIN_AUTO:
        case AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO:
        case AL_DIRECT_CHANNELS_SOFT:
                            return SetBoolean(L, source, param);
        case AL_ORIENTATION: return SetOrientation(L, source);
        case AL_STEREO_ANGLES: return SetStereoAngles(L, source);
        case AL_DISTANCE_MODEL: return SetDistanceModel(L, source);
        case AL_SOURCE_RESAMPLER_SOFT:  return SetResampler(L, source);
        case AL_SOURCE_SPATIALIZE_SOFT: return SetSpatializeMode(L, source);
        case AL_BUFFER: return SetBuffer(L, source);
        case AL_DIRECT_FILTER:  return SetDirectFilter(L, source);
        case AL_AUXILIARY_SEND_FILTER:  return SetAuxiliarySendFilter(L, source);
        case AL_BUFFERS_QUEUED:
        case AL_BUFFERS_PROCESSED:
        case AL_BYTE_LENGTH_SOFT:
        case AL_SAMPLE_LENGTH_SOFT:
        case AL_SEC_LENGTH_SOFT:
        case AL_SEC_OFFSET_LATENCY_SOFT:
        case AL_SAMPLE_OFFSET_LATENCY_SOFT:
        case AL_SOURCE_TYPE:
        case AL_SOURCE_STATE:
                            return errreadonlyparam(L, 2);
        default: return erralparam(L, 2);
        }
    return 0;
    }


RAW_FUNC(source)
TYPE_FUNC(source)
PARENT_FUNC(source)
DELETE_FUNC(source)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "delete", Delete },
        { "get", GetSource },
        { "set", SetSource },
        { "play", SourcePlay },
        { "stop", SourceStop },
        { "pause", SourcePause },
        { "rewind", SourceRewind },
        { "queue_buffers", SourceQueueBuffers },
        { "unqueue_buffers", SourceUnqueueBuffers },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_source", Create},
        { "delete_source", Delete },
        { "source_get", GetSource },
        { "source_set", SetSource },
        { "source_play", SourcePlay },
        { "source_stop", SourceStop },
        { "source_pause", SourcePause },
        { "source_rewind", SourceRewind },
        { "source_queue_buffers", SourceQueueBuffers },
        { "source_unqueue_buffers", SourceUnqueueBuffers },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_source(lua_State *L)
    {
    udata_define(L, SOURCE_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

